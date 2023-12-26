#include "SDLWindow.h"

#include <cassert>
#include <vector>
#include <algorithm>

#include <SDL2/SDL.h>

namespace Mmp
{

SDLWindow::SDLWindow()
{
    _mode      = SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN;
    _window    = nullptr;
    _contex    = nullptr;
    _major     = 0;
    _minor     = 0;
    _type      = APIType::OPENGL;

    _isInited = false;
    _displayWidth = 0;
    _displayHeight = 0;
    _displayRefresh = 0;
}

bool SDLWindow::SetAPIType(APIType type)
{
    assert(type == APIType::OPENGL || type == APIType::OPENGL_ES3);
    _type = type;
    return true;
}

bool SDLWindow::SetRenderMode(bool onScreen)
{
    // See aslo : https://github.com/libsdl-org/SDL/issues/7290
    WIN_LOG_INFO << "Set Render Mode : " << (onScreen ? "on-screen" : "off-screen");
    if (onScreen)
    {
        _mode = _mode & (~SDL_WINDOW_HIDDEN);
    }
    else
    {
        _mode = _mode | SDL_WINDOW_HIDDEN;
    }
    return true;
}

bool SDLWindow::Open()
{
    using Ver = std::pair<uint32_t, uint32_t>;
    static std::vector<Ver> attemptVers = 
    {
        // OpenGL
        {4, 6}, {4, 5}, {4, 4}, {4, 3}, {4, 2}, {4, 1}, {4, 0},
        // OpenGL ES3
        {3, 3}, {3, 2}, {3, 1}, {3, 0},
    };

    if (!InitSDL())
    {
        return false;
    }

    WIN_LOG_INFO << "Try to Open SDL Window";

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    for (const auto& ver : attemptVers)
    {
        if (_type == APIType::OPENGL && ver.first != 4) continue;
        else if (_type == APIType::OPENGL_ES3 && ver.first != 3) continue;

        if (!(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, ver.first) == 0 &&
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, ver.second) == 0 && 
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE) == 0)
        )
        {
            continue;
        }
        // Hint : 尺寸暂时固定为1080P
        _window = SDL_CreateWindow("MMP", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, std::min(_displayWidth, (uint32_t)1920), std::min(_displayHeight, (uint32_t)1080), _mode);
        if (!_window)
        {
            continue;
        }
        // Hint : 通过创建 context 来确认 GL version 是否合适
        _contex = SDL_GL_CreateContext(_window);
        if (!_contex)
        {
            SDL_DestroyWindow(_window);
            _window = nullptr;
            WIN_LOG_WARN << "Create window fail, api type is: " << _type
                << ", version is: (" << ver.first << "," << ver.second << "), try again...";
            continue;
        }
        // if (_window && _contex)
        {
            SDL_GL_DeleteContext(_contex);
            _contex = nullptr;
            _major = ver.first;
            _minor = ver.second;
            WIN_LOG_INFO << "Create window successfully, api type is: " << _type
                         << ", version is: (" << _major << "," << _minor << ")";
            break;
        }
    }
    if (_window)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool SDLWindow::Close()
{
    WIN_LOG_INFO << "Try to close SDL Window";
    if (_window)
    {
        SDL_DestroyWindow(_window);
        _window = nullptr;
    }
    UnInitSDL();
    return true;
}

bool SDLWindow::BindRenderThread(bool bind)
{
    WIN_LOG_INFO << "BindRenderThread : " << (bind ? "bind" : "unbind");
    if (bind && _contex==nullptr)
    {
        _contex = SDL_GL_CreateContext(_window);
        if (_contex)
        {
            WIN_LOG_INFO << "Create GL Contex";
        }
        else
        {
            WIN_LOG_ERROR << "Create GL Context fail, error is: " << SDL_GetError();
            assert(false);
        }
        return _contex ? true : false;
    }
    else if (!bind && _contex)
    {
        WIN_LOG_INFO << "Delete GL Contex";
        // WORKARUOND : ??? 好像 onScreen render 不能提前删除 gl contex, 会导致 SQL_QUIT 崩溃
        // TODO : 确认一下 SDL 的相关逻辑
        if (!(SDL_WINDOW_HIDDEN & _mode))
        {
            SDL_GL_DeleteContext(_contex);
        }
        return true;
    }
    else
    {
        // Hint : nothing to do
        return true;
    }
}

WindowInfo SDLWindow::GetInfo()
{
    WindowInfo info;
    info.apiType = _type;
    info.vMajor  = _major;
    info.vMinor  = _minor;
    return info;
}

void SDLWindow::Swap()
{
    assert(_window);
    if (!(_mode & SDL_WINDOW_HIDDEN))
    {
        // Hint : immediately update (don't care vsync)
        SDL_GL_SetSwapInterval(0);
        SDL_GL_SwapWindow(_window);
    }
}

bool SDLWindow::InitSDL()
{
    /**
     * @brief 检查 SDL 的编译时头文件和运行时库是否匹配
     */
    auto checkSDLVersionIsSame = []() -> bool
    {
        SDL_version compiled;
        SDL_version linked;
        SDL_VERSION(&compiled);
        SDL_GetVersion(&linked);
        WIN_LOG_INFO << "SDL version when complie is: (" << (int)compiled.major << "." << (int)compiled.minor << "." << (int)compiled.patch << ")";
        WIN_LOG_INFO << "SDL version when link is: (" << (int)linked.major << "." << (int)linked.minor << "." << (int)linked.patch << ")";
        return (compiled.major == linked.major && compiled.minor == linked.minor && compiled.patch == linked.patch);
    };
    
    /**
     * @brief 获取主屏幕相关信息
     */
    auto getMajorDisplayInfo = [this]() -> bool
    {
        SDL_DisplayMode displayMode;
        if (SDL_GetCurrentDisplayMode(0, &displayMode) != 0)
        {
            return false;
        }
        _displayWidth   = displayMode.w;
        _displayHeight  = displayMode.h;
        _displayRefresh = displayMode.refresh_rate;
        WIN_LOG_INFO << "Major Display info : (" << _displayWidth << "," << _displayHeight << ")x" << _displayRefresh;
        return true;
    };

    if (_isInited)
    {
        return true;
    }

    WIN_LOG_INFO << "SDL Display Init";

    if (!checkSDLVersionIsSame())
    {
        WIN_LOG_ERROR << "SDL version is not same when runing as when compiling";
        return false;
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        WIN_LOG_ERROR << "Failed to initialize SDL, error is: " << SDL_GetError();
        return false;
    }
    else
    {
        WIN_LOG_INFO << "Initialize SDL with SDL_INIT_VIDEO flag successfully";
    }
    
    if (!getMajorDisplayInfo())
    {
        return false;
    }
    _isInited = true;
    return true;
}

void SDLWindow::UnInitSDL()
{
    WIN_LOG_INFO << "SDL Display Uninit";
    SDL_Quit();
    _isInited = false;
}

} // namespace Mmp
