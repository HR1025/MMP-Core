#include "HTTPRPC.h"

#include <functional>
#include <Poco/StreamCopier.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

#include "Common/StringAllocateMethod.h"
#include "Common/SharedData.h"

namespace Mmp
{
namespace RPC
{

using OnHandleRequest = std::function<void(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)>;

class HTTPRequestHandlerApi : public Poco::Net::HTTPRequestHandler
{
public:
    HTTPRequestHandlerApi(OnHandleRequest onHandleRequest)
    {
        _onHandleRequest = onHandleRequest;
    }
    ~HTTPRequestHandlerApi() = default;
public:
    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) override
    {
        _onHandleRequest(request, response);
    }
private:
    OnHandleRequest _onHandleRequest;
};


class HTTPRequestHandlerFactoryRPC : public Poco::Net::HTTPRequestHandlerFactory
{
public:
    explicit HTTPRequestHandlerFactoryRPC(OnHandleRequest onHandleRequest)
    {
        _onHandleRequest = onHandleRequest;
    }
public:
    Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& /* request */) override
    {
        return new HTTPRequestHandlerApi(_onHandleRequest);
    }
private:
    OnHandleRequest _onHandleRequest;
};

std::shared_ptr<Poco::Net::HTTPRequestHandlerFactory> CreateHTTPRequestHandlerFactory( OnHandleRequest onHandleRequest)
{
    return std::make_shared<HTTPRequestHandlerFactoryRPC>(onHandleRequest);
}

HTTPRPC::HTTPRPC()
{
    _port = 0;
}

HTTPRPC::~HTTPRPC()
{

}

bool HTTPRPC::Init()
{
    _port = 8080;
    return true;
}

void HTTPRPC::Uninit()
{

}

bool HTTPRPC::Start()
{
    auto onHandleRequest = [this](Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
    {
        std::string url = request.getURI();
        std::shared_ptr<StringAllocateMethod> alloc = std::make_shared<StringAllocateMethod>();
        {
            std::stringstream ss;
            Poco::StreamCopier::copyStream(request.stream(), ss);
            alloc->_data = ss.str();
        }
        SharedData::ptr data = std::make_shared<SharedData>(alloc->_data.size(), alloc);
        this->DoRequest(&response, url, data, true);
    };
    
    _requestHandlerFactory = CreateHTTPRequestHandlerFactory(onHandleRequest);
    Poco::Net::ServerSocket svs = Poco::Net::ServerSocket((Poco::UInt16)_port);
    Poco::Net::HTTPServerParams* params = new Poco::Net::HTTPServerParams;
    {
        params->setMaxThreads(4);
        params->setServerName("MMP");
    }
    _httpServer = std::make_shared<Poco::Net::HTTPServer>(
                        Poco::SharedPtr<Poco::Net::HTTPRequestHandlerFactory>(_requestHandlerFactory.get()), svs, params);
    _httpServer->start();
    return true;
}

void HTTPRPC::Stop()
{
    _httpServer->stopAll();
    _httpServer.reset();
    _requestHandlerFactory.reset();
}

void HTTPRPC::OnResponse(Any user, AbstractSharedData::ptr response)
{
    Poco::Net::HTTPServerResponse& _response = *RefAnyCast<Poco::Net::HTTPServerResponse*>(user);
    if (response)
    {
        _response.send().write((const char*)response->GetData(), response->GetSize());
    }
    _response.send() << std::flush;
}

void HTTPRPC::OnNotFoundDispatcher(Any user, const std::string& strategy)
{
    Poco::Net::HTTPServerResponse& response = *RefAnyCast<Poco::Net::HTTPServerResponse*>(user);
    response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
    response.send() << std::flush;
}

} // namespace RPC
} // namespace Mmp