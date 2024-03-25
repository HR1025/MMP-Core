//
// OpenGLRenderContex.h
//
// Library: GPU
// Package: GL
// Module:  OpenGL
//

#pragma once

#include "OpenGLCommon.h"
#include "OpenGL.h"

#include <unordered_map>

namespace Mmp
{

class OpenGLRenderTexture
{
public:
    using ptr = std::shared_ptr<OpenGLRenderTexture>;
public:
    OpenGLRenderTexture(int width, int height, int depth, int numMips);
    ~OpenGLRenderTexture();
public:
    GLuint texture;
    uint16_t w;
    uint16_t h;
    uint16_t d;

    GlTextureFlags flags;
    DataFormat format;
    GLenum target;
    GLenum wrapS;
    GLenum wrapT;
    GLenum magFilter;
    GLenum minFilter;
    uint8_t numMips;
    bool canWrap;
    float anisotropy;
    float minLod;
    float maxLod;
    float lodBias;
public:
    bool  isMemoryAllocated;
};

class OpenGLRenderFrameBuffer
{
public:
    using ptr = std::shared_ptr<OpenGLRenderFrameBuffer>;
public:
    OpenGLRenderFrameBuffer(int width, int height, bool zStencil);
    ~OpenGLRenderFrameBuffer();
public:
    /**
     * @brief      为 frame buffer 设置自定义 color texture
     * @param[in]  textures
     */
    void SetColorTextures(const std::vector<OpenGLRenderTexture::ptr>& textures);
public:
    GLuint handle;
    GLuint                   zStencilBuffer;
    OpenGLRenderTexture::ptr zStencilTexture;
    GLuint zBuffer;
    GLuint stencilBuffer;

    int width;
    int height;
    GLuint colorDepth;

    bool zStencil;
public:
    bool                                   customColor;
    std::vector<OpenGLRenderTexture::ptr>  colorTextures;
};

class OpenGLRenderShader
{
public:
    using ptr = std::shared_ptr<OpenGLRenderShader>;
public:
    OpenGLRenderShader();
    ~OpenGLRenderShader();
public:
    GLuint shader;
    bool valid;

    bool failed;
    std::string desc;
    std::string code;
    std::string error;
};

struct OpenGLRenderProgramFlags
{
    bool useClipDistance0 : 1;
    bool useClipDistance1 : 1;
    bool useClipDistance2 : 1;
};

class OpenGLRenderProgramLocData
{
public:
    using ptr = std::shared_ptr<OpenGLRenderProgramLocData>;
public:
    virtual ~OpenGLRenderProgramLocData() = default;
};

class OpenGLRenderProgram
{
public:
    using ptr = std::shared_ptr<OpenGLRenderProgram>;
public:
    using OnDelete = std::function<void(void* /* opaque */)>;
public:
    struct Semantic
    {
        int          location;
        std::string  attrib;
    };
    struct UniformLocQuery
    {
        GLint*       dest;
        std::string  name;
        bool         required;
    };
    struct Initializer
    {
        GLint* uniform;
        int    type;
        int    value;
    };
    struct UniformInfo
    {
        int loc;
    };
public:
    OpenGLRenderProgram();
    ~OpenGLRenderProgram();
public:
    int GetUniformLoc(const std::string& name);
    void SetDeleteCallback(const OnDelete& onDelete, void* opaque);
public:
    GLuint program;
    std::vector<Semantic> semantics;
    std::vector<UniformLocQuery> queries;
    std::vector<Initializer> initialize;

    OpenGLRenderProgramLocData::ptr locData;
    bool use_clip_distance[8] {};
private:
    std::unordered_map<std::string, UniformInfo> uniformCache;
    OnDelete _onDelete;
    void*    _opaque;
};

class OpenGLRenderBuffer
{
public:
    using ptr = std::shared_ptr<OpenGLRenderBuffer>;
public:
    OpenGLRenderBuffer(GLuint target, size_t size);
    ~OpenGLRenderBuffer();
public:
    void* Map(OpenGLBufferStrategy strategy);
    bool  Unmap();
    bool Mapped();

    GLuint buffer;
    GLuint target;
    int size;
private:
    bool _mapped;
    bool _hasStorage;
};

class OpenGLRenderInputLayout
{
public:
    using ptr = std::shared_ptr<OpenGLRenderInputLayout>;
public:
    struct Entry
    {
        std::string attribute;
        int location;
        int count;
        GLenum type;
        GLboolean normalized;
        int stride;
        intptr_t offset;
    };
public:
    std::vector<Entry> entries;
    int semanticsMask = 0;
};

} // namespace Mmp
