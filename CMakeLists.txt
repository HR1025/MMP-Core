cmake_minimum_required(VERSION 3.8)
project(MMP-Core DESCRIPTION "Multi Media Plugin - Core")

#################################################################################################
#                              Global Vriable(BEGIN)                                            #
#################################################################################################
set(MMP_SYSTEM_DEVICE "UNKNOWN")
set(MMP_CORE_PRO_TOP_DIR ${CMAKE_CURRENT_SOURCE_DIR})
#################################################################################################
#                              Global Vriable(END)                                              #
#################################################################################################

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#################################################################################################
#                              Project Configuration(BEGIN)                                     #
#################################################################################################
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)

# dependencies configuration
option(USE_EGL         "Windows Backend EGL(Android)" OFF)
option(USE_X11         "Windows Backend X11(Desktop)" OFF)
option(USE_SDL         "Windows Backend SDL(Desktop)" OFF)
option(USE_OPENGL      "Graphics Library OpenGL(ES3)" OFF)
option(USE_D3D11       "Graphics Libray Direct 3D"    OFF)
option(USE_VULKAN      "Graphics Library Vulkan"      OFF) 
option(USE_VAAPI       "An implemention for VA-API(Video Acceleration API)" OFF)
option(USE_SPRIV_CROSS "Use to convert SPRIV to other shader languages" ON)
option(USE_GBM         "EGL backend GBM"              OFF)
option(USE_API_RPC     "RPC(Remote Process Communication) Interface (implement through HTTP OR WEBSOCKET etc.)" OFF)
option(USE_OPENH264    "H264 Software Encoder/Decoder" OFF)
option(NOT_AUTO_EGL "Not auto append EGL dependence" OFF)
#################################################################################################
#                              Project Configuration(END)                                       #
#################################################################################################

# Load custom cmake func
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)
include(utility)
MMP_CHECK_SYSTEM_DEVICE()
include(poco_config)
MMP_POCO_CONFIG()
include(spriv_cross)
MMP_SPRIV_CROSS_CONFIG()
include(vma)
MMP_VMA_CONFIG()
if (MSVC)
    include(msvc)
    MMP_MSVC_AUTO_CONFIG()
endif()

include(eigen/eigen)
add_subdirectory(CMake/LodePng)
if (USE_OPENH264)
    add_subdirectory(CMake/openH264)
endif()
add_subdirectory(Extension/GLEW)
add_subdirectory(Common)
add_subdirectory(Codec)
add_subdirectory(GPU)

if (USE_API_RPC)
    add_subdirectory(${MMP_CORE_PRO_TOP_DIR}/RPC)
endif()

#################################################################################################
#                                   Project Info(BEGIN)                                         #
#################################################################################################
message("PROJECT NAME : MMP-Core(Multi Media Plugin-Core)")
message("SYSTEM NAME : ${CMAKE_SYSTEM_NAME}")
message("SYSTEM DEVICE : ${MMP_SYSTEM_DEVICE}")
message("PROJECT CONFIGURATION")
message("  -- USE_EGL : ${USE_EGL}")
message("  -- USE_X11 : ${USE_X11}")
message("  -- USE_SDL : ${USE_SDL}")
message("  -- USE_OPENGL : ${USE_OPENGL}")
message("  -- USE_GBM : ${USE_GBM}")
message("  -- USE_D3D11 : ${USE_D3D11}")
message("  -- USE_VULKAN : ${USE_VULKAN}")
message("  -- USE_VAAPI : ${USE_VAAPI}")
message("  -- USE_SPRIV_CROSS : ${USE_SPRIV_CROSS}")
message("  -- USE_API_RPC : ${USE_API_RPC}")
message("  -- USE_OPENH264 : ${USE_OPENH264}")
#################################################################################################
#                                   Project Info(END)                                         #
#################################################################################################