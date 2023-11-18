#include "WindowUtilsWin32.h"

#if MMP_PLATFORM(WINDOWS)

#include <memory.h>

namespace Mmp
{

/**
 * @todo 完善处理流程
 */
static LRESULT CALLBACK DisplayProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return DefWindowProc(hWnd, message, wParam, lParam);
}

HINSTANCE Win32GetInstance()
{
    return GetModuleHandle(NULL);
}

HWND Win32CreateWindow(const std::string& name, DWORD dwStyle, uint32_t width, uint32_t height, HINSTANCE hInstance)
{
    WNDCLASSEX wsi = {};
    memset(&wsi, 0, sizeof(WNDCLASSEX));
    wsi.cbSize = sizeof(WNDCLASSEX);
    wsi.style = CS_HREDRAW | CS_VREDRAW;
    wsi.lpfnWndProc = (WNDPROC)DisplayProc;
    wsi.hInstance = hInstance;
    wsi.hCursor = LoadCursor(NULL, IDC_ARROW);
    wsi.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wsi.lpszMenuName = 0;
    wsi.lpszClassName = LPCWSTR(name.c_str());
    wsi.hIcon = 0;
    wsi.hIconSm = 0;
    RegisterClassEx(&wsi);
    return CreateWindowEx(0, LPCWSTR(name.c_str()), L"",  WS_OVERLAPPEDWINDOW, 0, 0, width, height, nullptr, nullptr, hInstance, nullptr);
}

bool Win32ShowWindow(HWND hWnd, int nCmdShow)
{
    return ShowWindow(hWnd, SW_SHOW);
}

} // namespace Mmp

#endif

