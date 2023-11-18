//
// AbstractLogger.h
//
// Library: Common
// Package: Log
// Module:  Logger
// 
#pragma once

#include <cstdint>
#include <map>
#include <atomic>
#include <string>
#include <functional>

namespace Mmp
{

/**
 * @brief    日志器
 * @note     非线程安全
 */
class AbstractLogger
{
public:
    /**
     * @brief 日志传输方向
     */
    enum class Direction
    {
        CONSLOE,
        FILE,
        CUSTOM
    };
    /**
     * @brief 日志等级
     * @note  FATAL 在某些情况下会调用 assert 捕获(暂未实现)
     */
    enum class Level
    {
        L_TRACE,
        L_DEBUG,
        L_INFO,
        L_WARN,
        L_ERROR,
        L_FATAL
    };
public:
    virtual ~AbstractLogger() = default;
    /**
     * @brief 使能
     */
    virtual void Enable(Direction direction) = 0;
    /**
     * @brief 关闭
     */
    virtual void Disable(Direction direction) = 0;
    /**
     * @brief     日志输出
     * @param[in] line
     * @param[in] fileName
     * @param[in] level
     * @param[in] module
     * @param[in] msg  
     * @sa        LogCallback
     */
    virtual void Log(uint32_t line, const std::string& fileName, Level level, const std::string& module, const std::string& msg) = 0;
public: /* file log channel */
    /**
     * @brief       设置日志文件路径
     * @param[in]   filePath
     * @note        针对于 Direction::FILE
     */
    virtual void SetFilePath(const std::string& filePath) = 0;
    /**
     * @brief      设置最大分片数量
     * @param[in]  maxSlice : 最大分量数量
     * @note       默认为 1
     */
    virtual void SetMaxSliceNum(uint64_t maxSliceMun) = 0;
    /**
     * @brief      设置单个分片最大大小
     * @param[in]  maxSliceSize : kb
     * @note       默认为 10M, 即 10 * 1024
     */
    virtual void SetMaxSliceSize(uint64_t maxSliceSize) = 0;
public: /* custom log channel */
    /**
     * @brief  日志回调
     * @param  line      : 行
     * @param  fileName  : 文件名
     * @param  module    : 模块标识
     * @param  tid       : 线程号
     * @param  pid       : 进程号(不支持)
     * @param  msg       : 日志信息
     */
    using LogCallback = std::function<void(uint32_t line, const std::string& fileName, Level level, const std::string& module, uint64_t tid, uint64_t pid, const std::string& msg)>;
    /**
     * @brief       设置日志回调
     * @param[in]   logCallback
     * @note        针对于 Direction::CUSTOM
     */
    virtual void SetCallback(const LogCallback& logCallback) = 0;
public:
    /**
     * @brief     设置日志等级阈值
     * @param[in] threshold
     */
    virtual void SetThreshold(Level threshold) = 0;
    /**
     * @brief     获取日志等级阈值
     * @return    Level
     */
    virtual Level GetThreshold() = 0;
    /**
     * @brief 单例获取
     */
    static AbstractLogger* LoggerSingleton();
};

// Hint : 等于逻辑复用枚举自身的比较
bool operator<(AbstractLogger::Level left, AbstractLogger::Level right);
bool operator>(AbstractLogger::Level left, AbstractLogger::Level right);
bool operator<=(AbstractLogger::Level left, AbstractLogger::Level right);
bool operator>=(AbstractLogger::Level left, AbstractLogger::Level right);
} // namespace Mmp