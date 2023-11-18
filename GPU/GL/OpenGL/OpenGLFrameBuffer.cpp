#include "OpenGL.h"

#include "OpenGLUtil.h"
#include "OpenGLContex.h"
#include "OpenGLStepData.h"
#include "OpenGLInitStepData.h"
#include "OpenGLRenderData.h"

namespace Mmp
{

constexpr GLuint kDeafultFBO = 0;

GLenum OpenGL::FBOGetFBTarget(bool read /* !read -> write */, GLuint** cached)
{
    bool supportsBlit = _openGLFeature.ARB_framebuffer_object;
    if (_openGLFeature.IsGLES)
    {
        supportsBlit = _openGLFeature.GLES3 || _openGLFeature.NV_framebuffer_blit;
    }
    // Note: GL_FRAMEBUFFER_EXT and GL_FRAMEBUFFER have the same value, same with _NV.
    if (supportsBlit)
    {
        if (read)
        {
            *cached = &_currentReadHandle;
            return GL_READ_FRAMEBUFFER;
        }
        else
        {
            *cached = &_currentDrawHandle;
            return GL_DRAW_FRAMEBUFFER;
        }
    }
    else
    {
        *cached = &_currentDrawHandle;
        return GL_FRAMEBUFFER;
    }
}

void OpenGL::FBOExtCreate(Any data)
{
    assert(data.type() == typeid(OpenGLCreateFramebufferInitData));
    const OpenGLCreateFramebufferInitData& createFrameBufferData = RefAnyCast<OpenGLCreateFramebufferInitData>(data);
    OpenGLRenderFrameBuffer::ptr fbo = createFrameBufferData.frameBuffer;
    
    CHECK_GL_ERROR_IF_DEBUG(); 

    // Color texture is same everywhere
    glGenFramebuffersEXT(1, &fbo->handle);
    glGenTextures(1, &fbo->colorTextures[0]->texture);

    // Create the surfaces.
    glBindTexture(GL_TEXTURE_2D, fbo->colorTextures[0]->texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fbo->width, fbo->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    fbo->colorTextures[0]->target     = GL_TEXTURE_2D;
    fbo->colorTextures[0]->wrapS      = GL_CLAMP_TO_EDGE;
    fbo->colorTextures[0]->wrapT      = GL_CLAMP_TO_EDGE;
    fbo->colorTextures[0]->magFilter  = GL_LINEAR;
    fbo->colorTextures[0]->minFilter  = GL_LINEAR;
    fbo->colorTextures[0]->maxLod     = 0.0f;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, fbo->colorTextures[0]->wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, fbo->colorTextures[0]->wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, fbo->colorTextures[0]->magFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, fbo->colorTextures[0]->minFilter);

    fbo->stencilBuffer = 0;
    fbo->zBuffer       = 0;
    // 24-bit Z, 8-bit stencil
    glGenRenderbuffersEXT(1, &fbo->zStencilBuffer);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, fbo->zStencilBuffer);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_STENCIL_EXT, fbo->width, fbo->height);

	// Bind it all together
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo->handle);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, fbo->colorTextures[0]->texture, 0);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, fbo->zStencilBuffer);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, fbo->zStencilBuffer);

    GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    switch (status)
    {
        case GL_FRAMEBUFFER_COMPLETE_EXT:
            GL_LOG_TRACE << "GL_FRAMEBUFFER_COMPLETE";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
            GL_LOG_ERROR << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
            break;
        default:
            GL_LOG_ERROR << "Other framebuffer error, stauts code is: " << status;
            assert(false);
            break;
    }
	// Unbind state we don't need
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
    
    CHECK_GL_ERROR_IF_DEBUG();

    _currentDrawHandle = fbo->handle;
    _currentReadHandle = fbo->handle;
}

void OpenGL::FBOBindFBTarget(bool read /* !read -> write */, GLuint name)
{
    GLuint* cache = nullptr;
    GLenum taget = FBOGetFBTarget(read, &cache);
    if (cache && *cache != name)
    {
        if (_openGLFeature.ARB_framebuffer_object || _openGLFeature.IsGLES)
        {
            glBindFramebuffer(taget, name);
            CHECK_GL_ERROR_IF_DEBUG();
        }
        *cache = name;
    }
}

void OpenGL::FBOUnbind()
{
    CHECK_GL_ERROR_IF_DEBUG();
    glBindFramebuffer(GL_FRAMEBUFFER, kDeafultFBO);
    _currentDrawHandle = 0;
    _currentReadHandle  = 0;
    CHECK_GL_ERROR_IF_DEBUG();
}

void OpenGL::PerformBindFramebufferAsRenderTarget(const OpenGLRenderStep& pass)
{
    assert(pass.data.type() == typeid(OpenGLRenderData));
    const OpenGLRenderData& renderData = RefAnyCast<OpenGLRenderData>(pass.data);
    if (renderData.framebuffer)
    {
        _currentFrameBufferWidth  = renderData.framebuffer->width;
        _currentFrameBufferHeight = renderData.framebuffer->height;
    }
    else
    {
        _currentFrameBufferWidth  = _targetWidth;
        _currentFrameBufferHeight = _targeHeight;
    }

    if (renderData.framebuffer)
    {
        FBOBindFBTarget(false, renderData.framebuffer->handle);
    }
    else if (_currentFrameBuffer /* && !enderData.framebuffer */)
    {
        FBOUnbind();
    }

    _currentFrameBuffer = renderData.framebuffer;

    CHECK_GL_ERROR_IF_DEBUG();
}

} // namespace Mmp