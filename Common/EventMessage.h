//
// EventMessage.h
//
// Library: Event
// Package: Event
// Module:  Message
// 
// Note : This file is modified from ZLMediaKit/3rdpart/ZLToolKit/src/Util/util.h (ZLMediaKit)
//        Date : 2023-10-05
//        Hash : 48f62810c7f480859623898227af2e0c05eff9b6
// Copyright (c) 2016 The ZLToolKit project authors. All Rights Reserved.
//

#pragma once

#include <memory>
#include <functional>

#include "MetaProgramming.h"

namespace Mmp
{

class EventMessage
{
public:
    using ptr = std::shared_ptr<EventMessage>;
public:
    EventMessage() = default;
    ~EventMessage() = default;
    EventMessage(const EventMessage& that) = default;
    EventMessage(const EventMessage&& that);
    EventMessage& operator=(const EventMessage& that) = default;
    EventMessage& operator=(const EventMessage&& that);
public:
    template<typename Func, typename... ArgsType>
    void Set(ArgsType&&... args)
    {
        _type = &typeid(Func);
        _data.reset(new Func(std::forward<ArgsType>(args)...), [](void *ptr) { delete (Func*)ptr; });
    }

    template<typename T>
    void Set(std::shared_ptr<T> data)
    {
        if (data)
        {
            _type = &typeid(T);
            _data = std::move(data);
        }
        else
        {
            Reset();
        }
    }

    template <typename T>
    T& Get() const
    {
        return *((T*)_data.get());
    }

    void Reset();

private:
    const std::type_info* _type = nullptr;
    std::shared_ptr<void> _data;
};

} // namespace Mmp