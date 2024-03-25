#include "OpenGLUtil.h"

#include <cassert>
#include <set>
#include <map>
#include <Poco/String.h>

namespace Mmp
{

static OpenGLFeature gOpenGLFeature;

static bool InitGlew()
{
    GLenum err = glewInit();
    // FIXME : 某些平台下(WLS2 in Win11) glewInit 返回 GLEW_ERROR_NO_GLX_DISPLAY, 实际上能够正常工作
    if (err != GLEW_OK && err != GLEW_ERROR_NO_GLX_DISPLAY)
    {
        return false;
    }
    else
    {
        return true;
    }
}

/**
 * @note for CheckGLError only
 * @sa   es_spec_3.2.pdf - Table 2.3: Summary of GL errors
 */
static const std::string OpenGLErrorToStr(GLenum err)
{
    switch (err)
    {
        case GL_NO_ERROR:                       return "GL_NO_ERROR";
        case GL_CONTEXT_LOST:                   return "GL_CONTEXT_LOST";
        case GL_INVALID_ENUM:                   return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE:                  return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION:              return "GL_INVALID_OPERATION";
        case GL_INVALID_FRAMEBUFFER_OPERATION:  return "GL_INVALID_FRAMEBUFFER_OPERATION";
        case GL_OUT_OF_MEMORY:                  return "GL_OUT_OF_MEMORY";
        case GL_STACK_OVERFLOW:                 return "GL_STACK_OVERFLOW";
        case GL_STACK_UNDERFLOW:                return "GL_STACK_UNDERFLOW";
        // TODO : supply the missing error part
        default:                                return "UNKNOWN_" + std::to_string(err);
    }
}

/**
 * @note modify from ppsspp, see 
 *       `https://github.com/hrydgard/ppsspp/blob/master/Common/GPU/OpenGL/GLFeatures.cpp` 
 *       - bool CheckGLExtensions() for more information
 * @sa   https://registry.khronos.org/OpenGL/index_es.php#otherextspecs
 */
bool CheckOpenGLExtensions()
{
    static bool extensionsDone = false;
    if (extensionsDone)
    {
        return true;
    }

    OpenGLFeature openGLFeature;
    if (!InitGlew())
    {
        assert(false);
    }
	const char *renderer = (const char *)glGetString(GL_RENDERER);
	const char *versionStr = (const char *)glGetString(GL_VERSION);
	const char *glslVersionStr = (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION);

	if (!renderer || !versionStr || !glslVersionStr) {
		GL_LOG_ERROR << "Can not set up OpenGL";
		return false;
	}
    else
    {
        extensionsDone = true;
    }

    const char* cvendor = (char *)glGetString(GL_VENDOR);
    if (cvendor)
    {
        std::string vendor = Poco::trim(std::string(cvendor));
		if (vendor == "NVIDIA Corporation"
			|| vendor == "Nouveau"
			|| vendor == "nouveau") {
			openGLFeature.gpuVendor = GpuVendor::NVIDIA;
		} 
        else if (vendor == "Advanced Micro Devices, Inc."
			|| vendor == "ATI Technologies Inc."
			|| vendor == "AMD") {
			openGLFeature.gpuVendor = GpuVendor::AMD;
		} 
        else if (vendor == "Intel"
			|| vendor == "Intel Inc."
			|| vendor == "Intel Corporation"
			|| vendor == "Tungsten Graphics, Inc") 
        { 
			openGLFeature.gpuVendor = GpuVendor::INTEL;
		} 
        else if (vendor == "ARM") 
        {
			openGLFeature.gpuVendor = GpuVendor::ARM;
		} 
        else if (vendor == "Imagination Technologies") 
        {
			openGLFeature.gpuVendor = GpuVendor::IMGTEC;
		} 
        else if (vendor == "Qualcomm") 
        {
			openGLFeature.gpuVendor = GpuVendor::QUALCOMM;
		} 
        else if (vendor == "Broadcom") 
        {
			openGLFeature.gpuVendor = GpuVendor::BROADCOM;
		} 
        else if (vendor == "Vivante Corporation") 
        {
			openGLFeature.gpuVendor = GpuVendor::VIVANTE;
		}
        else if (vendor == "Apple Inc." || vendor == "Apple") 
        {
			openGLFeature.gpuVendor = GpuVendor::APPLE;
		} 
        else 
        {
			openGLFeature.gpuVendor = GpuVendor::UNKNOWN;
		}
    }
    else
    {
        openGLFeature.gpuVendor = GpuVendor::UNKNOWN;
    }
    openGLFeature.render = std::string(renderer);
    glGetIntegerv(GL_MAJOR_VERSION, &(openGLFeature.ver[0]));
    glGetIntegerv(GL_MINOR_VERSION, &(openGLFeature.ver[1]));

    GL_LOG_INFO;
    GL_LOG_INFO << "OpenGL brief info:";
    GL_LOG_INFO << "-- GPU Vendor is: " << openGLFeature.gpuVendor;
    GL_LOG_INFO << "-- Render is: " << openGLFeature.render;
    GL_LOG_INFO << "-- Version is: " << openGLFeature.ver[0] << "." << openGLFeature.ver[1];
    GL_LOG_INFO << "-- GLSL version is: " << glslVersionStr;
    GL_LOG_INFO;

    std::set<std::string> glExtensions;
    GLint                 numExtensions = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
    GL_LOG_INFO << "OpenGL Extension num is:" << numExtensions;
    for (GLint i=0; i<numExtensions; i++)
    {
        const char* ext = (const char*)glGetStringi(GL_EXTENSIONS, i);
        glExtensions.insert(std::string(ext));
        GL_LOG_INFO << "-- (" << i << ") Extension : " << ext;
    }

    if (std::string(versionStr).find("OpenGL ES") != std::string::npos)
    {
        openGLFeature.IsGLES = true;
        if (openGLFeature.ver[0] == 3)
        {
            openGLFeature.GLES3 = true;
        }
        // Hint : only support OpenGL ES3
        assert(openGLFeature.GLES3);
    }

	openGLFeature.ARB_conservative_depth = glExtensions.count("GL_ARB_conservative_depth") != 0;
	openGLFeature.ARB_shader_image_load_store = (glExtensions.count("GL_ARB_shader_image_load_store") != 0) || (glExtensions.count("GL_EXT_shader_image_load_store") != 0);
	openGLFeature.ARB_shading_language_420pack = (glExtensions.count("GL_ARB_shading_language_420pack") != 0);
	openGLFeature.EXT_bgra = glExtensions.count("GL_EXT_bgra") != 0;
	openGLFeature.EXT_gpu_shader4 = glExtensions.count("GL_EXT_gpu_shader4") != 0;
	openGLFeature.NV_framebuffer_blit = glExtensions.count("GL_NV_framebuffer_blit") != 0;
	openGLFeature.NV_copy_image = glExtensions.count("GL_NV_copy_image") != 0;
	openGLFeature.OES_copy_image = glExtensions.count("GL_OES_copy_image") != 0;
	openGLFeature.EXT_copy_image = glExtensions.count("GL_EXT_copy_image") != 0;
	openGLFeature.ARB_copy_image = glExtensions.count("GL_ARB_copy_image") != 0;
	openGLFeature.ARB_buffer_storage = glExtensions.count("GL_ARB_buffer_storage") != 0;
	openGLFeature.ARB_vertex_array_object = glExtensions.count("GL_ARB_vertex_array_object") != 0;
	openGLFeature.ARB_texture_float = glExtensions.count("GL_ARB_texture_float") != 0;
	openGLFeature.EXT_texture_filter_anisotropic = glExtensions.count("GL_EXT_texture_filter_anisotropic") != 0 || glExtensions.count("GL_ARB_texture_filter_anisotropic") != 0;
	openGLFeature.EXT_draw_instanced = glExtensions.count("GL_EXT_draw_instanced") != 0;
	openGLFeature.ARB_draw_instanced = glExtensions.count("GL_ARB_draw_instanced") != 0;
	openGLFeature.ARB_cull_distance = glExtensions.count("GL_ARB_cull_distance") != 0;
	openGLFeature.ARB_depth_clamp = glExtensions.count("GL_ARB_depth_clamp") != 0;
	openGLFeature.ARB_uniform_buffer_object = glExtensions.count("GL_ARB_uniform_buffer_object") != 0;
	openGLFeature.ARB_explicit_attrib_location = glExtensions.count("GL_ARB_explicit_attrib_location") != 0;
	openGLFeature.ARB_texture_non_power_of_two = glExtensions.count("GL_ARB_texture_non_power_of_two") != 0;
	openGLFeature.ARB_shader_stencil_export = glExtensions.count("GL_ARB_shader_stencil_export") != 0;
    openGLFeature.ARB_viewport_array = glExtensions.count("GL_ARB_viewport_array") != 0;
    if (openGLFeature.IsGLES) 
    {
		openGLFeature.EXT_blend_func_extended = glExtensions.count("GL_EXT_blend_func_extended") != 0;
		openGLFeature.OES_texture_npot = glExtensions.count("GL_OES_texture_npot") != 0;
		openGLFeature.OES_packed_depth_stencil = (glExtensions.count("GL_OES_packed_depth_stencil") != 0) || openGLFeature.GLES3;
		openGLFeature.OES_depth24 = glExtensions.count("GL_OES_depth24") != 0;
		openGLFeature.OES_depth_texture = glExtensions.count("GL_OES_depth_texture") != 0;
		openGLFeature.OES_mapbuffer = glExtensions.count("GL_OES_mapbuffer") != 0;
		openGLFeature.EXT_blend_minmax = glExtensions.count("GL_EXT_blend_minmax") != 0;
		openGLFeature.EXT_unpack_subimage = glExtensions.count("GL_EXT_unpack_subimage") != 0;
		openGLFeature.EXT_shader_framebuffer_fetch = glExtensions.count("GL_EXT_shader_framebuffer_fetch") != 0;
		openGLFeature.ARM_shader_framebuffer_fetch = glExtensions.count("GL_ARM_shader_framebuffer_fetch") != 0;
		openGLFeature.OES_texture_float = glExtensions.count("GL_OES_texture_float") != 0;
		openGLFeature.OES_texture_3D = glExtensions.count("GL_OES_texture_3D") != 0;
        openGLFeature.OES_EGL_image_external = glExtensions.count("GL_OES_EGL_image_external") != 0;
		openGLFeature.EXT_buffer_storage = glExtensions.count("GL_EXT_buffer_storage") != 0;
		openGLFeature.EXT_clip_cull_distance = glExtensions.count("GL_EXT_clip_cull_distance") != 0;
		openGLFeature.EXT_depth_clamp = glExtensions.count("GL_EXT_depth_clamp") != 0;
		openGLFeature.APPLE_clip_distance = glExtensions.count("GL_APPLE_clip_distance") != 0;
        openGLFeature.EXT_YUV_target = glExtensions.count("GL_EXT_YUV_target") != 0;
		openGLFeature.OES_vertex_array_object = false;
		openGLFeature.EXT_discard_framebuffer = false;
	} 
    else 
    {
		openGLFeature.ARB_blend_func_extended = glExtensions.count("GL_ARB_blend_func_extended") != 0;
		openGLFeature.EXT_blend_minmax = true;
		openGLFeature.EXT_unpack_subimage = true;
	}

	if (openGLFeature.GLES3) 
    {
		openGLFeature.EXT_blend_minmax = true;
		openGLFeature.EXT_unpack_subimage = true;
	}

    glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &openGLFeature.maxVertexTextureUnits);

	openGLFeature.ARB_framebuffer_object = glExtensions.count("GL_ARB_framebuffer_object") != 0;
	openGLFeature.EXT_framebuffer_object = glExtensions.count("GL_EXT_framebuffer_object") != 0;
	openGLFeature.ARB_pixel_buffer_object = glExtensions.count("GL_ARB_pixel_buffer_object") != 0;
	openGLFeature.NV_pixel_buffer_object = glExtensions.count("GL_NV_pixel_buffer_object") != 0;

	if (!openGLFeature.IsGLES) 
    {
		if (OpenGLVersionGreater(openGLFeature, 3, 0)) 
        {
			openGLFeature.ARB_texture_float = true;
		}
		if (OpenGLVersionGreater(openGLFeature, 3, 1)) 
        {
			openGLFeature.ARB_draw_instanced = true;
			openGLFeature.ARB_uniform_buffer_object = true;
		}
		if (OpenGLVersionGreater(openGLFeature, 3, 2)) 
        {
			openGLFeature.ARB_depth_clamp = true;
		}
		if (OpenGLVersionGreater(openGLFeature, 3, 3)) 
        {
			openGLFeature.ARB_blend_func_extended = true;
			openGLFeature.ARB_explicit_attrib_location = true;
		}
		if (OpenGLVersionGreater(openGLFeature, 4, 3)) 
        {
			openGLFeature.ARB_copy_image = true;
		}
		if (OpenGLVersionGreater(openGLFeature, 4, 4)) 
        {
			openGLFeature.ARB_buffer_storage = true;
		}
		if (OpenGLVersionGreater(openGLFeature, 4, 5)) 
        {
			openGLFeature.ARB_cull_distance = true;
		}
		if (OpenGLVersionGreater(openGLFeature, 4, 6)) 
        {
			openGLFeature.EXT_texture_filter_anisotropic = true;
		}
	}
    gOpenGLFeature = openGLFeature;
    return true;
}

const OpenGLFeature& GetOpenGLFeature()
{
    return gOpenGLFeature;
}

bool OpenGLVersionGreater(const OpenGLFeature& openGLFeature, int major, int minor)
{
	if (openGLFeature.ver[0] > major)
		return true;
	if (openGLFeature.ver[0] < major)
		return false;
	if (openGLFeature.ver[1] > minor)
		return true;
	if (openGLFeature.ver[1] < minor)
		return false;
    return true;
}

void InfoOpenGLFeature(const OpenGLFeature& openGLFeature)
{
    std::map<bool, std::string> lookup = 
    {
        {true, "support"},
        {false, "no_support"}
    };

    GL_LOG_INFO;
    GL_LOG_INFO << "OpenGL extension feature info";
    GL_LOG_INFO << "[common]";
    GL_LOG_INFO << "gpu_type=" << (openGLFeature.IsGLES ? "OpenGL ES" : "OpenGL");
    GL_LOG_INFO << "gpu_version=" << openGLFeature.ver[0] << "." << openGLFeature.ver[1];
    GL_LOG_INFO << "gpu_vendor=" << openGLFeature.gpuVendor;
    GL_LOG_INFO << "gpu_render=" << openGLFeature.render;
    GL_LOG_INFO << "[OES]";
    GL_LOG_INFO << "GL_OES_depth24=" << lookup[openGLFeature.OES_depth24];
    GL_LOG_INFO << "GL_OES_packed_depth_stencil=" << lookup[openGLFeature.OES_packed_depth_stencil];
    GL_LOG_INFO << "GL_OES_depth_texture=" << lookup[openGLFeature.OES_depth_texture];
    GL_LOG_INFO << "GL_OES_texture_npot=" << lookup[openGLFeature.OES_texture_npot];
    GL_LOG_INFO << "GL_OES_mapbuffer=" << lookup[openGLFeature.OES_mapbuffer];
    GL_LOG_INFO << "GL_OES_vertex_array_object=" << lookup[openGLFeature.OES_vertex_array_object];
    GL_LOG_INFO << "GL_OES_copy_image=" << lookup[openGLFeature.OES_copy_image];
    GL_LOG_INFO << "GL_OES_EGL_image_external=" << lookup[openGLFeature.OES_EGL_image_external];
    GL_LOG_INFO << "GL_OES_texture_float=" << lookup[openGLFeature.OES_texture_float];
    GL_LOG_INFO << "GL_OES_texture_3D=" << lookup[openGLFeature.OES_texture_3D];
    GL_LOG_INFO << "[ARB]";
    GL_LOG_INFO << "GL_ARB_framebuffer_object=" << lookup[openGLFeature.ARB_framebuffer_object];
    GL_LOG_INFO << "GL_ARB_pixel_buffer_object=" << lookup[openGLFeature.ARB_pixel_buffer_object];
    GL_LOG_INFO << "GL_ARB_blend_func_extended=" << lookup[openGLFeature.ARB_blend_func_extended];
    GL_LOG_INFO << "GL_EXT_blend_func_extended=" << lookup[openGLFeature.EXT_blend_func_extended];
    GL_LOG_INFO << "GL_ARB_explicit_attrib_location=" << lookup[openGLFeature.ARB_explicit_attrib_location];
    GL_LOG_INFO << "GL_ARB_conservative_depth=" << lookup[openGLFeature.ARB_conservative_depth];
    GL_LOG_INFO << "GL_ARB_copy_image=" << lookup[openGLFeature.ARB_copy_image];
    GL_LOG_INFO << "GL_ARB_vertex_array_object=" << lookup[openGLFeature.ARB_vertex_array_object];
    GL_LOG_INFO << "GL_ARB_texture_float=" << lookup[openGLFeature.ARB_texture_float];
    GL_LOG_INFO << "GL_ARB_draw_instanced=" << lookup[openGLFeature.ARB_draw_instanced];
    GL_LOG_INFO << "GL_ARB_buffer_storage=" << lookup[openGLFeature.ARB_buffer_storage];
    GL_LOG_INFO << "GL_ARB_cull_distance=" << lookup[openGLFeature.ARB_cull_distance];
    GL_LOG_INFO << "GL_ARB_depth_clamp=" << lookup[openGLFeature.ARB_depth_clamp];
    GL_LOG_INFO << "GL_ARB_uniform_buffer_object=" << lookup[openGLFeature.ARB_uniform_buffer_object];
    GL_LOG_INFO << "GL_ARB_texture_non_power_of_two=" << lookup[openGLFeature.ARB_texture_non_power_of_two];
    GL_LOG_INFO << "GL_ARB_shader_stencil_export=" << lookup[openGLFeature.ARB_shader_stencil_export];
    GL_LOG_INFO << "GL_ARB_viewport_array=" << lookup[openGLFeature.ARB_viewport_array];
    GL_LOG_INFO << "[EXT]";
    GL_LOG_INFO << "GL_EXT_discard_framebuffer=" << lookup[openGLFeature.EXT_discard_framebuffer];
    GL_LOG_INFO << "GL_EXT_unpack_subimage=" << lookup[openGLFeature.EXT_unpack_subimage];
    GL_LOG_INFO << "GL_EXT_bgra=" << lookup[openGLFeature.EXT_bgra];
    GL_LOG_INFO << "GL_EXT_shader_framebuffer_fetch=" << lookup[openGLFeature.EXT_shader_framebuffer_fetch];
    GL_LOG_INFO << "GL_EXT_gpu_shader4=" << lookup[openGLFeature.EXT_gpu_shader4];
    GL_LOG_INFO << "GL_EXT_blend_minmax=" << lookup[openGLFeature.EXT_blend_minmax];
    GL_LOG_INFO << "GL_EXT_framebuffer_object=" << lookup[openGLFeature.EXT_framebuffer_object];
    GL_LOG_INFO << "GL_EXT_copy_image=" << lookup[openGLFeature.EXT_copy_image];
    GL_LOG_INFO << "GL_EXT_texture_filter_anisotropic=" << lookup[openGLFeature.EXT_texture_filter_anisotropic];
    GL_LOG_INFO << "GL_EXT_draw_instanced=" << lookup[openGLFeature.EXT_draw_instanced];
    GL_LOG_INFO << "GL_EXT_buffer_storage=" << lookup[openGLFeature.EXT_buffer_storage];
    GL_LOG_INFO << "GL_EXT_clip_cull_distance=" << lookup[openGLFeature.EXT_clip_cull_distance];
    GL_LOG_INFO << "GL_EXT_depth_clamp=" << lookup[openGLFeature.EXT_depth_clamp];
    GL_LOG_INFO << "GL_EXT_YUV_target=" << lookup[openGLFeature.EXT_YUV_target];
    GL_LOG_INFO << "[NV]";
    GL_LOG_INFO << "GL_NV_copy_image=" << lookup[openGLFeature.NV_copy_image];
    GL_LOG_INFO << "GL_NV_framebuffer_blit=" << lookup[openGLFeature.NV_framebuffer_blit];
    GL_LOG_INFO << "GL_NV_pixel_buffer_object=" << lookup[openGLFeature.NV_pixel_buffer_object];
    GL_LOG_INFO << "[ARM]";
    GL_LOG_INFO << "GL_ARM_shader_framebuffer_fetch=" << lookup[openGLFeature.ARM_shader_framebuffer_fetch];
    GL_LOG_INFO << "[APPLE]";
    GL_LOG_INFO << "GL_APPLE_clip_distance=" << lookup[openGLFeature.APPLE_clip_distance];
    GL_LOG_INFO << "[EGL]";
    GL_LOG_INFO << "EGL_NV_system_time=" << lookup[openGLFeature._EGL_NV_system_time];
    GL_LOG_INFO << "EGL_NV_coverage_sample=" << lookup[openGLFeature._EGL_NV_coverage_sample];
    GL_LOG_INFO;
    GL_LOG_INFO << "maxVertexTextureUnits=" << openGLFeature.maxVertexTextureUnits;
    GL_LOG_INFO;
}

void Uint8x4ToFloat(float f[4], uint32_t u)
{
	f[0] = ((u >> 0) & 0xFF) * (1.0f / 255.0f);
	f[1] = ((u >> 8) & 0xFF) * (1.0f / 255.0f);
	f[2] = ((u >> 16) & 0xFF) * (1.0f / 255.0f);
	f[3] = ((u >> 24) & 0xFF) * (1.0f / 255.0f);
}

void ClearError()
{
    GLenum err = glGetError();
    while (err != GL_NO_ERROR)
    {
        err = glGetError();
    }
}

bool CheckGLError(const std::string& file, uint64_t line)
{
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        Mmp::LogMessage(Mmp::AbstractLogger::Level::L_WARN, file, (uint32_t)line, "GL") << "error is: " << OpenGLErrorToStr(err);
        return false;
    }
    return true;
}

void IgnoreGLError()
{
    glGetError();
}

} // namespace Mmp