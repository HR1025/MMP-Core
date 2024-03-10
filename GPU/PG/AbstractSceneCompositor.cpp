#include "AbstractSceneCompositor.h"

#include <memory>

#include "Scene/SceneCompositorImpl.h"

namespace Mmp
{
namespace Gpu 
{

AbstractSceneCompositor::ptr AbstractSceneCompositor::Create()
{
    return std::make_shared<SceneCompositorImpl>();
}

} // namespace Gpu 
} // namespace Mmp