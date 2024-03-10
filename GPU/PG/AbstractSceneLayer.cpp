#include "AbstractSceneLayer.h"

#include "Scene/SceneLayerImpl.h"

namespace Mmp
{
namespace Gpu 
{

AbstractSceneLayer::ptr AbstractSceneLayer::Create()
{
    return std::make_shared<SceneLayerImpl>();
}

} // namespace Gpu 
} // namespace Mmp