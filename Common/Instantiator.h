//
// Instantiator.h
//
// Library: Common
// Package: Factory
// Module:  Instantiator
// 

#pragma once
#include <memory>

namespace Mmp
{

/**
 * @brief 抽象实例创建器
 */
template<class Base>
class AbstractInstantiator
{
public:
    AbstractInstantiator() = default;
    virtual ~AbstractInstantiator() = default;
    virtual std::shared_ptr<Base> CreateInstance() const = 0;
};

/**
 * @brief 抽象实例创建辅助器
 */
template<class Derive, class Base>
class Instantiator : public AbstractInstantiator<Base>
{
public:
    Instantiator() = default;
    ~Instantiator() = default;
    std::shared_ptr<Base> CreateInstance() const override
    {
        return std::make_shared<Derive>();
    }
};

}

