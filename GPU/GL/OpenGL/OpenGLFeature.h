//
// OpenGLFeature.h
//
// Library: GPU
// Package: GL
// Module:  OpenGL
// 
#pragma once

#include "OpenGLCommon.h"

namespace Mmp
{

/**
 * @brief GPU 厂商
 */
enum class GpuVendor
{
	NVIDIA,
	AMD,
	INTEL,
	ARM,       // Mali
	IMGTEC,    // PowerVR
	QUALCOMM,  // Adreno
	BROADCOM,  // Raspberry PI etc
	VIVANTE,
	APPLE,
	UNKNOWN,
};
const std::string GpuVendorToStr(GpuVendor vendor);
extern std::ostream& operator<<(std::ostream& os, GpuVendor vendor);

/**
 * @brief OpenGL Extension 相关特性
 */
class OpenGLFeature
{
public:
	OpenGLFeature();
public:
	// Common
	int          ver[2];        // GL(ES) 版本号 (major.minor)
 	GpuVendor    gpuVendor;     // GPU 厂商
	std::string  render;        // 渲染器
	bool IsGLES;                // 是否是 OpenGL ES
	bool GLES3;                 // 是否支持 OpenGL ES3	

	// OES
	bool OES_depth24;
	bool OES_packed_depth_stencil;
	bool OES_depth_texture;
	bool OES_texture_npot;
	bool OES_mapbuffer;
	bool OES_vertex_array_object;
	bool OES_copy_image;
	bool OES_texture_float;
	bool OES_texture_3D;

	// ARB
	bool ARB_framebuffer_object;
	bool ARB_pixel_buffer_object;
	bool ARB_blend_func_extended;
	bool EXT_blend_func_extended;
	bool ARB_explicit_attrib_location;
	bool ARB_shader_image_load_store;
	bool ARB_shading_language_420pack;
	bool ARB_conservative_depth;
	bool ARB_copy_image;
	bool ARB_vertex_array_object;
	bool ARB_texture_float;
	bool ARB_draw_instanced;
	bool ARB_buffer_storage;
	bool ARB_cull_distance;
	bool ARB_depth_clamp;
	bool ARB_uniform_buffer_object;
	bool ARB_texture_non_power_of_two;
	bool ARB_shader_stencil_export;
	bool ARB_viewport_array;

	// EXT
	bool EXT_discard_framebuffer;
	bool EXT_unpack_subimage;
	bool EXT_bgra;
	bool EXT_shader_framebuffer_fetch;
	bool EXT_gpu_shader4;
	bool EXT_blend_minmax;
	bool EXT_framebuffer_object;
	bool EXT_copy_image;
	bool EXT_texture_filter_anisotropic;
	bool EXT_draw_instanced;
	bool EXT_buffer_storage;
	bool EXT_clip_cull_distance;
	bool EXT_depth_clamp;

	// NV
	bool NV_copy_image;
	bool NV_framebuffer_blit;
	bool NV_pixel_buffer_object;

	// ARM
	bool ARM_shader_framebuffer_fetch;

	// APPLE
	bool APPLE_clip_distance;

	// EGL
	bool EGL_NV_system_time;
	bool EGL_NV_coverage_sample;

	int maxVertexTextureUnits;
};

} // namespace Mmp