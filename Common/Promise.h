//
// Task.h
//
// Library: Thread
// Package: Task
// Module:  Task
// 

#pragma once

#include <mutex>
#include <atomic>
#include <chrono>
#include <memory>
#include <cassert>
#include <cstdint>
#include <functional>
#include <condition_variable>

#include "Task.h"

namespace Mmp
{

/**
 * @brief 任务类型
 * @note  PASS 用于已经存在执行结果时 (任务不需要执行,或者已经执行完成)
 */
enum class PromiseType
{
    PASS,
    SYNC,    /* 同步 */
    ASYNC    /* 异步 */
};

/**
 * @brief  任务状态
 */
enum class PromiseSatus
{
    PENDING,    /* 挂起待运行 */
    RUNING,     /* 正在运行 */
    COMPLETE,   /* 已经完成 */
    CANCLE      /* 被取消 */
};

/**
 * @brief Promise 异步编程范式
 * @note  在标准 Promise Async 编程之上,同时支持 Sync 以及 Pass 模式,
 *        以此对外屏蔽线程模型
 */
template<class T>
class Promise : public Task
{
public:
    using ptr = std::shared_ptr<Promise>;
public:
    using Task = std::function<T /* data */ ()>;
    using OnReceived = std::function<void (const T& /* data */, PromiseSatus /* status */)>;
public:
    explicit Promise(T data)
    {
        Init();
        _type = PromiseType::PASS;
        _status = PromiseSatus::COMPLETE;
        _isReceived = true;
        _data = data;
    }
    Promise(const Task& task, const OnReceived& onReceived)
    {
        Init();
        _type = PromiseType::ASYNC;
        _task = task;
        _onReceived = onReceived;
    }
    explicit Promise(const Task& task, const PromiseType& type = PromiseType::ASYNC)
    {
        Init();
        assert(type != PromiseType::PASS);
        _type = type;
        _task = task;
        if (type == PromiseType::SYNC)
        {
            _data = task();
            _isReceived = true;
            _status = PromiseSatus::COMPLETE;
        }
        else
        {
            // Hint : nothing to do, wait async schedule
        }
    }
private:
    void Init()
    {
        _type = PromiseType::PASS;
        _isReceived = false;
        _status = PromiseSatus::PENDING;
        _task = [this]() -> T
        {
            return _data;
        };
        _onReceived = [](const T& /* data */, PromiseSatus /* status */) -> void
        {

        };
    }
public:
    void Run() override
    {
        if (_status == PromiseSatus::PENDING)
        {
            {
                std::lock_guard<std::mutex> lock(_mtx);
                _status = PromiseSatus::RUNING;
            }
            _data = _task();
            {
                std::lock_guard<std::mutex> lock(_mtx);
                _isReceived = true;
                _status = PromiseSatus::COMPLETE;
                _onReceived(_data, _status);
                _cond.notify_all();
            }
        }
    }
public:
    bool Wait(T& data, uint32_t waitTimeMs = 0 /* 0 means block */)
    {
        std::unique_lock<std::mutex> lock(_mtx);
        if (waitTimeMs == 0)
        {
            if (!_isReceived && _status != PromiseSatus::CANCLE)
            {
                _cond.wait(lock, [&] {return _isReceived;});
                data = _data;
                return true;
            }
            else
            {
                return true;
            }
        }
        else
        {
            if (!_isReceived && _status != PromiseSatus::CANCLE)
            {
                _cond.wait_for(lock, std::chrono::milliseconds(waitTimeMs));
            }
            if (_isReceived)
            {
                data = _data;
            }
            return _isReceived ? true : false;
        }
    }
    bool Cancle()
    {
        std::lock_guard<std::mutex> lock(_mtx);
        if (_status == PromiseSatus::RUNING) /* 无法停止已运行任务 */
        {
            return false;
        }
        else if (_status == PromiseSatus::PENDING)
        {
            _status = PromiseSatus::CANCLE;
            _onReceived(_data, _status);
            _cond.notify_all();
            return true;
        }
        else
        {
            return true;
        }
    }
    PromiseSatus Stauts() const
    {
        return _status;
    }
private:
    std::mutex                  _mtx;
    std::condition_variable     _cond;
    PromiseType                 _type;
    T                           _data;
    bool                        _isReceived;
    std::atomic<PromiseSatus>   _status;
    Task                        _task;
    OnReceived                  _onReceived;
};

template<>
class Promise<void> : public Task
{
public:
    using ptr = std::shared_ptr<Promise>;
public:
    using Task = std::function<void /* data */ ()>;
    using OnReceived = std::function<void (PromiseSatus /* status */)>;
public:
    Promise(const Task& task, const OnReceived& onReceived)
    {
        Init();
        _type = PromiseType::ASYNC;
        _task = task;
        _onReceived = onReceived;
    }
    explicit Promise(const Task& task, const PromiseType& type = PromiseType::ASYNC)
    {
        Init();
        assert(type != PromiseType::PASS);
        _type = type;
        _task = task;
        if (type == PromiseType::SYNC)
        {
            task();
            _isReceived = true;
            _status = PromiseSatus::COMPLETE;
        }
        else
        {
            // Hint : nothing to do, wait async schedule
        }
    }
private:
    void Init()
    {
        _type = PromiseType::PASS;
        _isReceived = false;
        _status = PromiseSatus::PENDING;
        _task = []() -> void
        {
        };
        _onReceived = [](PromiseSatus /* status */) -> void
        {

        };
    }
public:
    void Run() override
    {
        if (_status == PromiseSatus::PENDING)
        {
            {
                std::lock_guard<std::mutex> lock(_mtx);
                _status = PromiseSatus::RUNING;
            }
            _task();
            {
                std::lock_guard<std::mutex> lock(_mtx);
                _isReceived = true;
                _status = PromiseSatus::COMPLETE;
                _onReceived(_status);
                _cond.notify_all();
            }
        }
    }
public:
    bool Wait(uint32_t waitTimeMs = 0 /* 0 means block */)
    {
        std::unique_lock<std::mutex> lock(_mtx);
        if (waitTimeMs == 0)
        {
            if (!_isReceived && _status != PromiseSatus::CANCLE)
            {
                _cond.wait(lock, [&] {return _isReceived;});
                return true;
            }
            else
            {
                return true;
            }
        }
        else
        {
            if (!_isReceived && _status != PromiseSatus::CANCLE)
            {
                _cond.wait_for(lock, std::chrono::microseconds(waitTimeMs));
            }
            return _isReceived ? true : false;
        }
    }
    bool Cancle()
    {
        std::lock_guard<std::mutex> lock(_mtx);
        if (_status == PromiseSatus::RUNING) /* 无法停止已运行任务 */
        {
            return false;
        }
        else if (_status == PromiseSatus::PENDING)
        {
            _status = PromiseSatus::CANCLE;
            _onReceived(_status);
            _cond.notify_all();
            return true;
        }
        else
        {
            return true;
        }
    }
    PromiseSatus Stauts() const
    {
        return _status;
    }
private:
    std::mutex                  _mtx;
    std::condition_variable     _cond;
    PromiseType                 _type;
    bool                        _isReceived;
    std::atomic<PromiseSatus>   _status;
    Task                        _task;
    OnReceived                  _onReceived;
};

} // namespace Mmp