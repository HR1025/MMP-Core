#include "LogMessage.h"

#include <Poco/Path.h>

namespace Mmp
{

LogMessage::LogMessage(AbstractLogger::Level level, const std::string& fileName, uint32_t line, const std::string& module)
{
    _level    = level;
    if (_level >= AbstractLogger::LoggerSingleton()->GetThreshold())
    {
        _fileName = Poco::Path(fileName).getFileName();
        _line     = line;
        _module   = module;
    }

}

LogMessage::~LogMessage()
{
    if (_level >= AbstractLogger::LoggerSingleton()->GetThreshold())
    {
        AbstractLogger::LoggerSingleton()->Log(_line, _fileName, _level, _module, _ss.str());
    }
}

} // namespace Mmp