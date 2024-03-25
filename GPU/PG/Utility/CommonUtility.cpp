#include "CommonUtility.h"

#include <cassert>

#include "Common/SegmentAllocateMethod.h"

namespace Mmp
{
namespace Gpu
{

std::vector<Texture::ptr> Create2DTextures(GLDrawContex::ptr draw, const PixelsInfo& info, const std::string& tag, bool useForRender)
{
    std::vector<Texture::ptr> texs;
    if (info.format == PixelFormat::NV12)
    {
        {
            TextureDesc desc;
            desc.type          = TextureType::LINEAR2D;
            desc.format        = DataFormat::Y8_UNORM;
            desc.width         = info.width;
            desc.height        = info.height;
            desc.depth         = 1;
            desc.mipLevels     = 1;
            desc.generateMips  = false;
            desc.miscFlag      = useForRender ? GlTextureFlags::TEXTURE_USE_FOR_RENDER : GlTextureFlags::NONE;
            desc.tag           = tag;
            texs.push_back(draw->CreateTexture(desc));
        }
        {
            TextureDesc desc;
            desc.type          = TextureType::LINEAR2D;
            desc.format        = DataFormat::U8V8_UNORM;
            desc.width         = useForRender ? info.width : info.width / 2;
            desc.height        = useForRender ? info.height : info.height / 2;
            desc.depth         = 1;
            desc.mipLevels     = 1;
            desc.generateMips  = false;
            desc.miscFlag      = useForRender ? GlTextureFlags::TEXTURE_USE_FOR_RENDER : GlTextureFlags::NONE;
            desc.tag           = tag;
            texs.push_back(draw->CreateTexture(desc));
        }
    }
    else if (info.format == PixelFormat::RGBA8888
          || info.format == PixelFormat::ARGB8888
          || info.format == PixelFormat::BGRA8888
          || info.format == PixelFormat::ABGR8888
    )
    {
        TextureDesc desc;
        desc.type          = TextureType::LINEAR2D;
        desc.format        = DataFormat::R8G8B8A8_UNORM;
        desc.width         = info.width;
        desc.height        = info.height;
        desc.depth         = 1;
        desc.mipLevels     = 1;
        desc.generateMips  = false;
        desc.miscFlag      = useForRender ? GlTextureFlags::TEXTURE_USE_FOR_RENDER : GlTextureFlags::NONE;
        desc.tag           = tag;
        texs.push_back(draw->CreateTexture(desc));
    }
    else if (info.format == PixelFormat::RGB888 
          || info.format == PixelFormat::HSV888
          || info.format == PixelFormat::BGR888)
    {
        TextureDesc desc;
        desc.type          = TextureType::LINEAR2D;
        desc.format        = DataFormat::R8G8B8_UNORM;
        desc.width         = info.width;
        desc.height        = info.height;
        desc.depth         = 1;
        desc.mipLevels     = 1;
        desc.generateMips  = false;
        desc.miscFlag      = useForRender ? GlTextureFlags::TEXTURE_USE_FOR_RENDER : GlTextureFlags::NONE;
        desc.tag           = tag;
        texs.push_back(draw->CreateTexture(desc)); 
    }
    else if (info.format == PixelFormat::RGBA5551
          || info.format == PixelFormat::BRGA5551)
    {
        TextureDesc desc;
        desc.type          = TextureType::LINEAR2D;
        desc.format        = DataFormat::R5G5B5A1_UNORM_PACK16;
        desc.width         = info.width;
        desc.height        = info.height;
        desc.depth         = 1;
        desc.mipLevels     = 1;
        desc.generateMips  = false;
        desc.miscFlag      = useForRender ? GlTextureFlags::TEXTURE_USE_FOR_RENDER : GlTextureFlags::NONE;
        desc.tag           = tag;
        texs.push_back(draw->CreateTexture(desc)); 
    }
    else if (info.format == PixelFormat::RGB565
          || info.format == PixelFormat::BGR565)
    {
        TextureDesc desc;
        desc.type          = TextureType::LINEAR2D;
        desc.format        = DataFormat::R5G6B5_UNORM_PACK16;
        desc.width         = info.width;
        desc.height        = info.height;
        desc.depth         = 1;
        desc.mipLevels     = 1;
        desc.generateMips  = false;
        desc.miscFlag      = useForRender ? GlTextureFlags::TEXTURE_USE_FOR_RENDER : GlTextureFlags::NONE;
        desc.tag           = tag;
        texs.push_back(draw->CreateTexture(desc)); 
    }
    else if (info.format == PixelFormat::ARGB1555)
    {
        // Hint : not support
        assert(false);
    }
    else if (info.format == PixelFormat::YUV420P)
    {
        {
            TextureDesc desc;
            desc.type          = TextureType::LINEAR2D;
            desc.format        = DataFormat::Y8_UNORM;
            desc.width         = info.width;
            desc.height        = info.height;
            desc.depth         = 1;
            desc.mipLevels     = 1;
            desc.generateMips  = false;
            desc.miscFlag      = useForRender ? GlTextureFlags::TEXTURE_USE_FOR_RENDER : GlTextureFlags::NONE;
            desc.tag           = tag;
            texs.push_back(draw->CreateTexture(desc));
        }
        {
            TextureDesc desc;
            desc.type          = TextureType::LINEAR2D;
            desc.format        = DataFormat::U8_UNORM;
            desc.width         = useForRender ? info.width : info.width / 2;
            desc.height        = useForRender ? info.height : info.height / 2;
            desc.depth         = 1;
            desc.mipLevels     = 1;
            desc.generateMips  = false;
            desc.miscFlag      = useForRender ? GlTextureFlags::TEXTURE_USE_FOR_RENDER : GlTextureFlags::NONE;
            desc.tag           = tag;
            texs.push_back(draw->CreateTexture(desc));
        }
        {
            TextureDesc desc;
            desc.type          = TextureType::LINEAR2D;
            desc.format        = DataFormat::V8_UNORM;
            desc.width         = useForRender ? info.width : info.width / 2;
            desc.height        = useForRender ? info.height : info.height / 2;
            desc.depth         = 1;
            desc.mipLevels     = 1;
            desc.generateMips  = false;
            desc.miscFlag      = useForRender ? GlTextureFlags::TEXTURE_USE_FOR_RENDER : GlTextureFlags::NONE;
            desc.tag           = tag;
            texs.push_back(draw->CreateTexture(desc));
        }
    }
    else
    {
        assert(false);
    }
    return texs;
}

void Update2DTextures(GLDrawContex::ptr draw, std::vector<Texture::ptr> texs, AbstractPicture::ptr frame)
{
    Mmp::PixelsInfo info = frame->info;
    if ( (info.format == PixelFormat::RGBA8888
       || info.format == PixelFormat::ARGB8888
       || info.format == PixelFormat::BGRA8888
       || info.format == PixelFormat::ABGR8888
       ) && texs.size() == 1
    )
    {
        TextureDesc desc;
        desc.type          = TextureType::LINEAR2D;
        desc.format        = DataFormat::B8G8R8A8_UNORM;
        desc.width         = info.width;
        desc.height        = info.height;
        desc.depth         = 1;
        desc.mipLevels     = 1;
        desc.generateMips  = false;

        GLPixelData::ptr pixels = std::make_shared<GLPixelData>(desc.width, desc.height, desc.depth, desc.format, frame->GetAllocateMethod());
        desc.initData.push_back(pixels);
        draw->UpdateTexture(texs[0], desc);
    }
    else if ((info.format == PixelFormat::RGB888 
          || info.format == PixelFormat::HSV888
          || info.format == PixelFormat::BGR888)
          && texs.size() == 1)
    {
        TextureDesc desc;
        desc.type          = TextureType::LINEAR2D;
        desc.format        = DataFormat::R8G8B8_UNORM;
        desc.width         = info.width;
        desc.height        = info.height;
        desc.depth         = 1;
        desc.mipLevels     = 1;
        desc.generateMips  = false;

        assert(texs.size() == 1);
        GLPixelData::ptr pixels = std::make_shared<GLPixelData>(desc.width, desc.height, desc.depth, desc.format, frame->GetAllocateMethod());
        desc.initData.push_back(pixels);
        draw->UpdateTexture(texs[0], desc);
    }
    else if (info.format == PixelFormat::NV12 && texs.size() == 2)
    {
        int offset = 0;
        // Update L Texture
        {
            TextureDesc desc;
            desc.type          = TextureType::LINEAR2D;
            desc.format        = DataFormat::Y8_UNORM;
            desc.width         = info.width;
            desc.height        = info.height;
            desc.depth         = 1;
            desc.mipLevels     = 1;
            desc.generateMips  = false;
            SegmentAllocateMethod::ptr segmentAlloc = std::make_shared<SegmentAllocateMethod>();
            segmentAlloc->SetRange(offset, desc.width*desc.height, frame);
            offset += info.width*info.height;
            GLPixelData::ptr pixels = std::make_shared<GLPixelData>(desc.width, desc.height, desc.depth, desc.format, segmentAlloc);
            desc.initData.push_back(pixels);
            draw->UpdateTexture(texs[0], desc);
        }
        // Update UV Texture
        {
            TextureDesc desc;
            desc.type          = TextureType::LINEAR2D;
            desc.format        = DataFormat::U8V8_UNORM;
            desc.width         = info.width / 2;
            desc.height        = info.height / 2;
            desc.depth         = 1;
            desc.mipLevels     = 1;
            desc.generateMips  = false;
            SegmentAllocateMethod::ptr segmentAlloc = std::make_shared<SegmentAllocateMethod>();
            segmentAlloc->SetRange(offset, desc.width*desc.height*2, frame);
            GLPixelData::ptr pixels = std::make_shared<GLPixelData>(desc.width, desc.height, desc.depth, desc.format, segmentAlloc);
            desc.initData.push_back(pixels);
            draw->UpdateTexture(texs[1], desc);
        }
    }
    else if ((info.format == PixelFormat::RGBA5551 
           || info.format == PixelFormat::BRGA5551  
             ) && texs.size() == 1)
    {
        TextureDesc desc;
        desc.type          = TextureType::LINEAR2D;
        desc.format        = DataFormat::R5G5B5A1_UNORM_PACK16;
        desc.width         = info.width;
        desc.height        = info.height;
        desc.depth         = 1;
        desc.mipLevels     = 1;
        desc.generateMips  = false;

        assert(texs.size() == 1);
        GLPixelData::ptr pixels = std::make_shared<GLPixelData>(desc.width, desc.height, desc.depth, desc.format, frame->GetAllocateMethod());
        desc.initData.push_back(pixels);
        draw->UpdateTexture(texs[0], desc);
    }
    else if ((info.format == PixelFormat::RGB565 
          ||  info.format == PixelFormat::BGR565
            ) && texs.size() == 1)
    {
        TextureDesc desc;
        desc.type          = TextureType::LINEAR2D;
        desc.format        = DataFormat::R5G6B5_UNORM_PACK16;
        desc.width         = info.width;
        desc.height        = info.height;
        desc.depth         = 1;
        desc.mipLevels     = 1;
        desc.generateMips  = false;

        assert(texs.size() == 1);
        GLPixelData::ptr pixels = std::make_shared<GLPixelData>(desc.width, desc.height, desc.depth, desc.format, frame->GetAllocateMethod());
        desc.initData.push_back(pixels);
        draw->UpdateTexture(texs[0], desc);
    }
    else if (info.format == PixelFormat::ARGB1555 && texs.size() == 1)
    {
        // Hint : not support
        assert(false);
    }
    else if (info.format == PixelFormat::YUV420P)
    {
        int offset = 0;
        // Update L Texture
        {
            TextureDesc desc;
            desc.type          = TextureType::LINEAR2D;
            desc.format        = DataFormat::Y8_UNORM;
            desc.width         = info.width;
            desc.height        = info.height;
            desc.depth         = 1;
            desc.mipLevels     = 1;
            desc.generateMips  = false;
            SegmentAllocateMethod::ptr segmentAlloc = std::make_shared<SegmentAllocateMethod>();
            segmentAlloc->SetRange(offset, desc.width*desc.height, frame);
            offset += info.width*info.height;
            GLPixelData::ptr pixels = std::make_shared<GLPixelData>(desc.width, desc.height, desc.depth, desc.format, segmentAlloc);
            desc.initData.push_back(pixels);
            draw->UpdateTexture(texs[0], desc);
        }
        // Update U Texture
        {
            TextureDesc desc;
            desc.type          = TextureType::LINEAR2D;
            desc.format        = DataFormat::U8_UNORM;
            desc.width         = info.width / 2;
            desc.height        = info.height / 2;
            desc.depth         = 1;
            desc.mipLevels     = 1;
            desc.generateMips  = false;
            SegmentAllocateMethod::ptr segmentAlloc = std::make_shared<SegmentAllocateMethod>();
            segmentAlloc->SetRange(offset, desc.width*desc.height, frame);
            GLPixelData::ptr pixels = std::make_shared<GLPixelData>(desc.width, desc.height, desc.depth, desc.format, segmentAlloc);
            desc.initData.push_back(pixels);
            draw->UpdateTexture(texs[1], desc);
            offset += info.width*info.height / 4;
        } 
        // Update V Texture
        {
            TextureDesc desc;
            desc.type          = TextureType::LINEAR2D;
            desc.format        = DataFormat::V8_UNORM;
            desc.width         = info.width / 2;
            desc.height        = info.height / 2;
            desc.depth         = 1;
            desc.mipLevels     = 1;
            desc.generateMips  = false;
            SegmentAllocateMethod::ptr segmentAlloc = std::make_shared<SegmentAllocateMethod>();
            segmentAlloc->SetRange(offset, desc.width*desc.height, frame);
            GLPixelData::ptr pixels = std::make_shared<GLPixelData>(desc.width, desc.height, desc.depth, desc.format, segmentAlloc);
            desc.initData.push_back(pixels);
            draw->UpdateTexture(texs[2], desc);
        } 
    }
    else
    {
        assert(false);
    }
}

bool Copy2DTexturesToMemory(GLDrawContex::ptr draw, std::vector<Texture::ptr> texs, AbstractPicture::ptr frame)
{
    PixelsInfo info = frame->info;
    if (( info.format == PixelFormat::RGBA8888
       || info.format == PixelFormat::RGB888
       || info.format == PixelFormat::HSV888
       || info.format == PixelFormat::BGR888
       || info.format == PixelFormat::ARGB8888
       || info.format == PixelFormat::RGB565
       || info.format == PixelFormat::RGBA5551
       || info.format == PixelFormat::BGRA8888
       || info.format == PixelFormat::BGR565
       || info.format == PixelFormat::ABGR8888
       || info.format == PixelFormat::BRGA5551)
     && texs.size() == 1
    )
    {
        std::vector<GLRect2D> rects;
        rects.push_back({0, 0, texs[0]->Width(), texs[0]->Height()});
        return draw->CopyTextureToMemory(texs, rects, frame);
    }
    else if (info.format == PixelFormat::NV12 && texs.size() == 2)
    {
        std::vector<GLRect2D> rects;
        rects.push_back({0, 0, texs[0]->Width(), texs[0]->Height()});
        rects.push_back({0, 0, texs[0]->Width()/2, texs[0]->Height()/2});
        return draw->CopyTextureToMemory(texs, rects, frame);
    }
    else if (info.format == PixelFormat::ARGB1555 && texs.size() == 1)
    {
        assert(false);
    }
    else if (info.format == PixelFormat::YUV420P && texs.size() == 3)
    {
        std::vector<GLRect2D> rects;
        rects.push_back({0, 0, texs[0]->Width(), texs[0]->Height()});
        rects.push_back({0, 0, texs[0]->Width()/2, texs[0]->Height()/2});
        rects.push_back({0, 0, texs[0]->Width()/2, texs[0]->Height()/2});
        return draw->CopyTextureToMemory(texs, rects, frame);
    }
    else
    {
        assert(false);
    }
    return false;
}

} // namespace Gpu
} // namespace Mmp