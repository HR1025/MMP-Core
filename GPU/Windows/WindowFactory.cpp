#include "WindowFactory.h"

#include <Poco/SingletonHolder.h>

#ifdef USE_GBM
#include "EGL/EGLWindowGBM.h"
#endif /* USE_GBM */

#ifdef USE_EGL
#include "EGL/EGLWindowDefault.h"
#endif /* USE_EGL */

#ifdef USE_X11
#include "X11/X11Window.h"
#endif /* USE_X11 */

#ifdef USE_SDL
#include "SDL/SDLWindow.h"
#endif /* USE_SDL */

#ifdef USE_DXGI
#include "DXGI/DXGIWindow.h"
#endif /* USE_DXGI */

#ifdef USE_VULKAN
#include "Vulkan/VulkanWindow.h"
#endif /*USE_VULKAN */

namespace Mmp
{

namespace
{
    static Poco::SingletonHolder<WindowFactory> sh;
}

WindowFactory::WindowFactory()
{
    RegisterBuiltins();
}

WindowFactory::~WindowFactory()
{

}

void WindowFactory::registerClass(const std::string& className, AbstractWindowInstantiator* instantiator)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _windowFactory.registerClass(className, instantiator);
}

AbstractWindows::ptr WindowFactory::createWindow(const std::string& className)
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _windowFactory.CreateInstance(className);
}

void WindowFactory::RegisterBuiltins()
{
    std::lock_guard<std::mutex> lock(_mutex);
#ifdef USE_DXGI
    _windowFactory.registerClass("DXGIWindow", new Instantiator<DXGIWindow, AbstractWindows>);
#endif /* USE_DXGI */
#ifdef USE_GBM
    _windowFactory.registerClass("EGLWindowGBM", new Instantiator<EGLWindowGBM, AbstractWindows>);
#endif /* USE_GBM */
#ifdef USE_EGL
    _windowFactory.registerClass("EGLWindowDefault", new Instantiator<EGLWindowDefault, AbstractWindows>);
#endif /* USE_EGL */
#ifdef USE_X11
    _windowFactory.registerClass("X11Window", new Instantiator<X11Window, AbstractWindows>);
#endif /* USE_X11 */
#ifdef USE_SDL
    _windowFactory.registerClass("SDLWindow", new Instantiator<SDLWindow, AbstractWindows>);
#endif /* USE_SDL */
#ifdef USE_VULKAN
    _windowFactory.registerClass("VulkanWindow", new Instantiator<VulkanWindow, AbstractWindows>);
#endif /* USE_VULKAN */
}

WindowFactory& WindowFactory::DefaultFactory()
{
    return *sh.get();
}

} // namespace Mmp