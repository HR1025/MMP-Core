cmake_minimum_required(VERSION 3.8)

option(ENABLE_D3D11_DEBUG "D3D11 Debug Mode" ON)

set(Windows_SRCS)
set(Windows_INCS)
set(Windows_LIB_DIR)
set(Windows_LIBS)

list(APPEND Windows_INCS
    ${MMP_CORE_PRO_TOP_DIR}
    ${MMP_CORE_PRO_TOP_DIR}/GPU/GL
    ${CMAKE_CURRENT_SOURCE_DIR}
)

list(APPEND Windows_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/WindowsCommon.h
    ${CMAKE_CURRENT_SOURCE_DIR}/WindowsCommon.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractWindows.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractWindows.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/WindowFactory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/WindowFactory.cpp
)

# Windows Utils
list(APPEND Windows_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/WindowUtils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/WindowUtils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils/WindowUtilsWin32.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils/WindowUtilsWin32.cpp
)

# EGL Windows
if (USE_EGL OR USE_X11)
    # see also : https://cmake.org/cmake/help/latest/module/FindOpenGL.html
    if (NOT OPENGL_FOUND) # 交叉编译时需外部设置
        if (CMAKE_HOST_UNIX)
            set(OpenGL_GL_PREFERENCE GLVND)
        endif()
        find_package(OpenGL REQUIRED)
    endif()
    message("-- Found EGL")
    message("  -- EGL INCS : ${OPENGL_INCLUDE_DIR}")
    message("  -- EGL LIB DIR : ${OpenGL_LIBRARY_DIRS}")
    message("  -- EGL LIBS : ${OPENGL_LIBRARIES}")
    list(APPEND Windows_INCS ${OPENGL_INCLUDE_DIR})
    list(APPEND Windows_LIB_DIR ${OpenGL_LIBRARY_DIRS})
    list(APPEND Windows_LIBS ${OPENGL_LIBRARIES})
    if (USE_EGL)
        list(APPEND Windows_LIBS Mmp::GLEW)
    endif()
endif()

if (USE_X11)
    find_package(X11 REQUIRED)
    message("-- Found X11")
    message("  -- X11 INCS : ${X11_INCLUDE_DIR}")
    message("  -- X11 LIB DIR : ${X11_LIBRARY_DIRS}")
    message("  -- X11 LIBS : ${X11_LIBRARIES}")
    list(APPEND Windows_INCS ${X11_INCLUDE_DIR})
    list(APPEND Windows_LIB_DIR ${X11_LIBRARY_DIRS})
    list(APPEND Windows_LIBS ${X11_LIBRARIES})
endif()

if (USE_EGL)
    if (NOT MSVC AND NOT NOT_AUTO_EGL)
        # Workaround : make sure to find EGL
        list(APPEND Windows_LIBS EGL)
    endif()
    list(APPEND Windows_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/EGL/AbstractEGLWindow.h
        ${CMAKE_CURRENT_SOURCE_DIR}/EGL/AbstractEGLWindow.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/EGL/EGLWindow.h
        ${CMAKE_CURRENT_SOURCE_DIR}/EGL/EGLWindow.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/EGL/EGLWindowDefault.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/EGL/EGLCommon.h
        ${CMAKE_CURRENT_SOURCE_DIR}/EGL/EGLCommon.cpp
    )
    if (USE_GBM)
        ## drm
        if (NOT DRM_FOUND)
            find_package(PkgConfig REQUIRED)
            pkg_check_modules(DRM REQUIRED libdrm)
        endif()
        message("-- Found DRM")
        message("  -- DRM INCS : ${DRM_INCLUDE_DIRS}")
        message("  -- DRM LIB DIR : ${DRM_LIBRARY_DIRS}")
        message("  -- DRM LIBS : ${DRM_LIBRARIES}")
        list(APPEND Windows_INCS ${DRM_INCLUDE_DIRS})
        list(APPEND Windows_LIB_DIR ${DRM_LIBRARY_DIRS})
        list(APPEND Windows_LIBS ${DRM_LIBRARIES})
        ## gbm : sudo apt install libgbm-dev
        if (NOT GBM_FOUND)
            find_package(PkgConfig REQUIRED)
            pkg_check_modules(GBM REQUIRED gbm)
        endif()
        message("-- Found GBM")
        message("  -- GBM INCS : ${GBM_INCLUDE_DIRS}")
        message("  -- GBM LIB DIR : ${GBM_LIBRARY_DIRS}")
        message("  -- GBM LIBS : ${GBM_LIBRARIES}")
        list(APPEND Windows_INCS ${GBM_INCLUDE_DIRS})
        list(APPEND Windows_LIB_DIR ${GBM_LIBRARY_DIRS})
        list(APPEND Windows_LIBS ${GBM_LIBRARIES})
        ## implemention
        list(APPEND Windows_SRCS
            ${CMAKE_CURRENT_SOURCE_DIR}/EGL/EGLWindowGBM.h
            ${CMAKE_CURRENT_SOURCE_DIR}/EGL/EGLWindowGBM.cpp
        )
    endif()
else()
    set(USE_GBM OFF)
endif()

# X11 window
if (USE_X11)
    list(APPEND Windows_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/X11/X11Window.h
        ${CMAKE_CURRENT_SOURCE_DIR}/X11/X11Window.cpp
    )
endif()

# SDL Windows
if (USE_SDL)
    if (NOT SDL2_FOUND)
        find_package(SDL2 CONFIG REQUIRED)
    endif()
    message("-- Found SDL2")
    message("  -- SDL INCS : ${SDL2_INCLUDE_DIRS}")
    message("  -- SDL LIB DIR : ${SDL2_LIBRARY_DIRS}")
    message("  -- SDL LIBS : ${SDL2_LIBRARIES}")
    list(APPEND Windows_INCS ${SDL2_INCLUDE_DIRS})
    list(APPEND Windows_LIB_DIR ${SDL2_LIBRARY_DIRS})
    list(APPEND Windows_LIBS ${SDL2_LIBRARIES})
endif()

if (USE_SDL)
    list(APPEND Windows_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/SDL/SDLWindow.h
        ${CMAKE_CURRENT_SOURCE_DIR}/SDL/SDLWindow.cpp
    )
endif()

# DXGI Windows
if (USE_D3D11)
    list(APPEND Windows_INCS 
        ${MMP_CORE_PRO_TOP_DIR}/Extension/DirectX/Include
    )
endif()

if (USE_D3D11)
    list(APPEND Windows_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/DXGI/DXGICommon.h
        ${CMAKE_CURRENT_SOURCE_DIR}/DXGI/DXGICommon.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/DXGI/DXGIWindow.h
        ${CMAKE_CURRENT_SOURCE_DIR}/DXGI/DXGIWindow.cpp
    )
endif()

# Vulkan Windows
if (USE_VULKAN)
    list(APPEND Windows_INCS
        ${MMP_CORE_PRO_TOP_DIR}/Extension/Vulkan-Headers/include
    )
endif()

if (USE_VULKAN)
    list(APPEND Windows_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/VulkanWindowsCommon.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/VulkanWindowsCommon.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/VulkanInterfaceImpl.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/VulkanInterfaceImpl.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/VulkanWindow.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/VulkanWindow.cpp
    )
endif()

add_library(MMP_Windows OBJECT ${Windows_SRCS})
add_library(Mmp::Windows ALIAS MMP_Windows)
target_include_directories(MMP_Windows PUBLIC ${Windows_INCS})
target_link_libraries(MMP_Windows PUBLIC ${Windows_LIBS} PRIVATE Poco::Foundation Mmp::Common) 
list(LENGTH Windows_LIB_DIR Windows_LIB_DIR_LEN)
if (NOT(${Windows_LIB_DIR_LEN} EQUAL 0))
    link_directories(MMP_Windows ${Windows_LIB_DIR})
endif()

if (USE_EGL)
    target_compile_definitions(MMP_Windows PRIVATE USE_EGL)
    if (USE_GBM)
        target_compile_definitions(MMP_Windows PRIVATE USE_GBM)
    endif()
endif()
if (USE_X11)
    target_compile_definitions(MMP_Windows PRIVATE USE_X11)
endif()
if (USE_SDL)
    target_compile_definitions(MMP_Windows PRIVATE USE_SDL)
endif()
if (USE_D3D11)
    target_compile_definitions(MMP_Windows PRIVATE USE_DXGI)
    if (ENABLE_D3D11_DEBUG)
        target_compile_definitions(MMP_Windows PRIVATE ENABLE_D3D11_DEBUG)
    endif()
endif()
if (USE_VULKAN)
    target_compile_definitions(MMP_Windows PRIVATE USE_VULKAN)
endif()