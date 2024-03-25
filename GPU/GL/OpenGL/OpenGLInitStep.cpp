#include "Any.h"
#include "GLCommon.h"
#include "OpenGL.h"

#include <cassert>
#include <iomanip>
#include <memory>

#if MMP_PLATFORM(LINUX)
#include "GL/glew.h"
#include "GL/eglew.h"
#endif /* MMP_PLATFORM(LINUX) */

#include "GLDrawContex.h"
#include "OpenGLUtil.h"
#include "OpenGLContex.h"
#include "OpenGLInitStepData.h"

#include "GPU/Windows/AbstractWindows.h"
#include "Common/PixelsInfo.h"
#include "Common/PixelFormat.h"
#include "Common/AbstractDeviceAllocateMethod.h"

#if MMP_PLATFORM(LINUX)
namespace Mmp
{
//
// Hint : this section is copyed from `drm/drm_fourcc.h`
//
// Todo     : ARM 平台下某些 Soc 具有特殊 AFBC (Arm Frame buffer compression) 功能, 是否需要支持其导入 (不过相关资料很少,适配难度挺大)
// Brief    : AFBC 是总线带宽压缩技术,数据从 Process Uint 出去时压缩,进入 Process Uint 时解压, 从而在总线传输时数据的带宽下降
// See aslo : https://lkml.org/lkml/2018/7/10/360
//

#define mmp_fourcc_code(a, b, c, d) ((uint32_t)(a) | ((uint32_t)(b) << 8) | \
				                    ((uint32_t)(c) << 16) | ((uint32_t)(d) << 24))


#define MMP_DRM_FORMAT_ABGR8888	mmp_fourcc_code('A', 'B', '2', '4') /* [31:0] A:B:G:R 8:8:8:8 little endian */

/* 24 bpp RGB */
#define MMP_DRM_FORMAT_RGB888	mmp_fourcc_code('R', 'G', '2', '4') /* [23:0] R:G:B little endian */
#define MMP_DRM_FORMAT_BGR888	mmp_fourcc_code('B', 'G', '2', '4') /* [23:0] B:G:R little endian */

/* packed YCbCr */
#define MMP_DRM_FORMAT_YUYV		mmp_fourcc_code('Y', 'U', 'Y', 'V') /* [31:0] Cr0:Y1:Cb0:Y0 8:8:8:8 little endian */

/*
 * 1-plane YUV 4:2:0
 * In these formats, the component ordering is specified (Y, followed by U
 * then V), but the exact Linear layout is undefined.
 * These formats can only be used with a non-Linear modifier.
 */
#define MMP_DRM_FORMAT_YUV420_8BIT	mmp_fourcc_code('Y', 'U', '0', '8')

/*
 * 2 plane YCbCr
 * index 0 = Y plane, [7:0] Y
 * index 1 = Cr:Cb plane, [15:0] Cr:Cb little endian
 * or
 * index 1 = Cb:Cr plane, [15:0] Cb:Cr little endian
 */
#define MMP_DRM_FORMAT_NV12		mmp_fourcc_code('N', 'V', '1', '2') /* 2x2 subsampled Cr:Cb plane */

void* CreateEGLImage(OpenGLRenderTexture::ptr tex, AbstractDeviceAllocateMethod::ptr allocate)
{
    EGLImageKHR image = nullptr;
    static PFNEGLCREATEIMAGEKHRPROC _eglCreateImageKHR = nullptr;
    AbstractWindows::ptr windows = GLDrawContex::Instance()->GetWindows();
    if (windows && !_eglCreateImageKHR)
    {
        _eglCreateImageKHR = (PFNEGLCREATEIMAGEKHRPROC)windows->GetProcAddress("eglCreateImageKHR");
    }
    if (_eglCreateImageKHR)
    {
        EGLDisplay display = AnyCast<EGLDisplay>(windows->Get("EGLDisplay"));
        EGLContext context = AnyCast<EGLContext>(windows->Get("EGLContext"));
        switch (tex->format)
        {
            case DataFormat::R8G8B8A8_UINT:
            {
                EGLint attr[] = 
                {
                    EGL_WIDTH, tex->w,
                    EGL_HEIGHT, tex->h,
                    EGL_LINUX_DRM_FOURCC_EXT, MMP_DRM_FORMAT_ABGR8888,
                    EGL_DMA_BUF_PLANE0_FD_EXT, allocate->GetFd(),
                    EGL_DMA_BUF_PLANE0_OFFSET_EXT, 0,
                    EGL_DMA_BUF_PLANE0_PITCH_EXT, tex->w,
                    EGL_NONE
                };
                image = _eglCreateImageKHR(display, context, EGL_LINUX_DMA_BUF_EXT, (EGLClientBuffer)nullptr, attr);
                break;
            }
            case DataFormat::NV12_UINT:
            {
                EGLint attr[] = 
                {
                    EGL_WIDTH, tex->w,
                    EGL_HEIGHT, tex->h,
                    EGL_LINUX_DRM_FOURCC_EXT,  MMP_DRM_FORMAT_NV12,
                    EGL_DMA_BUF_PLANE0_FD_EXT, allocate->GetFd(),
                    EGL_DMA_BUF_PLANE0_OFFSET_EXT, 0,
                    EGL_DMA_BUF_PLANE0_PITCH_EXT, tex->w,
                    EGL_DMA_BUF_PLANE1_FD_EXT, allocate->GetFd(),
                    EGL_DMA_BUF_PLANE1_OFFSET_EXT, tex->w * tex->h,
                    EGL_DMA_BUF_PLANE1_PITCH_EXT, tex->w,
                    EGL_NONE
                };
                image = _eglCreateImageKHR(display, context, EGL_LINUX_DMA_BUF_EXT, (EGLClientBuffer)nullptr, attr);
                break;
            }
            default:
                break;
        } 
    }
    return image;
}

} // namespace Mmp
#endif /* MMP_PLATFORM(LINUX) */

namespace Mmp
{

/**
 * @brief 查询 compile shader 和 link program 的错误日志
 */
template <typename Getiv, typename GetLog>
static std::string GetInfoLog(GLuint name, Getiv getiv, GetLog getLog) 
{
	GLint bufLength = 0;
	getiv(name, GL_INFO_LOG_LENGTH, &bufLength);
	if (bufLength <= 0)
    {
        bufLength = 2048;
    }
	std::string infoLog;
	infoLog.resize(bufLength);
	GLsizei len = 0;
	getLog(name, (GLsizei)infoLog.size(), &len, &infoLog[0]);
	if (len <= 0)
		return "(unknown reason)";

	infoLog.resize(len);
	return infoLog;
}

/**
 * @brief 添加行号 (for debug usage)
 */
static std::string GlslCodeWithLineNumber(const std::string& code)
{
    std::stringstream input(code);
    std::stringstream output;
    std::string line;
    int lineNumber = 1;
    while (std::getline(input, line))
    {
        output << std::setw(4) << lineNumber++ << ": " << line << std::endl;
    }
    return output.str();
}

} // namespace Mmp

namespace Mmp
{

GLuint OpenGL::InitStepCreateTexture(Any data)
{
    assert(data.type() == typeid(OpenGLCreateTextureInitData));
    GL_LOG_TRACE << "[Init Step] Create Texture";
    const OpenGLCreateTextureInitData& createTextureData = RefAnyCast<OpenGLCreateTextureInitData>(data);
    OpenGLRenderTexture::ptr tex = createTextureData.texture;
    glGenTextures(1, &(tex->texture));
    glBindTexture(tex->target, tex->texture);
    CHECK_GL_ERROR_IF_DEBUG();
    return tex->texture;
}

void OpenGL::InitStepCreateBuffer(Any data)
{
    assert(data.type() == typeid(OpenGLCreateBufferInitData));
    GL_LOG_TRACE << "[Init Step] Create Buffer";
    const OpenGLCreateBufferInitData& createBufferData = RefAnyCast<OpenGLCreateBufferInitData>(data);
    OpenGLRenderBuffer::ptr buffer = createBufferData.buffer;
    glGenBuffers(1, &(buffer->buffer));
    glBindBuffer(buffer->target, buffer->buffer);
    glBufferData(buffer->target, createBufferData.size, nullptr, createBufferData.usage);
    CHECK_GL_ERROR_IF_DEBUG();
}

/**
 * @note always use buffer subdata to updata VBO (vertex buffer object) or IBO (index buffer object)
 * @todo may be use `glMapBufferRange` method also a good solution
 * @sa   es_spec_3.2.pdf - 6.3 Mapping and Unmapping Buffer Data
 */
void OpenGL::InitStepBufferSubdata(Any data)
{
    assert(data.type() == typeid(OpenGLBufferSubdataInitData));
    GL_LOG_TRACE << "[Init Step] Buffer Subdata";
    const OpenGLBufferSubdataInitData& bufferSubdataData = RefAnyCast<OpenGLBufferSubdataInitData>(data);
    OpenGLRenderBuffer::ptr buffer = bufferSubdataData.buffer;
    glBindBuffer(buffer->target, buffer->buffer);
    glBufferSubData(buffer->target, bufferSubdataData.offset, bufferSubdataData.size, bufferSubdataData.data->GetData());
    CHECK_GL_ERROR_IF_DEBUG();
}

void OpenGL::InitStepCreateProgram(Any data)
{
    assert(data.type() == typeid(OpenGLCreateProgramInitData));
    GL_LOG_TRACE << "[Init Step] Create Program";
    const OpenGLCreateProgramInitData& createProgramData = RefAnyCast<OpenGLCreateProgramInitData>(data);
    OpenGLRenderProgram::ptr program = createProgramData.program;
    program->program = glCreateProgram();
    bool hasAnyFailed = false;
    CHECK_GL_ERROR_IF_DEBUG();
    for (uint32_t j=0; j<createProgramData.shaderNum; j++)
    {
        assert(createProgramData.shaders[j]);
        hasAnyFailed = hasAnyFailed || createProgramData.shaders[j]->failed;
        glAttachShader(program->program, createProgramData.shaders[j]->shader);
    }
    for (auto& semantics : program->semantics)
    {
        // Hint : attribute  and  varying  qualifiers are removed from glsl specification
        // Example （xxx.vert）
	    //      #if __VERSION__ >= 130
	    //          #define attribute in
	    //          #define varying out
	    //      #endif
        glBindAttribLocation(program->program, semantics.location, semantics.attrib.c_str());
    }
    if (OpenGLVersionGreater(_openGLFeature, 3, 0))
    {
        // Hint : fragColor0 is used to inseted of gl_FragColor
        // Example (xxx.frag)
        //    #if __VERSION__ >= 130
        //         #define gl_FragColor fragColor0
        //         out vec4 fragColor0;
        //    #endif
        // See aslo : GLSLangSpec.4.60.pdf - 14.2.5. Mapping Variables (gl_FragColor)
        // The fragment-stage built-in gl_FragColor, which implies a broadcast to all outputs, is not present in
        // SPIR-V.  Shaders  where  writing  to  gl_FragColor  is  allowed  can  still  write  to  it,  but  it  only  means  to
        // write to an output:
        // • of the same type as gl_FragColor
        // • decorated with location 0
        // • not decorated as a built-in variable.
        // There is no implicit broadcast
        if (glBindFragDataLocation)
        {
            glBindFragDataLocation(program->program, 0, "fragColor0");
            glBindFragDataLocation(program->program, 1, "fragColor1");
            glBindFragDataLocation(program->program, 2, "fragColor2");
        }
    }
    glLinkProgram(program->program);
    GLint linkStatus = GL_FALSE;
    glGetProgramiv(program->program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE)
    {
        std::string infoLog = GetInfoLog(program->program, glGetProgramiv, glGetProgramInfoLog);
        OpenGLRenderShader::ptr vs = createProgramData.shaders[0];
        OpenGLRenderShader::ptr fs = createProgramData.shaderNum > 1 ? createProgramData.shaders[1] : nullptr;
        std::string   vsDesc = vs->desc + (vs->failed ? "(failed)" : "");
        std::string   fsDesc = fs ? (fs->desc + (fs->failed ? "(failed)" : "")) : "none";
        std::string   vsCode = vs->code;
        std::string   fsCode = fs ? fs->code : "none";
        GL_LOG_ERROR << "Could not link program:\n" << infoLog;
        GL_LOG_ERROR << "VS desc : \n" << vsDesc;
        GL_LOG_ERROR << "FS desc : \n" << fsDesc;
        GL_LOG_ERROR << "[VS]\n" << vsCode;
        GL_LOG_ERROR << "[FS]\n" << fsCode;
        CHECK_GL_ERROR_IF_DEBUG();
        assert(false);
        return;
    }
    glUseProgram(program->program);
    CHECK_GL_ERROR_IF_DEBUG();
    // Query all the uniforms.
    for (size_t j=0; j<program->queries.size(); j++)
    {
        auto& query = program->queries[j];
        assert(!query.name.empty());
        // See also : GLSL_ES_Specification_3.20.pdf - 4.3.4. Input Variables
        // Vertex shader input variables (or attributes) receive per-vertex data. 
        // The values copied in are established by the API or through the use of the layout identifier location
        GLint location = glGetUniformLocation(program->program, query.name.c_str());
        if (location < 0 && query.required)
        {
            GL_LOG_WARN << "Required uniform query for " << query.name << " failed";
        }
        GL_LOG_TRACE << "-- program is: " << program->program << ", uniform name is: " << query.name
                     << ", location is: " << location;
        *query.dest = location;
    }
    // Run initializers
    for (size_t j=0; j<program->initialize.size(); j++)
    {
        auto& init = program->initialize[j];
        GLint uniform = *init.uniform;
        if (uniform != -1)
        {
            switch (init.type) 
            {
                case 0:
                    glUniform1i(uniform, init.value);
                break;
                default: assert(false); break;
            }
        }
    }
    CHECK_GL_ERROR_IF_DEBUG();
}

void OpenGL::InitStepCreateShader(Any data)
{
    assert(data.type() == typeid(OpenGLCreateShaderInitData));
    GL_LOG_TRACE << "[Init Step] Create Shader";
    const OpenGLCreateShaderInitData& createShaderData = RefAnyCast<OpenGLCreateShaderInitData>(data);
    CHECK_GL_ERROR_IF_DEBUG();
    GLuint shader = glCreateShader(createShaderData.stage);
    const char* _code = createShaderData.code.c_str();
    glShaderSource(shader, 1, &_code, nullptr);
    glCompileShader(shader);
    GL_LOG_TRACE << "Shader stage is:" << createShaderData.stage << ", code is: \n" << createShaderData.code;
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        std::string infoLog = GetInfoLog(shader, glGetShaderiv, glGetShaderInfoLog);
        std::stringstream ss;
        ss << "Error in shader compilation for: " << createShaderData.shader->desc << std::endl
            << "Info log is: " << std::endl << infoLog << std::endl
            << "Shader source :\n" << GlslCodeWithLineNumber(createShaderData.code) << std::endl << std::endl;
        GL_LOG_ERROR << ss.str();
        createShaderData.shader->failed = true;
        createShaderData.shader->error  = infoLog;
        assert(false);
    }
    createShaderData.shader->code  = createShaderData.code;
    createShaderData.shader->valid = true;
    createShaderData.shader->shader = shader;
}

void OpenGL::InitStepCreateInputLayout(Any /* data */)
{
    // Hint : do it really in BIND_VERTEX_BUFFER render commond
    // See also : RenderStepRender implemention
    GL_LOG_TRACE << "[Init Step] Input Layout";
}

/**
 * @sa es_spec_3.2.pdf - Chapter 9 Framebuffers and Framebuffer Objects
 */
void OpenGL::InitStepCreateFrameBuffer(Any data)
{
    assert(data.type() == typeid(OpenGLCreateFramebufferInitData));
    GL_LOG_TRACE << "[Init Step] Create Frame Buffer";
    const OpenGLCreateFramebufferInitData& createFrameBufferData = AnyCast<OpenGLCreateFramebufferInitData>(data);
    OpenGLRenderFrameBuffer::ptr fbo = createFrameBufferData.frameBuffer;
    CHECK_GL_ERROR_IF_DEBUG();

    if (!fbo->customColor &&_openGLFeature.ARB_framebuffer_object && _openGLFeature.EXT_framebuffer_object)
    {
        FBOExtCreate(data);
    }
    else if (!_openGLFeature.ARB_framebuffer_object && !_openGLFeature.IsGLES)
    {
        return;
    }

    auto initFBOTexture = [fbo, this](OpenGLRenderTexture::ptr tex, GLint internalFormat, GLenum format, GLenum type, bool linear) -> void
    {
        glGenTextures(1, &(tex->texture));
        tex->maxLod = 0.0f;

        // Create the surfaces.
        glBindTexture(GL_TEXTURE_2D, tex->texture);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, fbo->width, fbo->height, 0, format, type, nullptr);

        tex->wrapS      =  GL_CLAMP_TO_EDGE;
        tex->wrapT      =  GL_CLAMP_TO_EDGE;
        tex->magFilter  =  linear ? GL_LINEAR : GL_NEAREST;
        tex->minFilter  =  linear ? GL_LINEAR : GL_NEAREST;

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tex->wrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tex->wrapT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, tex->magFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, tex->minFilter);

        if (!_openGLFeature.IsGLES || _openGLFeature.GLES3)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
        }
    };

    // Color texture is same everywhere
    glGenFramebuffers(1, &fbo->handle);
    if (!fbo->customColor)
    {
        initFBOTexture(fbo->colorTextures[0], GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, true);
    }
    else
    {
        assert(fbo->width == fbo->colorTextures[0]->w);
        assert(fbo->height == fbo->colorTextures[0]->h);
        for (auto& colorTex : fbo->colorTextures)
        {
            glBindTexture(GL_TEXTURE_2D, colorTex->texture);
            GLenum internalFormat, format, type;
            int alignment;
            DataFormatToGLFormatAndType(colorTex->format, _openGLFeature, internalFormat, format, type, alignment);
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, colorTex->w, colorTex->h, 0, format, type, nullptr);
            CHECK_GL_ERROR_IF_DEBUG();
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            if (!_openGLFeature.IsGLES || _openGLFeature.GLES3)
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
            }
            CHECK_GL_ERROR_IF_DEBUG();
        }
    }
    
retry_depth:
    auto attachColorChn = [this](std::vector<OpenGLRenderTexture::ptr> colorTextures) -> void
    {
        // Hint : colorTextures 大小不为 1 时走 MRT 逻辑
        // WORKAROUND : 目前仍然无法解决多个 GL_COLOR_ATTACHMENTx 大小不一致问题, 强制多个 GL_COLOR_ATTACHMENT 之间大小一致，
        // TODO : 据说 GL_ARB_viewport_array 能够解决此问题,但是实际调试过程中仍然存在部分问题
        if (colorTextures.size() != 1)
        {
            GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7, GL_COLOR_ATTACHMENT8 };
            glDrawBuffers((GLsizei)colorTextures.size(), attachments);
            CHECK_GL_ERROR_IF_DEBUG();
            for(int i=0; i<(int)colorTextures.size(); i++)
            {
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorTextures[i]->texture, 0);
            }
            CHECK_GL_ERROR_IF_DEBUG();
        }
        else
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTextures[0]->texture, 0);
        }
    };

    if (!fbo->zStencil)
    {
        GL_LOG_TRACE << "Creating " << fbo->width << " x " << fbo->height << " FBO using no depth";

        fbo->zStencilBuffer  = 0;
        fbo->stencilBuffer   = 0;
        fbo->zBuffer         = 0;

        glBindFramebuffer(GL_FRAMEBUFFER, fbo->handle);
        attachColorChn(fbo->colorTextures);
        CHECK_GL_ERROR_IF_DEBUG();
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
        CHECK_GL_ERROR_IF_DEBUG();
    }
    else if (_openGLFeature.IsGLES)
    {
        if (_openGLFeature.OES_packed_depth_stencil && (_openGLFeature.OES_depth_texture || _openGLFeature.GLES3))
        {
            GL_LOG_INFO << "Creating " << fbo->width << " x " << fbo->height << " FBO using DEPTH24_STENCIL8";
            fbo->zStencilBuffer = 0;
            fbo->stencilBuffer   = 0;
            fbo->zBuffer         = 0;

            if (_openGLFeature.GLES3)
            {
                initFBOTexture(fbo->zStencilTexture, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, false);
            }
            else
            {
                initFBOTexture(fbo->zStencilTexture, GL_DEPTH_STENCIL, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, false);
            }
			glBindFramebuffer(GL_FRAMEBUFFER, fbo->handle);
            attachColorChn(fbo->colorTextures);
			if (_openGLFeature.GLES3) 
            {
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, fbo->zStencilTexture->texture, 0);
			} 
            else 
            {
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fbo->zStencilTexture->texture, 0);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, fbo->zStencilTexture->texture, 0);
			}
            CHECK_GL_ERROR_IF_DEBUG();
        }
        else if (_openGLFeature.OES_packed_depth_stencil)
        {
            GL_LOG_TRACE << "Creating " << fbo->width << " x " << fbo->height << " FBO using DEPTH24_STENCIL8";
			// Standard method
			fbo->stencilBuffer = 0;
			fbo->zBuffer = 0;
			// 24-bit Z, 8-bit stencil combined
			glGenRenderbuffers(1, &fbo->zStencilBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, fbo->zStencilBuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_OES, fbo->width, fbo->height);

			// Bind it all together
			glBindFramebuffer(GL_FRAMEBUFFER, fbo->handle);
            attachColorChn(fbo->colorTextures);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fbo->zStencilBuffer);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fbo->zStencilBuffer); 
        }
        else
        {
            GL_LOG_TRACE << "Creating " << fbo->width << " x " << fbo->height << " FBO using separate stencil";
			// TEGRA
			fbo->zStencilBuffer = 0;
			// 16/24-bit Z, separate 8-bit stencil
			glGenRenderbuffers(1, &fbo->zBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, fbo->zBuffer);
			// Don't forget to make sure fbo_standard_z_depth() matches.
			glRenderbufferStorage(GL_RENDERBUFFER, _openGLFeature.OES_depth24 ? GL_DEPTH_COMPONENT24 : GL_DEPTH_COMPONENT16, fbo->width, fbo->height);

			// 8-bit stencil buffer
			glGenRenderbuffers(1, &fbo->stencilBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, fbo->stencilBuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, fbo->width, fbo->height);

			// Bind it all together
			glBindFramebuffer(GL_FRAMEBUFFER, fbo->handle);
            attachColorChn(fbo->colorTextures);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fbo->zBuffer);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fbo->stencilBuffer);
        }
    }
    else if (OpenGLVersionGreater(_openGLFeature, 3, 0))
    {
        GL_LOG_TRACE << "Creating " << fbo->width << " x " << fbo->height << " FBO using DEPTH24_STENCIL8 texture";
		fbo->zStencilBuffer = 0;
		fbo->stencilBuffer = 0;
		fbo->zBuffer = 0;

        initFBOTexture(fbo->zStencilTexture, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, false);

		// Bind it all together
		glBindFramebuffer(GL_FRAMEBUFFER, fbo->handle);
        attachColorChn(fbo->colorTextures);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, fbo->zStencilTexture->texture, 0);
    }
    else
    {
        fbo->stencilBuffer = 0;
        fbo->zBuffer       = 0;
		// 24-bit Z, 8-bit stencil
		glGenRenderbuffers(1, &fbo->zStencilBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, fbo->zStencilBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, fbo->width, fbo->height);

		// Bind it all together
		glBindFramebuffer(GL_FRAMEBUFFER, fbo->handle);
        attachColorChn(fbo->colorTextures);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fbo->zStencilBuffer);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fbo->zStencilBuffer);
    }

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE && !fbo->zBuffer)
    {
        CHECK_GL_ERROR_IF_DEBUG();
        assert(!fbo->zStencil);
        fbo->zStencil = true;
        // Uh oh, maybe we need a z/stencil.  Platforms sometimes, right?
        goto retry_depth;
    }

    switch (status)
    {
        case GL_FRAMEBUFFER_COMPLETE:
            GL_LOG_TRACE << "GL_FRAMEBUFFER_COMPLETE";
        break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            GL_LOG_ERROR << "GL_FRAMEBUFFER_UNSUPPORTED";
        break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            GL_LOG_ERROR << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
        break;
        default:
            GL_LOG_ERROR << "Unknown frame buffer error, status is: " << status;
            assert(false);
            break;
    }

    // Unbind state we don't need
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
    CHECK_GL_ERROR_IF_DEBUG();

    _currentDrawHandle = fbo->handle;
    _currentReadHandle = fbo->handle;
}

void OpenGL::InitStepTextureImage(Any data, GLuint& boundTexture)
{
    // TODO : check new feature, see https://github.com/hrydgard/ppsspp/commit/b687f1bba867e609eef251b5a287acb134e4906c
    assert(data.type() == typeid(OpenGLTextureImageInitData));
    GL_LOG_TRACE << "[Init Step] Texture Image";
    const OpenGLTextureImageInitData& textureImageData = RefAnyCast<OpenGLTextureImageInitData>(data);
    CHECK_GL_ERROR_IF_DEBUG();
    OpenGLRenderTexture::ptr tex = textureImageData.texture;
    if (boundTexture != tex->texture)
    {
        glBindTexture(tex->target, tex->texture);
        boundTexture = tex->texture;
    }
    GLenum internalFormat, format, type;
    int alignment;
    DataFormatToGLFormatAndType(textureImageData.format, _openGLFeature, internalFormat, format, type, alignment);
    if (!(tex->flags & GlTextureFlags::TEXTURE_EXTERNAL))
    {
        if (textureImageData.depth == 1) // 2D
        {
            if (!tex->isMemoryAllocated)
            {
                glTexImage2D(tex->target, textureImageData.level, internalFormat, textureImageData.width, textureImageData.height,
                    0, format, type, textureImageData.data->GetData()
                );
                tex->isMemoryAllocated = true;
            }
            else
            {
                glTexSubImage2D(tex->target, textureImageData.level, 0, 0, textureImageData.width, textureImageData.height, 
                    format, type, textureImageData.data->GetData()
                );
            }
        }
        else // 3D
        {
            if (!tex->isMemoryAllocated)
            {
                glTexImage3D(tex->target, textureImageData.level, internalFormat, textureImageData.width, textureImageData.height,
                    textureImageData.depth, 0, format, type, textureImageData.data->GetData()
                );
                tex->isMemoryAllocated = true;
            }
            else
            {
                glTexSubImage3D(tex->target, textureImageData.level, 0, 0, 0, textureImageData.width, textureImageData.height, textureImageData.depth,
                    format, type, textureImageData.data->GetData()
                );
            }
        }
    }
#if MMP_PLATFORM(LINUX)
    else 
    {
        static PFNGLEGLIMAGETARGETTEXTURE2DOESPROC _glEGLImageTargetTexture2DOES = nullptr;
        AbstractDeviceAllocateMethod::ptr alloc = std::dynamic_pointer_cast<AbstractDeviceAllocateMethod>(textureImageData.data->GetAllocateMethod());
        AbstractWindows::ptr windows = GLDrawContex::Instance()->GetWindows();
        if (windows && !_glEGLImageTargetTexture2DOES)
        {
            _glEGLImageTargetTexture2DOES = (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)windows->GetProcAddress("glEGLImageTargetTexture2DOES");
        }
        if (alloc && _glEGLImageTargetTexture2DOES)
        {
            EGLImageKHR image = CreateEGLImage(tex, alloc);
            glBindTexture(tex->target, tex->texture);
            _glEGLImageTargetTexture2DOES(tex->target, image);
        }
        else
        {
            assert(false);
        }
    }
#endif

    tex->wrapS      = GL_CLAMP_TO_EDGE;
    tex->wrapT      = GL_CLAMP_TO_EDGE;
    tex->magFilter  = textureImageData.linearFilter ? GL_LINEAR : GL_NEAREST;
    tex->minFilter  = textureImageData.linearFilter ? GL_LINEAR : GL_NEAREST;
    CHECK_GL_ERROR_IF_DEBUG();
    glTexParameteri(tex->target, GL_TEXTURE_WRAP_S, tex->wrapS);
    glTexParameteri(tex->target, GL_TEXTURE_WRAP_T, tex->wrapT);
    glTexParameteri(tex->target, GL_TEXTURE_MAG_FILTER, tex->magFilter);
    glTexParameteri(tex->target, GL_TEXTURE_MIN_FILTER, tex->minFilter);
    if (textureImageData.depth > 1)
    {
        glTexParameteri(tex->target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }
    CHECK_GL_ERROR_IF_DEBUG();
}

void OpenGL::InitStepTextureFinalize(Any data, GLuint& boundTexture)
{
    assert(data.type() == typeid(OpenGLTextureFinalizeInitData));
    GL_LOG_TRACE << "[Init Step] Texture Finalize";
    const OpenGLTextureFinalizeInitData& textureFinalizeData = RefAnyCast<OpenGLTextureFinalizeInitData>(data);
    CHECK_GL_ERROR_IF_DEBUG();
    auto tex = textureFinalizeData.texture;
    if (boundTexture != tex->texture) 
    {
        glBindTexture(tex->target, tex->texture);
        boundTexture = tex->texture;
    }
    if ((!_openGLFeature.IsGLES || _openGLFeature.GLES3) && textureFinalizeData.loadedLevels > 1) 
    {
        glTexParameteri(tex->target, GL_TEXTURE_MAX_LEVEL, textureFinalizeData.loadedLevels - 1);
    }
    tex->maxLod = (float)textureFinalizeData.loadedLevels - 1;
    if (textureFinalizeData.genMips) 
    {
        glGenerateMipmap(tex->target);
    }
    CHECK_GL_ERROR_IF_DEBUG();
}

} // namespace Mmp