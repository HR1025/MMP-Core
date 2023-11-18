//
// MetaProgramming.h
//
// Library: Common
// Package: Any
// Module:  MetaProgramming
//
// Note : copy from Foundation/include/Poco/MetaProgramming.h (Poco)
//

#pragma once

#include <tuple>
#include <functional>

namespace Mmp
{

template <typename T>
struct IsReference
	/// Use this struct to determine if a template type is a reference.
{
	enum
	{
		VALUE = 0
	};
};


template <typename T>
struct IsReference<T&>
{
	enum
	{
		VALUE = 1
	};
};


template <typename T>
struct IsReference<const T&>
{
	enum
	{
		VALUE = 1
	};
};


template <typename T>
struct IsConst
	/// Use this struct to determine if a template type is a const type.
{
	enum
	{
		VALUE = 0
	};
};


template <typename T>
struct IsConst<const T&>
{
	enum
	{
		VALUE = 1
	};
};


template <typename T>
struct IsConst<const T>
{
	enum
	{
		VALUE = 1
	};
};


template <typename T, int i>
struct IsConst<const T[i]>
	/// Specialization for const char arrays
{
	enum
	{
		VALUE = 1
	};
};


template <typename T>
struct TypeWrapper
	/// Use the type wrapper if you want to decouple constness and references from template types.
{
	typedef T TYPE;
	typedef const T CONSTTYPE;
	typedef T& REFTYPE;
	typedef const T& CONSTREFTYPE;
};


template <typename T>
struct TypeWrapper<const T>
{
	typedef T TYPE;
	typedef const T CONSTTYPE;
	typedef T& REFTYPE;
	typedef const T& CONSTREFTYPE;
};


template <typename T>
struct TypeWrapper<const T&>
{
	typedef T TYPE;
	typedef const T CONSTTYPE;
	typedef T& REFTYPE;
	typedef const T& CONSTREFTYPE;
};


template <typename T>
struct TypeWrapper<T&>
{
	typedef T TYPE;
	typedef const T CONSTTYPE;
	typedef T& REFTYPE;
	typedef const T& CONSTREFTYPE;
};


template<typename T>
struct function_traits;

//普通函数
template<typename Ret, typename... Args>
struct function_traits<Ret(Args...)>
{
public:
    enum { arity = sizeof...(Args) };
    typedef Ret function_type(Args...);
    typedef Ret return_type;
    using stl_function_type = std::function<function_type>;
    typedef Ret(*pointer)(Args...);

    template<size_t I>
    struct args
    {
        static_assert(I < arity, "index is out of range, index must less than sizeof Args");
        using type = typename std::tuple_element<I, std::tuple<Args...> >::type;
    };
};

//函数指针
template<typename Ret, typename... Args>
struct function_traits<Ret(*)(Args...)> : function_traits<Ret(Args...)>{};

//std::function
template <typename Ret, typename... Args>
struct function_traits<std::function<Ret(Args...)>> : function_traits<Ret(Args...)>{};

//member function
#define FUNCTION_TRAITS(...) \
    template <typename ReturnType, typename ClassType, typename... Args>\
    struct function_traits<ReturnType(ClassType::*)(Args...) __VA_ARGS__> : function_traits<ReturnType(Args...)>{}; \

FUNCTION_TRAITS()
FUNCTION_TRAITS(const)
FUNCTION_TRAITS(volatile)
FUNCTION_TRAITS(const volatile)

//函数对象
template<typename Callable>
struct function_traits : function_traits<decltype(&Callable::operator())>{};

} // namespace Mmp