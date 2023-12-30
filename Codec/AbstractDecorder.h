//
// AbstractDecorder.h
//
// Library: Codec
// Package: Codec
// Module:  Decoder
// 

#pragma once

#include <string>
#include <memory>
#include <functional>

#include "CodecCommon.h"

namespace Mmp
{
namespace Codec
{
class AbstractDecoder
{
public:
    using ptr = std::shared_ptr<AbstractDecoder>;
public:
    virtual ~AbstractDecoder() = default;
public:
    /**
     * @brief 设置解码器参数
     * @param parameter : 根据解码器的类型决定
     * @sa    CodecType
     */
    virtual void SetParameter(Any parameter) = 0;
    /**
     * @brief 获取解码器参数
     * @sa    SetParameter
     */
    virtual Any GetParamter() = 0;
    /**
     * @brief 初始化
     */
    virtual bool Init() { return true; };
    /**
     * @brief 重置
     */
    virtual void Uninit() {};
    /**
     * @brief 开始运行 
     */
    virtual bool Start() { return true; }
    /**
     * @brief 结束运行 
     */
    virtual void Stop() {};
    /**
     * @brief 提交压缩数据
     */
    virtual bool Push(AbstractPack::ptr pack) = 0;
    /**
     * @brief 获取解压数据
     */
    virtual bool Pop(AbstractFrame::ptr& frame) = 0;
    /**
     * @brief 是否可提交
     */
    virtual bool CanPush() = 0;
    /**
     * @brief 是否可获取
     */
    virtual bool CanPop() = 0;
    /**
     * @brief 获取解码器描述信息
     */
    virtual const std::string& Description() = 0;
};
} // namespace Codec
} // namespace Mmp