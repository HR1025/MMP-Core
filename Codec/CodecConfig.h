//
// CodecConfig.h
//
// Library: Coedec
// Package: Codec
// Module:  Factory
// 

#pragma once

#include <memory>

#include "CodecCommon.h"

namespace Mmp
{
namespace Codec
{

class CodecConfig
{
public:
    using ptr = std::shared_ptr<CodecConfig>;
public:
    static CodecConfig::ptr Instance();
public:
    CodecConfig();
    ~CodecConfig();
public:
    void Init();
    void Uninit();
};

} // namespace Codec
} // namespace Mmp