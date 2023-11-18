//
// EGLCommon.h
//
// Library: GPU
// Package: Windows
// Module:  EGL
// 

#pragma once
#include <iostream>
#include <string>

#include <EGL/egl.h>

#include "WindowsCommon.h"

namespace Mmp
{

/**
 * @brief EGL_Client_API
 */
enum class GLMode
{
    OPENGL,
    OPENGL_ES3,
    OPENGL_ES2, // not support
    OPENVG,     // not support
    UNKNOWN,
};
const std::string GLModeToStr(GLMode mode);
extern std::ostream& operator<<(std::ostream& os, GLMode mode);

/** 
 * @sa 1 - Default Frame Buffer : eglspec.1.5.pdf - 3.5 Rendering Surfaces
 *     2 - Frame Buffer Object  : es_spec_3.2.pdf - Chapter 9 Framebuffers and Framebuffer Objects
 */
enum class GLSurfaceType
{
    WINDOWS, // On-Screen  (Default Frame Buffer)
    PBUFFER, // Off-Screen (Default Frame Buffer)
    PIXMAP,  // Off-Screen (Default Frame Buffer) - not support for now
    TEXTURE, // Off-Screen (No Default Frame Buffer)
};
const std::string GLSurfaceTypeToStr(GLSurfaceType type);
extern std::ostream& operator<<(std::ostream& os, GLSurfaceType type);

/**
 * @sa eglspec.1.5.pdf - 3.1 Errors
 */
std::string EGLErrorToStr(EGLint error);

/**
 * @sa eglspec.1.5.pdf - Table 3.1: EGLConfig attributes
 */
std::string EGLAttributeToStr(EGLint attr);

/**
 * @sa eglspec.1.5.pdf - Table 3.3: Types of client APIs supported by an EGLConfig
 */
std::string EGLRenderableType(EGLint type);

} // namespace Mmp 