#include "OpenGLFeature.h"

namespace Mmp
{

const std::string GpuVendorToStr(GpuVendor vendor)
{
    switch (vendor)
    {
        case GpuVendor::NVIDIA: return "NVIDIA";
        case GpuVendor::AMD: return "AMD";
        case GpuVendor::INTEL: return "INTEL";
        case GpuVendor::ARM: return "ARM";
        case GpuVendor::IMGTEC: return "IMGTEC";
        case GpuVendor::QUALCOMM: return "QUALCOMM";
        case GpuVendor::BROADCOM: return "BROADCOM";
        case GpuVendor::VIVANTE: return "VIVANTE";
        case GpuVendor::APPLE: return "APPLE";
        case GpuVendor::UNKNOWN: return "UNKNOWN";
        default: assert(false); return "UNKNOWN";
    }
}

std::ostream& operator<<(std::ostream& os, GpuVendor vendor)
{
    os << GpuVendorToStr(vendor);
    return os;
}

OpenGLFeature::OpenGLFeature()
{
	// Common
	ver[0] = 0;
    ver[1] = 0;
    gpuVendor = GpuVendor::UNKNOWN;
	IsGLES = false;
	GLES3 = false;

	// OES
	OES_depth24 = false;
	OES_packed_depth_stencil = false;
	OES_depth_texture = false;
	OES_texture_npot = false;
	OES_mapbuffer = false;
	OES_vertex_array_object = false;
	OES_copy_image = false;
	OES_texture_float = false;
	OES_texture_3D = false;
	OES_EGL_image_external = false;

	// ARB
	ARB_framebuffer_object = false;
	ARB_pixel_buffer_object = false;
	ARB_blend_func_extended = false;
	EXT_blend_func_extended = false;
	ARB_explicit_attrib_location = false;
	ARB_shader_image_load_store = false;
	ARB_shading_language_420pack = false;
	ARB_conservative_depth = false;
	ARB_copy_image = false;
	ARB_vertex_array_object = false;
	ARB_texture_float = false;
	ARB_draw_instanced = false;
	ARB_buffer_storage = false;
	ARB_cull_distance = false;
	ARB_depth_clamp = false;
	ARB_uniform_buffer_object = false;
	ARB_texture_non_power_of_two = false;
	ARB_shader_stencil_export = false;
	ARB_viewport_array        = false;

	// EXT
	EXT_discard_framebuffer = false;
	EXT_unpack_subimage = false;
	EXT_bgra = false;
	EXT_shader_framebuffer_fetch = false;
	EXT_gpu_shader4 = false;
	EXT_blend_minmax = false;
	EXT_framebuffer_object = false;
	EXT_copy_image = false;
	EXT_texture_filter_anisotropic = false;
	EXT_draw_instanced = false;
	EXT_buffer_storage = false;
	EXT_clip_cull_distance = false;
	EXT_depth_clamp = false;
	EXT_YUV_target = false;

	// NV
	NV_copy_image = false;
	NV_framebuffer_blit = false;
	NV_pixel_buffer_object = false;

	// ARM
	ARM_shader_framebuffer_fetch = false;

	// APPLE
	APPLE_clip_distance = false;

	// EGL
	_EGL_NV_system_time = false;
	_EGL_NV_coverage_sample = false;

	maxVertexTextureUnits = -1;
}

} // namespace Mmp