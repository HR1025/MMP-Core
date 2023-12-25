//
// AbstractSLFactory.h
//
// Library: SL
// Package: SL
// Module:  SL
// 

#include <string>
#include "Common/NormalSharedData.h"
#include "SLCommon.h"

namespace Mmp
{
namespace SL
{

/**
 * @note 基于 SPIRV-Cross
 * @sa   https://github.com/KhronosGroup/SPIRV-Cross
 */
class AbstractSLConverter
{
public:
    /**
     * @brief       将 spriv 转化为对应 SL 语言的代码
     * @param[in]   stage
     * @param[in]   spriv : SPRI-V 标准的二进制文件
     */
    virtual std::string Convert(const ShaderStage& stage, AbstractSharedData::ptr spriv) = 0;
};

}
} // namespace Mmp