#include "X11Window.h"

#include <cassert>

namespace Mmp
{

#define X11_DISPLAY_WDITH   1920
#define X11_DISPLAY_HEIGHT  1080

X11Window::X11Window()
{
    _dispaly = nullptr;
    _fbConfig = nullptr;
    _apiType = APIType::OPENGL;
    _onScreen = false;
    _majorVersion = 0;
    _minorVersion = 0;
    _pbuffer = 0;
    _context = 0;
    _visualInfo = nullptr;
}

bool X11Window::Open()
{
    WIN_LOG_INFO << "Try to Open X11 Window";
    _dispaly = XOpenDisplay(NULL);
    if (!_dispaly)
    {
        WIN_LOG_ERROR << "XOpenDisplay fail";
        assert(false);
        goto END;
    }
    {
        int visualAttribs[1] = {};
        int fbConfigNums = 0;
        _fbConfig = glXChooseFBConfig(_dispaly, DefaultScreen(_dispaly), visualAttribs, &fbConfigNums);
        if (!_fbConfig)
        {
            WIN_LOG_ERROR << "glXChooseFBConfig fail";
            assert(false);
            goto END1;
        }
    }
    if (_onScreen)
    {
        int windowAttribs[] = {
            GLX_PBUFFER_WIDTH, X11_DISPLAY_WDITH,
            GLX_PBUFFER_HEIGHT, X11_DISPLAY_HEIGHT,
            None
        };
        _window = glXCreateWindow(_dispaly, _fbConfig[0], DefaultRootWindow(_dispaly), windowAttribs);
        if (!_window)
        {
            WIN_LOG_ERROR << "glXCreateWindow fail";
            assert(false);
            goto END2;
        }
    }
    else
    {
        int pbufferAttribs[] = {
            GLX_PBUFFER_WIDTH, X11_DISPLAY_WDITH,
            GLX_PBUFFER_HEIGHT, X11_DISPLAY_HEIGHT,
            None
        };
        _pbuffer = glXCreatePbuffer(_dispaly, _fbConfig[0], pbufferAttribs);
        if (!_pbuffer)
        {
            WIN_LOG_ERROR << "glXCreatePbuffer fail";
            assert(false);
            goto END2;
        }
    }
    {
        if (_apiType == APIType::OPENGL)
        {
            int attribs[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
            _visualInfo = glXChooseVisual(_dispaly, DefaultScreen(_dispaly), attribs);
            if (_visualInfo == nullptr) 
            {
                WIN_LOG_ERROR << "glXChooseVisual fail";
                assert(false);
                goto END3;
            }

            _context = glXCreateContext(_dispaly, _visualInfo, NULL, True);
            if (!_context) 
            {
                XFree(_visualInfo);
                WIN_LOG_ERROR << "glXCreateContext fail";
                assert(false);
                goto END3;
            }
        }
        else if (_apiType == APIType::OPENGL_ES3)
        {
            PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddressARB((const GLubyte*)"glXCreateContextAttribsARB");
            if (glXCreateContextAttribsARB == nullptr) 
            {
                WIN_LOG_ERROR << "glXGetProcAddressARB fail, query glXCreateContextAttribsARB";
                assert(false);
                goto END3;
            }
            int contextAttributes[] = {
                GLX_CONTEXT_MAJOR_VERSION_ARB, 3, // OpenGL ES32
                GLX_CONTEXT_MAJOR_VERSION_ARB, 2,
                GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
                None
            };
            _context = glXCreateContextAttribsARB(_dispaly, _fbConfig[0], nullptr, True, contextAttributes);
            if (!_context)
            {
                WIN_LOG_ERROR << "glXCreateContext fail";
                assert(false);
                goto END3;
            }
        }

    }
     WIN_LOG_INFO << "Open X11 Window successfully";
    return true;
END4:
    glXDestroyContext(_dispaly, _context);
    _context = 0;
END3:
    if (_onScreen)
    {
        XFree(_visualInfo);
        glXDestroyWindow(_dispaly, _window);
        _visualInfo = nullptr;
        _window = 0;
    }
    else
    {
        glXDestroyPbuffer(_dispaly, _pbuffer);
        _pbuffer = 0;
    }
END2:
    XFree(_fbConfig);
    _fbConfig = nullptr;
END1:
    XCloseDisplay(_dispaly);
    _dispaly = nullptr;
END:
    return false;
}

bool X11Window::Close()
{
    // FIXME : Close Window 会导致 GLX 底层崩溃
    // Hint : 貌似是异步崩溃, 有什么资源没有同步等待回收导致的 ???

    WIN_LOG_INFO << "Try to Close X11 Window";
    if (_context)
    {
        glXWaitGL();
        glXDestroyContext(_dispaly, _context);
        _context = 0;
    }
    glXWaitX();
    if (_pbuffer && !_onScreen)
    {
        glXDestroyPbuffer(_dispaly, _pbuffer);
        _pbuffer = 0;
    }
    else if (_window && _onScreen)
    {
        glXDestroyWindow(_dispaly, _window);
        XFree(_visualInfo);
        _visualInfo = nullptr;
        _window = 0;
    }
    if (_fbConfig)
    {
        XFree(_fbConfig);
        _fbConfig = nullptr;
    }
    if (_dispaly)
    {
        XCloseDisplay(_dispaly);
        _dispaly = nullptr;
    }
    WIN_LOG_INFO << "Close X11 Window successfully";
    return true;
}

bool X11Window::BindRenderThread(bool bind)
{
    WIN_LOG_INFO << "BindRenderThread, bind is: " << (bind ? "true" :  "false");
    if (bind)
    {
        if (!_onScreen)
        {
            glXMakeContextCurrent(_dispaly, _pbuffer, _pbuffer, _context);
        }
        else
        {
            glXMakeContextCurrent(_dispaly, _window, _window, _context);
        }
    }
    else
    {
        glXMakeContextCurrent(_dispaly, 0, 0, NULL);
    }
    return true;
}

bool X11Window::SetAPIType(APIType type)
{
    _apiType = type;
    return true;
}

bool X11Window::SetRenderMode(bool onScreen)
{
    _onScreen = onScreen;
    return true;
}

WindowInfo X11Window::GetInfo()
{
    WindowInfo info;
    info.apiType = _apiType;
    info.vMajor = _majorVersion;
    info.vMajor = _minorVersion;
    return info;
}

void  X11Window::Swap()
{
    if (_onScreen)
    {
        glXSwapBuffers(_dispaly, _window);
    }
}

} // namespace Mmp