//
// AbstractDecorder.h
//
// Library: Coedec
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
    using OnStatusChange = std::function<void(DecoderStatus status, const std::string& info)>;
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
     * @brief 提交压缩数据
     */
    virtual bool Push(Pack::ptr pack) = 0;
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
     * @brief 设置事件监听
     */
    virtual void SetListener(OnStatusChange onStatusChange) = 0;
    /**
     * @brief 取消事件监听
     */
    virtual void DelListener() = 0;
    /**
     * @brief 获取解码器描述信息
     */
    virtual const std::string& Description() = 0;
};
} // namespace Codec
} // namespace Mmp