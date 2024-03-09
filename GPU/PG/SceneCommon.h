//
// SceneCommon.h
//
// Library: GPU
// Package: Program
// Module:  Scene
// 

#pragma once

#include <cstdint>
#include <string>
#include <iostream>

#include "Common/CommonGeometry.h"

namespace Mmp
{
namespace Gpu 
{

/**
 * @brief 元素优先级
 */
enum class SceneItemLevel
{
    N_30, N_29, N_28, N_27, N_26, N_25, N_24, N_23, N_22, N_21, N_20, N_19, N_18, N_17, N_16,
    N_15, N_14, N_13, N_12, N_11, N_10, N_9,  N_8,  N_7,  N_6,  N_5,  N_4,  N_3,  N_2,  N_1,
    C_0,
    P_1,  P_2,  P_3,  P_4,  P_5,  P_6,  P_7,  P_8,  P_9,  P_10, P_11, P_12, P_13, P_14, P_15,
    P_16, P_17, P_18, P_19, P_20, P_21, P_22, P_23, P_24, P_25, P_26, P_27, P_28, P_29, P_30
};
const std::string SceneItemLevelToStr(SceneItemLevel level);
extern std::ostream& operator<<(std::ostream& os, SceneItemLevel level);
extern bool operator<(SceneItemLevel left, SceneItemLevel right);
extern bool operator>(SceneItemLevel left, SceneItemLevel right);
extern bool operator<=(SceneItemLevel left, SceneItemLevel right);
extern bool operator>=(SceneItemLevel left, SceneItemLevel right);
extern bool operator==(SceneItemLevel left, SceneItemLevel right);
extern bool operator!=(SceneItemLevel left, SceneItemLevel right);
SceneItemLevel IncreaseSceneItemLevel(const SceneItemLevel& level, uint32_t step);
SceneItemLevel DecreaseSceneItemLevel(const SceneItemLevel& level, uint32_t step);

/**
 * @brief 图层优先级
 */
enum class SceneLyaerLevel
{
    N_60, N_59, N_58, N_57, N_56, N_55, N_54, N_53, N_52, N_51, N_50, N_49, N_48, N_47, N_46,
    N_45, N_44, N_43, N_42, N_41, N_40, N_39, N_38, N_37, N_36, N_35, N_34, N_33, N_32, N_31,
    N_30, N_29, N_28, N_27, N_26, N_25, N_24, N_23, N_22, N_21, N_20, N_19, N_18, N_17, N_16,
    N_15, N_14, N_13, N_12, N_11, N_10, N_9,  N_8,  N_7,  N_6,  N_5,  N_4,  N_3,  N_2,  N_1,
    C_0,
    P_1,  P_2,  P_3,  P_4,  P_5,  P_6,  P_7,  P_8,  P_9,  P_10, P_11, P_12, P_13, P_14, P_15,
    P_16, P_17, P_18, P_19, P_20, P_21, P_22, P_23, P_24, P_25, P_26, P_27, P_28, P_29, P_30,
    P_31, P_32, P_33, P_34, P_35, P_36, P_37, P_38, P_39, P_40, P_41, P_42, P_43, P_44, P_45,
    P_46, P_47, P_48, P_49, P_50, P_51, P_52, P_53, P_54, P_55, P_56, P_57, P_58, P_59, P_60
};
extern bool operator<(SceneLyaerLevel left, SceneLyaerLevel right);
extern bool operator>(SceneLyaerLevel left, SceneLyaerLevel right);
extern bool operator<=(SceneLyaerLevel left, SceneLyaerLevel right);
extern bool operator>=(SceneLyaerLevel left, SceneLyaerLevel right);
extern bool operator==(SceneLyaerLevel left, SceneLyaerLevel right);
extern bool operator!=(SceneLyaerLevel left, SceneLyaerLevel right);
const std::string SceneLyaerLevelToStr(SceneLyaerLevel level);
extern std::ostream& operator<<(std::ostream& os, SceneLyaerLevel level);
SceneLyaerLevel IncreaseSceneLyaerLevel(const SceneLyaerLevel& level, uint32_t step);
SceneLyaerLevel DecreaseSceneLyaerLevel(const SceneLyaerLevel& level, uint32_t step);

/**
 * @brief 场景元素描述
 */
class SceneItemParam
{
public:
    SceneItemParam();
public:
    SceneItemLevel     level;
    uint32_t           rotation;
    bool               rowFilp;
    bool               colFilp;
    NormalizedPoint    location;
    NormalizedRect     area;
    float              transparency;
};

/**
 * @brief 场景图层描述
 */
class SceneLayerParam
{
public:
    SceneLayerParam();                                                                         
public:
    SceneLyaerLevel level;
    bool  show;
    float transparency;
    uint32_t width;
    uint32_t height;
};

/**
 * @brief 场景合成器描述
 * 
 */
class SceneCompositorParam
{
public:
    SceneCompositorParam();
public:
    uint32_t width;
    uint32_t height;
    uint32_t bufSize;
};

} // namespace Gpu 
} // namespace Mmp