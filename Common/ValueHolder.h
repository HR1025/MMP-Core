//
// ValueHolder.h
//
// Library: Common
// Package: Any
// Module:  ValueHolder
// 
// Note : This file is modified from Foundation/include/Poco/Any.h (Poco)
// 

#pragma once

#include <typeinfo>

#include "Placeholder.h"

namespace Mmp
{

class ValueHolder
{
public:
    virtual ~ValueHolder() = default;
    virtual const std::type_info& type() const = 0;
    virtual void clone(Placeholder<ValueHolder>* pPlaceholder) const = 0;
};

template<typename ValueType>
class Holder : public ValueHolder
{
public:
    explicit Holder(const ValueType& value) : _held(value) {}
    const std::type_info& type() const override
    {
        return typeid(ValueType);
    }
    void clone(Placeholder<ValueHolder>* pPlaceholder) const override
    {
        pPlaceholder->assign<Holder<ValueType>, ValueType>(_held);
    }
    ValueType _held;
private:
    Holder& operator=(const Holder&);
};

} // namespace Mmp