#pragma once

#include <chrono>

#include "Common/AbstractFrame.h"
#include "Common/SharedDataDecorator.h"
#include "Common/NormalFrame.h"
#include "Common/PixelsInfo.h"

namespace Mmp
{
namespace Codec
{

class StreamFrame : public SharedDataDecoratorTemplate<AbstractFrame>
{
public:
    using ptr = std::shared_ptr<StreamFrame>;
public:
    explicit StreamFrame(const PixelsInfo& info, AbstractAllocateMethod::ptr allocateMethod = nullptr);
    explicit StreamFrame(const PixelsInfo& info, AbstractSharedData::ptr data);
public:
    PixelsInfo                 info;
    std::chrono::milliseconds  pts;
    std::chrono::milliseconds  dts;
};

} // namespace Codec
} // namespace Mmp