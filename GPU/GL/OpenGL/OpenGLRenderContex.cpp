#include "OpenGLRenderContex.h"

#include "OpenGLUtil.h"

#include <cassert>

namespace Mmp
{

static bool IsTextureNPOTFullySupported(const OpenGLFeature& openGLFeature)
{
    if ((!openGLFeature.IsGLES && OpenGLVersionGreater(openGLFeature, 2, 0))
        || openGLFeature.GLES3 || openGLFeature.ARB_texture_non_power_of_two
        || openGLFeature.OES_texture_npot
    )
    {
        return true;
    }
    else
    {
        return false;
    }
}

static bool IsPowerOfTwo(int n)
{
    return n == 1 || (n & (n - 1)) == 0;
}

/************************************ OpenGLRenderTexture(BEGIN) **************************************/
OpenGLRenderTexture::OpenGLRenderTexture(int width, int height, int depth, int numMips)
{
    const OpenGLFeature& openGLFeature = GetOpenGLFeature();
    if (IsTextureNPOTFullySupported(openGLFeature))
    {
        canWrap = true;
    }
    else
    {
        canWrap = IsPowerOfTwo(width) && IsPowerOfTwo(height);
    }

    texture = 0;
    w = width;
    h = height;
    d = depth;
    this->numMips = numMips;

	target = 0xFFFF;
	wrapS = 0xFFFF;
	wrapT = 0xFFFF;
	magFilter = 0xFFFF;
	minFilter = 0xFFFF;
	anisotropy = -100000.0f;
	minLod = -1000.0f;
	maxLod = 1000.0f;
	lodBias = 0.0f;

    isMemoryAllocated = false;
}

OpenGLRenderTexture::~OpenGLRenderTexture()
{
    if (texture)
    {
        glDeleteTextures(1, &texture);
    }
}
/************************************ OpenGLRenderTexture(END) **************************************/

/************************************ OpenGLFrameBuffer(BEGIN) **************************************/
OpenGLRenderFrameBuffer::OpenGLRenderFrameBuffer(int width, int height, bool zStencil)
{
    colorTextures.push_back(std::make_shared<OpenGLRenderTexture>(width, height, 1, 1));
    zStencilTexture = std::make_shared<OpenGLRenderTexture>(width, height, 1, 1);

    this->width     = width;
    this->height    = height;
    this->zStencil  = zStencil;

	handle         = 0;
	zStencilBuffer = 0;
	zBuffer        = 0;
	stencilBuffer  = 0;
	colorDepth     = 0;
    customColor    = false;
}

OpenGLRenderFrameBuffer::~OpenGLRenderFrameBuffer()
{
    if (handle == 0 && zStencilBuffer == 0 && zBuffer == 0 && stencilBuffer == 0)
        return;

    CHECK_GL_ERROR_IF_DEBUG();
    const OpenGLFeature& openGLFeature = GetOpenGLFeature();
    
    if (handle)
    {
        if (openGLFeature.ARB_framebuffer_object || openGLFeature.IsGLES)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, handle);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDeleteFramebuffers(1, &handle);
        }
        else if (openGLFeature.EXT_framebuffer_object)
        {
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, handle);
            glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
            glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER_EXT, 0);
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
            glDeleteFramebuffersEXT(1, &handle);
        }
    }
    
    if (zStencilBuffer)
    {
        glDeleteRenderbuffers(1, &zStencilBuffer);
    }
    if (zBuffer)
    {
        glDeleteRenderbuffers(1, &zBuffer);
    }
    if (stencilBuffer)
    {
        glDeleteRenderbuffers(1, &stencilBuffer);
    }
    CHECK_GL_ERROR_IF_DEBUG();
}

void OpenGLRenderFrameBuffer::SetColorTextures(const std::vector<OpenGLRenderTexture::ptr>& textures)
{
    customColor = true;
    colorTextures = textures;
}
/************************************ OpenGLFrameBuffer(END) **************************************/

/************************************ OpenGLRenderShader(BEGIN) **************************************/
OpenGLRenderShader::OpenGLRenderShader()
{
	shader = 0;
	valid = false;
	failed = false;
}

OpenGLRenderShader::~OpenGLRenderShader()
{
    if (shader)
    {
        glDeleteShader(shader);
    }
}
/************************************ OpenGLRenderShader(END) **************************************/

/************************************ OpenGLRenderProgram(BEGIN) **************************************/
OpenGLRenderProgram::OpenGLRenderProgram()
{
    program = 0;
    _opaque = nullptr;
    _onDelete = [](void* /* opaque */) -> void
    {

    };
    locData = std::make_shared<OpenGLRenderProgramLocData>();
}

OpenGLRenderProgram::~OpenGLRenderProgram()
{
    _onDelete(_opaque);
    if (program)
    {
        glDeleteProgram(program);
    }
}

int OpenGLRenderProgram::GetUniformLoc(const std::string& name)
{
    if (uniformCache.count(name) == 1)
    {
        return uniformCache[name].loc;
    }
    else
    {
        // es_spec_3.2.pdf - 7.6 Uniform Variables
        int loc = glGetUniformLocation(program, reinterpret_cast<const GLchar *>(name.c_str()));
		UniformInfo info;
        info.loc = loc;
        uniformCache[name] = info;
        return loc;
    }
    assert(false);
    return -1;
}

void OpenGLRenderProgram::SetDeleteCallback(const OnDelete& onDelete, void* opaque)
{
    _onDelete = onDelete;
    _opaque   = opaque;
}
/************************************ OpenGLRenderProgram(END) **************************************/

/************************************ OpenGLRenderBuffer(BEGIN) **************************************/
OpenGLRenderBuffer::OpenGLRenderBuffer(GLuint target, size_t size)
{
    this->target      = target;
    this->size        = (int)size;
    buffer = 0;
    _mapped     = false;
    _hasStorage = false;
}

OpenGLRenderBuffer::~OpenGLRenderBuffer()
{
    if (buffer)
    {
        glDeleteBuffers(1, &buffer);
    }
}

void* OpenGLRenderBuffer::Map(OpenGLBufferStrategy strategy)
{
    assert(buffer != 0);
    const OpenGLFeature& openGLFeature = GetOpenGLFeature();

    GLbitfield access = GL_MAP_WRITE_BIT;
    if ((strategy & OpenGLBufferStrategy::MASK_FLUSH) != 0)
    {
        access |= GL_MAP_FLUSH_EXPLICIT_BIT;
    }
    if ((strategy & OpenGLBufferStrategy::MASK_INVALIDATE) != 0)
    {
        access |= GL_MAP_INVALIDATE_BUFFER_BIT;
    }
    void *p = nullptr;
    bool allowNativeBuffer = strategy != OpenGLBufferStrategy::SUBDATA;
    if (allowNativeBuffer)
    {
        glBindBuffer(target, buffer);
        if (openGLFeature.ARB_buffer_storage || openGLFeature.EXT_buffer_storage)
        {
            if (!_hasStorage)
            {
                GLbitfield storageFlags = access & ~(GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_FLUSH_EXPLICIT_BIT);
                glBufferStorage(target, size, nullptr, storageFlags);
                _hasStorage = true;
            }
            p = glMapBufferRange(target, 0, size, access);
        }
        else if (OpenGLVersionGreater(openGLFeature, 3, 0))
        {
            p = glMapBufferRange(target, 0, size, access);
        }
        else if (!openGLFeature.IsGLES)
        {
            p = glMapBuffer(target, GL_READ_WRITE);
        }        
    }
    _mapped = p != nullptr;
    return p;
}

bool  OpenGLRenderBuffer::Unmap()
{
    glBindBuffer(target, buffer);
    _mapped = false;
    return glUnmapBuffer(target) == GL_TRUE;
}

bool OpenGLRenderBuffer::Mapped()
{
    return _mapped;
}
/************************************ OpenGLRenderBuffer(END) **************************************/

} // namespace Mmp