//
// WindowUtils.h
//
// Library: GPU
// Package: Windows
// Module:  Utils
// 

#pragma once

#include "Common/Common.h"

#if MMP_PLATFORM(WINDOWS)

#include <winuser.h>
#include <winnt.h>

// TODO : set it according to some other way
#define DXGI_SCREEN_WIDTH 1920
#define DXGI_SCREEN_HEIGHT 1080

namespace Mmp
{

HINSTANCE Win32GetInstance();

/**
 * @brief 创建 WIN32 窗口
 * @sa    windows-win32-winmsg.pdf - CreateWindowExA function
 */
HWND Win32CreateWindow(const std::string& name, DWORD dwStyle, uint32_t width, uint32_t height, HINSTANCE hInstance);

/**
 * @brief   显示 WIN32 窗口
 * @sa      windows-win32-winmsg.pdf - ShowWindow function
 */
bool Win32ShowWindow(HWND hWnd, int nCmdShow);

} // namespace Mmp

#endif