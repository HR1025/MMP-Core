//
// Placeholder.h
//
// Library: Common
// Package: Any
// Module:  Placeholder
// 
// Note : This file is modified from Foundation/include/Poco/Any.h (Poco),
//        a simple version but without SSO (Small Object Optimization),
//        SSO can avoid frequently memory allocation in some situation,
//        see Poco::Placeholder for more information
// 

#pragma once

namespace Mmp
{

/**
 * @note  must be C++ POD type (for "sizeof")
 */
template<typename PlaceholderT /* V */>
struct Placeholder
{
public:
    Placeholder(const Placeholder&) = delete;
    Placeholder(const Placeholder&&) = delete;
    Placeholder& operator=(const Placeholder&) = delete;
    Placeholder& operator=(Placeholder &&) = delete;
public:
    Placeholder() : pHolder(nullptr) {}

    ~Placeholder()
    {
        erase();
    }
    
    void erase()
    {
        if (pHolder) delete pHolder;
        pHolder = nullptr;
    }

    template<typename T, typename V>
    PlaceholderT* assign(const V& value)
    {
        erase();
        pHolder = new T(value);
        return pHolder;
    }
    

    PlaceholderT* content() const
    {
        return pHolder;
    }

    bool empty() const
    {
        return pHolder == nullptr;
    }

private:
    PlaceholderT* pHolder;
};

} // nameapce Mmp