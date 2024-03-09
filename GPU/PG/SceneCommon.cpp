#include "SceneCommon.h"

#include <cassert>
#include <cstdint>
#include <algorithm>

namespace Mmp
{
namespace Gpu 
{

bool operator<(SceneItemLevel left, SceneItemLevel right)
{
    return uint32_t(left) < uint32_t(right);
}

bool operator>(SceneItemLevel left, SceneItemLevel right)
{
    return uint32_t(left) > uint32_t(right);
}

bool operator<=(SceneItemLevel left, SceneItemLevel right)
{
    return uint32_t(left) <= uint32_t(right);
}

bool operator>=(SceneItemLevel left, SceneItemLevel right)
{
    return uint32_t(left) >= uint32_t(right);
}

bool operator==(SceneItemLevel left, SceneItemLevel right)
{
    return uint32_t(left) == uint32_t(right);
}

bool operator!=(SceneItemLevel left, SceneItemLevel right)
{
     return uint32_t(left) != uint32_t(right);
}

bool operator<(SceneLyaerLevel left, SceneLyaerLevel right)
{
    return uint32_t(left) < uint32_t(right);
}

const std::string SceneItemLevelToStr(SceneItemLevel level)
{
    switch (level) 
    {
        case SceneItemLevel::N_30: return "N_30";
        case SceneItemLevel::N_29: return "N_29";
        case SceneItemLevel::N_28: return "N_28";
        case SceneItemLevel::N_27: return "N_27";
        case SceneItemLevel::N_26: return "N_26";
        case SceneItemLevel::N_25: return "N_25";
        case SceneItemLevel::N_24: return "N_24";
        case SceneItemLevel::N_23: return "N_23";
        case SceneItemLevel::N_22: return "N_22";
        case SceneItemLevel::N_21: return "N_21";
        case SceneItemLevel::N_20: return "N_20";
        case SceneItemLevel::N_19: return "N_19";
        case SceneItemLevel::N_18: return "N_18";
        case SceneItemLevel::N_17: return "N_17";
        case SceneItemLevel::N_16: return "N_16";
        case SceneItemLevel::N_15: return "N_15";
        case SceneItemLevel::N_14: return "N_14";
        case SceneItemLevel::N_13: return "N_13";
        case SceneItemLevel::N_12: return "N_12";
        case SceneItemLevel::N_11: return "N_11";
        case SceneItemLevel::N_10: return "N_10";
        case SceneItemLevel::N_9: return "N_9";
        case SceneItemLevel::N_8: return "N_8";
        case SceneItemLevel::N_7: return "N_7";
        case SceneItemLevel::N_6: return "N_6";
        case SceneItemLevel::N_5: return "N_5";
        case SceneItemLevel::N_4: return "N_4";
        case SceneItemLevel::N_3: return "N_3";
        case SceneItemLevel::N_2: return "N_2";
        case SceneItemLevel::N_1: return "N_1";
        case SceneItemLevel::C_0: return "C_0";
        case SceneItemLevel::P_1: return "P_1";
        case SceneItemLevel::P_2: return "P_2";
        case SceneItemLevel::P_3: return "P_3";
        case SceneItemLevel::P_4: return "P_4";
        case SceneItemLevel::P_5: return "P_5";
        case SceneItemLevel::P_6: return "P_6";
        case SceneItemLevel::P_7: return "P_7";
        case SceneItemLevel::P_8: return "P_8";
        case SceneItemLevel::P_9: return "P_9";
        case SceneItemLevel::P_10: return "P_10";
        case SceneItemLevel::P_11: return "P_11";
        case SceneItemLevel::P_12: return "P_12";
        case SceneItemLevel::P_13: return "P_13";
        case SceneItemLevel::P_14: return "P_14";
        case SceneItemLevel::P_15: return "P_15";
        case SceneItemLevel::P_16: return "P_16";
        case SceneItemLevel::P_17: return "P_17";
        case SceneItemLevel::P_18: return "P_18";
        case SceneItemLevel::P_19: return "P_19";
        case SceneItemLevel::P_20: return "P_20";
        case SceneItemLevel::P_21: return "P_21";
        case SceneItemLevel::P_22: return "P_22";
        case SceneItemLevel::P_23: return "P_23";
        case SceneItemLevel::P_24: return "P_24";
        case SceneItemLevel::P_25: return "P_25";
        case SceneItemLevel::P_26: return "P_26";
        case SceneItemLevel::P_27: return "P_27";
        case SceneItemLevel::P_28: return "P_28";
        case SceneItemLevel::P_29: return "P_29";
        case SceneItemLevel::P_30: return "P_30";
        default:
            assert(false);
            return "(N/A)";
    }
}

std::ostream& operator<<(std::ostream& os, SceneItemLevel level)
{
    os << SceneItemLevelToStr(level);
    return os;
}

SceneItemLevel IncreaseSceneItemLevel(const SceneItemLevel& level, uint32_t step)
{
    return (SceneItemLevel)std::min((uint32_t)level + step, (uint32_t)SceneItemLevel::P_30);
}

SceneItemLevel DecreaseSceneItemLevel(const SceneItemLevel& level, uint32_t step)
{
    if ((int32_t)level - (int32_t)step >= 0)
    {
        return SceneItemLevel((uint32_t)level - step);
    }
    else
    {
        return SceneItemLevel::N_30;
    }
}

bool operator>(SceneLyaerLevel left, SceneLyaerLevel right)
{
    return uint32_t(left) > uint32_t(right);
}

bool operator<=(SceneLyaerLevel left, SceneLyaerLevel right)
{
    return uint32_t(left) <= uint32_t(right);
}

bool operator>=(SceneLyaerLevel left, SceneLyaerLevel right)
{
    return uint32_t(left) >= uint32_t(right);
}

bool operator==(SceneLyaerLevel left, SceneLyaerLevel right)
{
    return uint32_t(left) == uint32_t(right);
}

bool operator!=(SceneLyaerLevel left, SceneLyaerLevel right)
{
     return uint32_t(left) != uint32_t(right);
}

SceneLyaerLevel IncreaseSceneLyaerLevel(const SceneLyaerLevel& level, uint32_t step)
{
    return (SceneLyaerLevel)std::min((uint32_t)level + step, (uint32_t)SceneLyaerLevel::P_60);
}

SceneLyaerLevel DecreaseSceneLyaerLevel(const SceneLyaerLevel& level, uint32_t step)
{
    if ((int32_t)level - (int32_t)step >= 0)
    {
        return SceneLyaerLevel((uint32_t)level - step);
    }
    else
    {
        return SceneLyaerLevel::N_60;
    }
}

const std::string SceneLyaerLevelToStr(SceneLyaerLevel level)
{
    switch (level) 
    {
        case SceneLyaerLevel::N_30: return "N_30";
        case SceneLyaerLevel::N_29: return "N_29";
        case SceneLyaerLevel::N_28: return "N_28";
        case SceneLyaerLevel::N_27: return "N_27";
        case SceneLyaerLevel::N_26: return "N_26";
        case SceneLyaerLevel::N_25: return "N_25";
        case SceneLyaerLevel::N_24: return "N_24";
        case SceneLyaerLevel::N_23: return "N_23";
        case SceneLyaerLevel::N_22: return "N_22";
        case SceneLyaerLevel::N_21: return "N_21";
        case SceneLyaerLevel::N_20: return "N_20";
        case SceneLyaerLevel::N_19: return "N_19";
        case SceneLyaerLevel::N_18: return "N_18";
        case SceneLyaerLevel::N_17: return "N_17";
        case SceneLyaerLevel::N_16: return "N_16";
        case SceneLyaerLevel::N_15: return "N_15";
        case SceneLyaerLevel::N_14: return "N_14";
        case SceneLyaerLevel::N_13: return "N_13";
        case SceneLyaerLevel::N_12: return "N_12";
        case SceneLyaerLevel::N_11: return "N_11";
        case SceneLyaerLevel::N_10: return "N_10";
        case SceneLyaerLevel::N_9: return "N_9";
        case SceneLyaerLevel::N_8: return "N_8";
        case SceneLyaerLevel::N_7: return "N_7";
        case SceneLyaerLevel::N_6: return "N_6";
        case SceneLyaerLevel::N_5: return "N_5";
        case SceneLyaerLevel::N_4: return "N_4";
        case SceneLyaerLevel::N_3: return "N_3";
        case SceneLyaerLevel::N_2: return "N_2";
        case SceneLyaerLevel::N_1: return "N_1";
        case SceneLyaerLevel::C_0: return "C_0";
        case SceneLyaerLevel::P_1: return "P_1";
        case SceneLyaerLevel::P_2: return "P_2";
        case SceneLyaerLevel::P_3: return "P_3";
        case SceneLyaerLevel::P_4: return "P_4";
        case SceneLyaerLevel::P_5: return "P_5";
        case SceneLyaerLevel::P_6: return "P_6";
        case SceneLyaerLevel::P_7: return "P_7";
        case SceneLyaerLevel::P_8: return "P_8";
        case SceneLyaerLevel::P_9: return "P_9";
        case SceneLyaerLevel::P_10: return "P_10";
        case SceneLyaerLevel::P_11: return "P_11";
        case SceneLyaerLevel::P_12: return "P_12";
        case SceneLyaerLevel::P_13: return "P_13";
        case SceneLyaerLevel::P_14: return "P_14";
        case SceneLyaerLevel::P_15: return "P_15";
        case SceneLyaerLevel::P_16: return "P_16";
        case SceneLyaerLevel::P_17: return "P_17";
        case SceneLyaerLevel::P_18: return "P_18";
        case SceneLyaerLevel::P_19: return "P_19";
        case SceneLyaerLevel::P_20: return "P_20";
        case SceneLyaerLevel::P_21: return "P_21";
        case SceneLyaerLevel::P_22: return "P_22";
        case SceneLyaerLevel::P_23: return "P_23";
        case SceneLyaerLevel::P_24: return "P_24";
        case SceneLyaerLevel::P_25: return "P_25";
        case SceneLyaerLevel::P_26: return "P_26";
        case SceneLyaerLevel::P_27: return "P_27";
        case SceneLyaerLevel::P_28: return "P_28";
        case SceneLyaerLevel::P_29: return "P_29";
        case SceneLyaerLevel::P_30: return "P_30";
        case SceneLyaerLevel::P_31: return "P_31";
        case SceneLyaerLevel::P_32: return "P_32";
        case SceneLyaerLevel::P_33: return "P_33";
        case SceneLyaerLevel::P_34: return "P_34";
        case SceneLyaerLevel::P_35: return "P_35";
        case SceneLyaerLevel::P_36: return "P_36";
        case SceneLyaerLevel::P_37: return "P_37";
        case SceneLyaerLevel::P_38: return "P_38";
        case SceneLyaerLevel::P_39: return "P_39";
        case SceneLyaerLevel::P_40: return "P_40";
        case SceneLyaerLevel::P_41: return "P_41";
        case SceneLyaerLevel::P_42: return "P_42";
        case SceneLyaerLevel::P_43: return "P_43";
        case SceneLyaerLevel::P_44: return "P_44";
        case SceneLyaerLevel::P_45: return "P_45";
        case SceneLyaerLevel::P_46: return "P_46";
        case SceneLyaerLevel::P_47: return "P_47";
        case SceneLyaerLevel::P_48: return "P_48";
        case SceneLyaerLevel::P_49: return "P_49";
        case SceneLyaerLevel::P_50: return "P_50";
        case SceneLyaerLevel::P_51: return "P_51";
        case SceneLyaerLevel::P_52: return "P_52";
        case SceneLyaerLevel::P_53: return "P_53";
        case SceneLyaerLevel::P_54: return "P_54";
        case SceneLyaerLevel::P_55: return "P_55";
        case SceneLyaerLevel::P_56: return "P_56";
        case SceneLyaerLevel::P_57: return "P_57";
        case SceneLyaerLevel::P_58: return "P_58";
        case SceneLyaerLevel::P_59: return "P_59";
        case SceneLyaerLevel::P_60: return "P_60";
        default:
            assert(false);
            return "(N/A)";
    }
}

extern std::ostream& operator<<(std::ostream& os, SceneLyaerLevel level)
{
    os << SceneLyaerLevelToStr(level);
    return os;
}

SceneItemParam::SceneItemParam()
{
    level = SceneItemLevel::C_0;
    rotation = 0;
    rowFilp = false;
    colFilp = false;
    transparency = 1.0f;
}

SceneLayerParam::SceneLayerParam()
{
    level = SceneLyaerLevel::C_0;
    show = true;
    transparency = 1.0f;
    width = 0;
    height = 0;
}

SceneCompositorParam::SceneCompositorParam()
{
    width = 1920;
    height = 1080;
    bufSize = 3;
}

} // namespace Gpu 
} // namespace Mmp