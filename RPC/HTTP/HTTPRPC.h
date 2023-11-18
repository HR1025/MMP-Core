#pragma once

#include "AbstractRPC.h"

#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>

namespace Mmp
{
namespace RPC
{

class HTTPRPC : public AbstractRPC
{
public:
    HTTPRPC();
    ~HTTPRPC();
public:
    bool Init() override;
    void Uninit() override;
    bool Start() override;
    void Stop() override;
public:
    void OnResponse(Any user, SharedData::ptr response) override;
    void OnNotFoundDispatcher(Any user, const std::string& strategy) override;
private:
    int64_t _port;
    std::shared_ptr<Poco::Net::HTTPServer> _httpServer;
    std::shared_ptr<Poco::Net::HTTPRequestHandlerFactory> _requestHandlerFactory;
};

} // namespace RPC
} // namespace Mmp