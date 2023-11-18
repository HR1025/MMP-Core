//
// Any.h
//
// Library: Common
// Package: Any
// Module:  Any
// 
// Note : This file is modified from Foundation/include/Poco/Any.h (Poco)
// 
// FAQ : (1) 为什么使用 Any 而非裸指针
//           1 - 提供内存管理
//           2 - 运行时提供类型转换检查
//       (2) 为什么使用 Any 而非 Union
//           对于 POD 类型, Union 也是一种不错的解决方案,但是 Union 的内存分配是以联合体中最大
//           来进行计算的,这对于联合体中的其他成员是"不公平"的
//       (3) Any 的设计原理
//           Any 实际上是一个通用容器,其核心与 share_ptr 没有太大差异,借助 template 模板技术来
//           进行实现; 在实例化模板的同时就确认了 Any 容器具体的类型,从而能够做到安全的类型返回
//       (4) 与 Poco::Any 有什么差异
//           对于调用者而言没有差异,运行时效率在某些情况下低于 Poco::Any; 由于去除了 Poco 对于 SSO
//           有关的优化,模板的使用得到了简化,代码逻辑 Mmp::Any 简单于 Poco::Any
//

#pragma once

#include <cassert>

#include "ValueHolder.h"
#include "MetaProgramming.h"

namespace Mmp
{

class Any
{
public:
    Any() {}
    ~Any() {}
public:
    /**
     * @brief 类型泛化
     */
    template<typename ValueType>
    Any(const ValueType& value)
    {
        construct(value);
    }

    /**
     * @brief 拷贝构造
     */
    explicit Any(const Any& other)
    {
        if ((this != &other) && !other.empty())
            construct(other);
        else if ((this != &other) && other.empty())
             _valueHolder.erase();
    }

    /**
     * @brief 类型泛化
     */
    template <typename ValueType>
    Any& operator = (const ValueType& rhs)
    {
        construct(rhs);
        return *this;
    }

    /**
     * @brief 拷贝赋值
     */
    Any& operator = (const Any& rhs)
    {
		if ((this != &rhs) && !rhs.empty())
			construct(rhs);
		else if ((this != &rhs) && rhs.empty())
			_valueHolder.erase();

		return *this;
    }

    /**
     * @brief 是否为空
     */
    bool empty() const
    {
        return _valueHolder.empty();
    }

    /**
     * @sa     ValueHolder
     * @note   ValueHolder 是实际类型实例的持有者
     */
    ValueHolder* content() const
    {
        return _valueHolder.content();
    }

    /**
     * @brief 获取 Any 的具体类型
     */
    const std::type_info& type() const
    {
        return empty() ? typeid(void) : content()->type();
    }

private:
    template<typename ValueType>
    void construct(const ValueType& value)
    {
        _valueHolder.assign<Holder<ValueType>, ValueType>(value);
    }

    void construct(const Any& other)
    {
        if (!other.empty())
            other.content()->clone(&_valueHolder);
        else
            _valueHolder.erase();
    }

private:
    Placeholder<ValueHolder> _valueHolder;

    template<typename ValueType>
    friend ValueType* AnyCast(Any*);

    template<typename ValueType>
    friend ValueType* UnsafeCast(Any*);

    template<typename ValueType>
    friend const ValueType& RefAnyCast(const Any&);

    template<typename ValueType>
    friend ValueType& RefAnyCast(Any&);

    template<typename ValueType>
    friend ValueType AnyCast(Any&);
};

template<typename ValueType>
ValueType* AnyCast(Any* operand)
{
    if (operand == nullptr || operand->type() != typeid(ValueType))
    {
        return nullptr;
    }
    else
    {
        return &static_cast<Holder<ValueType>*>(operand->content())->_held;
    }
}

template<typename ValueType>
const ValueType* AnyCast(const Any* operand)
{
    return AnyCast<ValueType>(const_cast<Any*>(operand)); //const Any* -> Any*
}

template <typename ValueType>
ValueType AnyCast(Any& operand)
{
	typedef typename TypeWrapper<ValueType>::TYPE NonRef;

    // Hint : Holder::ValueType 一定是非引用类型,
    //        所以触发内存拷贝, AnyCast 又名为 NoRefAnyCast
	NonRef* result = AnyCast<NonRef>(&operand);
	if (!result)
	{
        assert(false);
	}
	return *result;
}

template <typename ValueType>
ValueType AnyCast(const Any& operand)
{
	typedef typename TypeWrapper<ValueType>::TYPE NonRef;

	return AnyCast<NonRef&>(const_cast<Any&>(operand));
}

template <typename ValueType>
const ValueType& RefAnyCast(const Any& operand)
{
    // Hint : Holder::ValueType 可能是非引用类型,在允许的情况下不进行内存拷贝
	ValueType* result = AnyCast<ValueType>(const_cast<Any*>(&operand));
	if (!result)
	{
        assert(false);
	}
	return *result;
}

template <typename ValueType>
ValueType& RefAnyCast(Any& operand)
{
	ValueType* result = AnyCast<ValueType>(&operand);
	if (!result)
	{
        assert(false);
	}
	return *result;
}

template <typename ValueType>
ValueType* UnsafeAnyCast(Any* operand)
{
	return &static_cast<Holder<ValueType>*>(operand->content())->_held;
}

template <typename ValueType>
const ValueType* UnsafeAnyCast(const Any* operand)
{
	return AnyCast<ValueType>(const_cast<Any*>(operand));
}

} // namespace Mmp

