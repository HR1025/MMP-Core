#include "AbstractConfig.h"

#include <atomic>
#include <thread>
#include <mutex>
#include <map>
#include <condition_variable>
#include <Poco/File.h>
#include <Poco/SingletonHolder.h>
#include <Poco/Util/IniFileConfiguration.h>

#include "Common/ThreadPool.h"
#include "Codec/CodecConfig.h"
#include "GPU/GL/GLDrawContex.h"
#include "GPU/Windows/WindowFactory.h"

#ifdef USE_API_RPC
#include "RPC/RPCProxy.h"
#endif /* USE_API_RPC */

namespace Mmp
{
namespace Config
{

static AbstractLogger::Level GetLogLevelFromStr(const std::string& levelStr)
{
    AbstractLogger::Level level = AbstractLogger::Level::L_INFO;
    if (levelStr == "TRACE")
    {
        level = AbstractLogger::Level::L_TRACE;
    }
    else if (levelStr == "DEBUG")
    {
        level = AbstractLogger::Level::L_DEBUG;
    }
    else if (levelStr == "INFO")
    {
        level = AbstractLogger::Level::L_INFO;
    }
    else if (levelStr == "WARN")
    {
        level = AbstractLogger::Level::L_WARN;
    }
    else if (levelStr == "ERROR")
    {
        level = AbstractLogger::Level::L_ERROR;
    }
    else if (levelStr == "FATAL")
    {
        level = AbstractLogger::Level::L_FATAL;
    }
    return level;
}

static Mmp::GPUBackend GetGPUBackendFromStr(const std::string& backendStr)
{
    if (backendStr == "OPENGL")
    {
        return Mmp::GPUBackend::OPENGL;
    }
    else if (backendStr == "OPENGL_ES")
    {
        return Mmp::GPUBackend::OPENGL_ES;
    }
    else if (backendStr == "D3D11")
    {
        return Mmp::GPUBackend::D3D11;
    }
    else
    {
        return Mmp::GPUBackend::OPENGL;
    }
}

} // namespace Config
} // namespace Mmp

namespace Mmp
{
namespace Config
{

class AbstractConfigImpl : public AbstractConfig
{
public:
    AbstractConfigImpl();
    ~AbstractConfigImpl();
public:
    void Load(const std::string& iniPath) override;
    void Init() override;
    void Uninit() override;
    void Set(const std::string& section, const std::string& key, Any value) override;
    Any Get(const std::string& section, const std::string& key) override;
private:
    std::atomic<bool> _isInited;
    std::atomic<bool> _gpuInited;
    std::map<std::pair<std::string/* section */, std::string /* key */>, Any> _iniMap;
private: /* gpu */
    std::thread _renderThread;
    AbstractWindows::ptr _window;
    GLDrawContex::ptr    _draw;
};

AbstractConfigImpl::AbstractConfigImpl()
{
    _isInited = false;
    _gpuInited = false;
    // default conifg
    {
        /* log section */ 
        _iniMap[{"log", "level"}] = std::string("INFO");
        _iniMap[{"log", "with_console"}] = false;
        _iniMap[{"log", "log_file"}] = std::string();
        _iniMap[{"log", "max_slice_num"}] = 2;
        _iniMap[{"log", "max_slice_size"}] = 10 * 1024;

        /* gpu section */
        _iniMap[{"gpu", "with_gpu"}] = true;
        _iniMap[{"gpu", "on_screen"}] = true;
        _iniMap[{"gpu", "windows"}] = std::string();
        _iniMap[{"gpu", "gl_backend"}] = std::string();

        /* asset section */
        _iniMap[{"asset", "asset_path"}] = std::string();
        _iniMap[{"asset", "spriv"}] = std::string("SPRIV");
        _iniMap[{"asset", "transition"}] = std::string("TRANSITION");

        /* rpc section */
        _iniMap[{"rpc", "enable"}] = true;
        _iniMap[{"rpc", "with_http"}] = true;
        _iniMap[{"rpc", "http_port"}] = 7120;
    }
}

AbstractConfigImpl::~AbstractConfigImpl()
{
    Uninit();
}

void AbstractConfigImpl::Load(const std::string& iniPath)
{
#define INI_BOOL_VALUE(tag, section, key)    if (pConf->hasProperty(tag))\
                                             {\
                                                 _iniMap[{section, key}] = pConf->getBool(tag);\
                                             }
#define INI_STR_VALUE(tag, section, key)     if (pConf->hasProperty(tag))\
                                             {\
                                                 _iniMap[{section, key}] = pConf->getString(tag);\
                                             }
#define INI_INT_VALUE(tag, section, key)       if (pConf->hasProperty(tag))\
                                             {\
                                                 _iniMap[{section, key}] = pConf->getInt(tag);\
                                             }

    if (!iniPath.empty())
    {
        Poco::File file(iniPath);
        if (file.exists() && file.isFile())
        {
            Poco::AutoPtr<Poco::Util::IniFileConfiguration> pConf = new Poco::Util::IniFileConfiguration(iniPath);

            /* log section */
            INI_STR_VALUE("log.level", "log", "level");
            INI_BOOL_VALUE("log.with_console", "log", "with_console");
            INI_STR_VALUE("log.log_file", "log", "log_file");
            INI_INT_VALUE("log.max_slice_num", "log", "max_slice_num");
            INI_INT_VALUE("log.max_slice_size", "log", "max_slice_size");

            /* gpu section */
            INI_BOOL_VALUE("gpu.with_gpu", "gpu", "with_gpu");
            INI_BOOL_VALUE("gpu.on_screen", "gpu", "on_screen");
            INI_STR_VALUE("gpu.windows", "gpu", "windows");
            INI_STR_VALUE("gpu.gl_backend", "gpu", "gl_backend");

            /* asset section */
            INI_STR_VALUE("asset.asset_path", "asset", "asset_path");
            INI_STR_VALUE("asset.spriv", "asset", "SPRIV");
            INI_STR_VALUE("asset.transition", "asset", "TRANSITION");

            /* rpc section */
            INI_BOOL_VALUE("rpc.enable", "rpc", "enable");
            INI_BOOL_VALUE("rpc.with_http", "rpc", "with_http");
            INI_INT_VALUE("rpc.http_port", "rpc", "http_port");
        }
    }

#undef INI_BOOL_VALUE
#undef INI_STR_VALUE
#undef INI_STR_INT
}

void AbstractConfigImpl::Set(const std::string& section, const std::string& key, Any value)
{
    assert(!_isInited);
    
    if (_iniMap.count({section, key}))
    {
        _iniMap[{section, key}] = value;
    }
    else
    {
        assert(false);
    }
}

Any AbstractConfigImpl::Get(const std::string& section, const std::string& key)
{

    if (_iniMap.count({section, key}))
    {
        return _iniMap[{section, key}];
    }
    else
    {
        assert(false);
        return Any();
    }
}

void AbstractConfigImpl::Init()
{
    assert(!_isInited);
    _isInited = true;
    
    Mmp::Codec::CodecConfig::Instance()->Init();

#define INI_VALUE_BOOL(section, key)         AnyCast<bool>(_iniMap[{section, key}])
#define INI_VALUE_STR(section, key)          AnyCast<std::string>(_iniMap[{section, key}])
#define INI_VALUE_INT(section, key)          AnyCast<int>(_iniMap[{section, key}])

    // 初始化日志相关配置
    {
        if (INI_VALUE_BOOL("log", "with_console"))
        {
            AbstractLogger::LoggerSingleton()->Enable(Mmp::AbstractLogger::Direction::CONSLOE);
        }
        if (!INI_VALUE_STR("log", "log_file").empty())
        {
            AbstractLogger::LoggerSingleton()->SetFilePath(INI_VALUE_STR("log", "log_file"));
            AbstractLogger::LoggerSingleton()->SetMaxSliceNum(INI_VALUE_INT("log", "max_slice_num") - 1);
            AbstractLogger::LoggerSingleton()->SetMaxSliceSize(INI_VALUE_INT("log", "max_slice_size"));
            AbstractLogger::LoggerSingleton()->Enable(Mmp::AbstractLogger::Direction::FILE);
        }
        AbstractLogger::LoggerSingleton()->SetThreshold(GetLogLevelFromStr(INI_VALUE_STR("log", "level")));
    }

    
    // 初始化线程池
    ThreadPool::ThreadPoolSingleton()->Init();

#ifdef USE_API_RPC
    // 初始化 RPC 先关配置
    if (INI_VALUE_BOOL("rpc", "enable"))
    {
        if (INI_VALUE_BOOL("rpc", "with_http"))
        {
            Mmp::RPC::RPCProxy::Instance()->AddDelegate("HTTPRPC");
        }
        Mmp::RPC::RPCProxy::Instance()->Init();
        Mmp::RPC::RPCProxy::Instance()->Start();
    }
#endif /* USE_API_RPC */

    // 初始化 GPU 相关配置
    if (INI_VALUE_BOOL("gpu", "with_gpu"))
    {
        _renderThread = std::thread([this]() -> void
        {
                GLDrawContex::SetGPUBackendType(GetGPUBackendFromStr(INI_VALUE_STR("gpu", "gl_backend")));
                _window = WindowFactory::DefaultFactory().createWindow(INI_VALUE_STR("gpu", "windows"));
                _window->SetRenderMode(INI_VALUE_BOOL("gpu", "on_screen"));
                _window->Open();
                _window->BindRenderThread(true);
                _draw = GLDrawContex::Instance();
                if (GLDrawContex::GetGpuBackendType() == GPUBackend::D3D11)
                {
                    _draw->Set("DXGIWindow", _window);
                }
                _gpuInited = true;
                _draw->ThreadStart();
                while (_isInited)
                {
                    GpuTaskStatus status;
                    status = _draw->ThreadFrame();
                    if (status == GpuTaskStatus::EXIT)
                    {
                        break;
                    }
                    else if (status == GpuTaskStatus::PRESENT)
                    {
                        _window->Swap();
                    }
                    else
                    {
                        continue;
                    }
                };
                _draw->ThreadEnd();
                _window->BindRenderThread(false);
                _window->Close();
        });
        while (!_gpuInited)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(200));
        }
        
    }

#undef INI_VALUE_BOOL
#undef INI_VALUE_STR
#undef INI_VALUE_INT
}

void AbstractConfigImpl::Uninit()
{
#define INI_VALUE_BOOL(section, key)         AnyCast<bool>(_iniMap[{section, key}])
#define INI_VALUE_STR(section, key)          AnyCast<std::string>(_iniMap[{section, key}])
#define INI_VALUE_INT(section, key)          AnyCast<int>(_iniMap[{section, key}])
    if (_isInited)
    {
        if (INI_VALUE_BOOL("gpu", "with_gpu"))
        {
            _draw->ThreadStop();
            _renderThread.join();
        }
        _isInited = false;
    }

#ifdef USE_API_RPC
    if (INI_VALUE_BOOL("rpc", "enable"))
    {
        Mmp::RPC::RPCProxy::Instance()->Stop();
        Mmp::RPC::RPCProxy::Instance()->Uninit();
    }
#endif /* USE_API_RPC */

    ThreadPool::ThreadPoolSingleton()->Uninit();

    Mmp::Codec::CodecConfig::Instance()->Uninit();

#undef INI_VALUE_BOOL
#undef INI_VALUE_STR
#undef INI_VALUE_INT
}

AbstractConfig* AbstractConfig::ConfigSingleton()
{
    static AbstractConfigImpl sh;
    return &sh;
}

} // namespace Config
} // namespace Mmp