//
// EventDispatcher.h
//
// Library: Event
// Package: Event
// Module:  Dispatcher
// 
// Note : This file is modified from ZLMediaKit/3rdpart/ZLToolKit/src/Util/NoticeCenter.h (ZLMediaKit)
//        Date : 2023-10-05
//        Hash : 48f62810c7f480859623898227af2e0c05eff9b6
// Copyright (c) 2016 The ZLToolKit project authors. All Rights Reserved.
//

#pragma once

#include <mutex>
#include <memory>
#include <functional>
#include <unordered_map>

#include "MetaProgramming.h"
#include "EventMessage.h"

namespace Mmp
{

class EventDispatcher
{
public:
    using ptr = std::shared_ptr<EventDispatcher>;
public:
    EventDispatcher() = default;
    ~EventDispatcher() = default;
public:

    template <typename... ArgsType>
    uint64_t EmitEvent(ArgsType&&... args)
    {
        using stl_func = std::function<void(decltype(std::forward<ArgsType>(args))...)>;
        std::unordered_map<void* /* tag */, EventMessage> listeners;
        {
            std::lock_guard<std::mutex> lock(_mtx);
            listeners = _listeners;
        }
        uint64_t count = 0;
        for (auto& listener : listeners)
        {
            listener.second.Get<stl_func>()(std::forward<ArgsType>(args)...);
            count++;
        }
        return count;
    }

    template <typename Func>
    void AddListener(void *tag, Func &&func) 
    {
        using stl_func = typename function_traits<typename std::remove_reference<Func>::type>::stl_function_type;
        EventMessage message;
        message.Set<stl_func>(std::forward<Func>(func));
        std::lock_guard<std::mutex> lock(_mtx);
        _listeners[tag] = message;
    }

    void DelListener(void* tag, bool& isEmpty);
private:
    std::mutex _mtx;
    std::unordered_map<void* /* tag */, EventMessage> _listeners;
};

} // namespace Mmp