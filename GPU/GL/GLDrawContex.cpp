#include "GLDrawContex.h"

#include <cassert>
#include <mutex>
#include <atomic>

#ifdef USE_OPENGL
#include "OpenGL/OpenGLDrawContex.h"
#endif /* USE_OPENGL */
#ifdef USE_D3D11
#include "D3D11/D3D11DrawContex.h"
#endif /* USE_D3D11 */

namespace Mmp
{

static GPUBackend gBackend = GPUBackend::OPENGL;

GLDrawContex::GLDrawContex()
{
    _windows = nullptr;
}

Any GLDrawContex::Get(const std::string& /* key */)
{
    return Any();
}

void GLDrawContex::Set(const std::string& /* key */, Any /* value */)
{

}

void GLDrawContex::SetGPUBackendType(GPUBackend backend)
{
    gBackend = backend;
}

GPUBackend GLDrawContex::GetGpuBackendType()
{
    return gBackend;
}

GLDrawContex::ptr GLDrawContex::Instance()
{

    static std::shared_ptr<GLDrawContex> sh;
    static std::mutex mtx;
    if (!sh)
    {
        std::lock_guard<std::mutex> lock(mtx);
        switch (gBackend)
        {
#ifdef USE_OPENGL
            case GPUBackend::OPENGL: /* pass through */
            case GPUBackend::OPENGL_ES:
            {

                // Hint : OpenGL is C/S model, OpenGL client call should wait contex(OpenGL Server) ready 
                sh = std::make_shared<OpenGLDrawContex>();
                break;
            }
#endif
#ifdef USE_D3D11
            case GPUBackend::D3D11:
            {
                sh = std::make_shared<D3D11DrawContex>();
                break;
            }
#endif /* USE_D3D11 */
            default:
            {
                // TODO : empty  implementation
                assert(false);
                break;
            }
        }
    }
    return sh;
}

AbstractWindows::ptr GLDrawContex::GetWindows()
{
    return _windows;
}

void GLDrawContex::SetWindows(AbstractWindows::ptr windows)
{
    _windows = windows;
}

} // namespace Mmp

