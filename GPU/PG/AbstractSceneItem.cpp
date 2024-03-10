#include "AbstractSceneItem.h"

#include "Scene/SceneItemImpl.h"


namespace Mmp 
{
namespace Gpu 
{

AbstractSceneItem::ptr AbstractSceneItem::Create()
{
    return std::make_shared<SceneItemImpl>();
}

} // namespace Gpu 
} // namespace Mmp