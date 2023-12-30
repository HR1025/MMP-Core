//
// CodecFactory.h
//
// Library: Codec
// Package: Codec
// Module:  Factory
// 

#pragma once

#include <mutex>

#include "Common/DynamicFactory.h"
#include "Common/Instantiator.h"

#include "CodecCommon.h"
#include "AbstractEncoder.h"
#include "AbstractDecorder.h"

namespace Mmp
{
namespace Codec
{

class EncoderFactory
{
public:
    using EncoderInstantiator = AbstractInstantiator<AbstractEncoder>;
public:
    EncoderFactory();
    ~EncoderFactory();
public:
    void RegisterEncoderClass(const std::string& className, EncoderInstantiator* instantiator);
    AbstractEncoder::ptr CreateEncoder(const std::string& className);
    static EncoderFactory& DefaultFactory();
private:
    void RegisterBuiltins();
private:
    std::mutex _mutex;
    DynamicFactory<AbstractEncoder> _encoderFactory;
};

class DecoderFactory
{
public:
    using DecoderInstantiator = AbstractInstantiator<AbstractDecoder>;
public:
    DecoderFactory();
    ~DecoderFactory();
public:
    void RegisterDecoderClass(const std::string& className, DecoderInstantiator* instantiator);
    AbstractDecoder::ptr CreateDecoder(const std::string& className);
    static DecoderFactory& DefaultFactory();
private:
    void RegisterBuiltins();
private:
    std::mutex _mutex;
    DynamicFactory<AbstractDecoder> _decoderFactory;
};

} // namespace Codec
} // namespace Mmp