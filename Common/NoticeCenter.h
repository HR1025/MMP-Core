//
// NoticeCenter.h
//
// Library: Event
// Package: Event
// Module:  Center
// 
// Note : This file is modified from ZLMediaKit/3rdpart/ZLToolKit/src/Util/NoticeCenter.h (ZLMediaKit)
//        Date : 2023-10-05
//        Hash : 48f62810c7f480859623898227af2e0c05eff9b6
// Copyright (c) 2016 The ZLToolKit project authors. All Rights Reserved.
//

#pragma once

#include <mutex>
#include <memory>
#include <string>

#include "EventDispatcher.h"

namespace Mmp
{

class NoticeCenter
{
public:
    using ptr = std::shared_ptr<NoticeCenter>;
public:
    NoticeCenter() = default;
    ~NoticeCenter() = default;
public:
    static NoticeCenter::ptr Instance();
public:
    template<typename... ArgsType>
    uint64_t EmitEvent(const std::string& event, ArgsType&&... args)
    {
        EventDispatcher::ptr dispatcher = GetDispatcher(event);
        if (dispatcher)
        {
            return dispatcher->EmitEvent(std::forward<ArgsType>(args)...);
        }
        return 0;
    }

    template<typename Func>
    void AddListener(const std::string& event, void* tag, Func&& func)
    {
        EventDispatcher::ptr dispatcher = GetDispatcher(event, true);
        dispatcher->AddListener(tag, std::forward<Func>(func));
    }

    void DelListener(const std::string& event, void* tag);
private:
    EventDispatcher::ptr GetDispatcher(const std::string& event, bool create = false);
private:
    std::mutex _mtx;
    std::unordered_map<std::string /* event */, EventDispatcher::ptr> _dispatchers;
};

} // namespace Mmp