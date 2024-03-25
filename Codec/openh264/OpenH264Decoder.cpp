#include "OpenH264Decoder.h"

#include <cassert>
#include <memory.h>

#include "OpenH264Util.h"

namespace Mmp
{
namespace Codec
{

OpenH264Decoder::OpenH264Decoder()
{
    _decoder = nullptr;
    _bufSize = 5;
}

void OpenH264Decoder::SetParameter(Any parameter)
{
}

Any OpenH264Decoder::GetParamter()
{
    return Any();
}

bool OpenH264Decoder::Init()
{
    std::lock_guard<std::mutex> locK(_mtx);
    assert(!_decoder);
    if (WelsCreateDecoder(&_decoder) != 0)
    {
        OPENH264_LOG_ERROR << "WelsCreateDecoder fail";
        assert(false);
        goto END;
    }
    {
        int logLevel = WELS_LOG_WARNING;
        WelsTraceCallback logCallback = OpenH264LogCallBack;
        _decoder->SetOption(DECODER_OPTION_TRACE_LEVEL, &logLevel);
        _decoder->SetOption(DECODER_OPTION_TRACE_CALLBACK, (void *)&logCallback);
    }
    {
        SDecodingParam param = {};
        param.eEcActiveIdc = ERROR_CON_DISABLE;
        param.sVideoProperty.eVideoBsType = VIDEO_BITSTREAM_AVC;
        if (_decoder->Initialize(&param) != cmResultSuccess)
        {
            OPENH264_LOG_ERROR << "CWelsDecoder::Initialize fail";
            assert(false);
            goto END1;
        }
    }
    return true;
END1:
    WelsDestroyDecoder(_decoder);
    _decoder = nullptr;
END:
    return false;
}

void OpenH264Decoder::Uninit()
{
    std::lock_guard<std::mutex> lock(_mtx);
    assert(_decoder);
    WelsDestroyDecoder(_decoder);
    _decoder = nullptr;
}

bool OpenH264Decoder::Start()
{
    std::lock_guard<std::mutex> lock(_mtx);
    return true;
}

void OpenH264Decoder::Stop()
{
    std::lock_guard<std::mutex> lock(_mtx);
}

bool OpenH264Decoder::Push(AbstractPack::ptr pack)
{
    std::lock_guard<std::mutex> lock(_mtx);
    uint8_t *palnes[4] = {0};
    SBufferInfo info = {0};
    {
        DECODING_STATE state = DECODING_STATE::dsErrorFree;
        SFrameBSInfo bsInfo = {};
        this->_decoder->DecodeFrameNoDelay(reinterpret_cast<const unsigned char*>(pack->GetData()), pack->GetSize(), palnes, &info);
        if (state != DECODING_STATE::dsErrorFree)
        {
            OPENH264_LOG_WARN << "ISVCDecoder::DecodeFrameNoDelay fail";
            assert(false);
            return false;
        }
        if (info.iBufferStatus != 1)
        {
            return true;
        }
    }

    if (info.UsrData.sSystemBuffer.iWidth != this->_info.width 
        || info.UsrData.sSystemBuffer.iHeight != this->_info.height
        || OpenH264PixFormatToMppPixFormat((EVideoFormatType)info.UsrData.sSystemBuffer.iFormat) != this->_info.format)
    {
        OPENH264_LOG_INFO << "Pixels info change, width(" << info.UsrData.sSystemBuffer.iWidth 
                            << ") height(" << info.UsrData.sSystemBuffer.iHeight 
                            << ") Format(" << OpenH264PixFormatToMppPixFormat((EVideoFormatType)info.UsrData.sSystemBuffer.iFormat) << ")";
        this->_pool.reset();
        this->_info.width  = info.UsrData.sSystemBuffer.iWidth;
        this->_info.height = info.UsrData.sSystemBuffer.iHeight;
        this->_info.format = OpenH264PixFormatToMppPixFormat((EVideoFormatType)info.UsrData.sSystemBuffer.iFormat);
        this->_pool = std::make_shared<SharedDataPool>((size_t)(this->_info.width * this->_info.height * BytePerPixel(this->_info.format)), this->_bufSize);
    }
    {
        AbstractSharedData::ptr frame = this->_pool->Request();
        // Y
        for (uint32_t i=0; i<_info.height; i++)
        {
            uint8_t* src = palnes[0] + (info.UsrData.sSystemBuffer.iStride[0] * i);
            uint8_t* dst = (uint8_t*)frame->GetData(_info.width*i);
            uint32_t align = std::min(_info.width, info.UsrData.sSystemBuffer.iStride[0]);
            memcpy(dst, src, align);
        }
        // U
        for (uint32_t i=0; i<_info.height/2; i++)
        {
            uint8_t* src = palnes[1] + (info.UsrData.sSystemBuffer.iStride[1] * i);
            uint8_t* dst = (uint8_t*)frame->GetData(_info.width*_info.height + i * _info.width / 2);
            uint32_t align = std::min(_info.width/2, info.UsrData.sSystemBuffer.iStride[1]);
            memcpy(dst, src, align);
        }
        // V
        for (uint32_t i=0; i<_info.height/2; i++)
        {
            uint8_t* src = palnes[2] + (info.UsrData.sSystemBuffer.iStride[1] * i);
            uint8_t* dst = (uint8_t*)frame->GetData(_info.width*_info.height*5/4 + i * _info.width / 2);
            uint32_t align = std::min(_info.width/2, info.UsrData.sSystemBuffer.iStride[1]);
            memcpy(dst, src, align);   
        }
        std::lock_guard<std::mutex> bufLock(_bufMtx);
        StreamFrame::ptr streamFrame = std::make_shared<StreamFrame>(_info, frame);
        streamFrame->pts = std::chrono::milliseconds(info.uiOutYuvTimeStamp);
        _buffers.push_back(streamFrame);
    }
    return true;
}

bool OpenH264Decoder::Pop(AbstractFrame::ptr& frame) 
{
    std::lock_guard<std::mutex> lock(_bufMtx);
    if (_buffers.empty())
    {
        return false;
    }
    else
    {
        frame = _buffers.front();
        _buffers.pop_front();
        return true;
    }
}

bool OpenH264Decoder::CanPush() 
{
    std::lock_guard<std::mutex> lock(_mtx);
    std::lock_guard<std::mutex> bufLock(_bufMtx);
    return _buffers.size() < _bufSize;
}

bool OpenH264Decoder::CanPop()
{
    std::lock_guard<std::mutex> lock(_mtx);
    return _buffers.empty();
}

const std::string& OpenH264Decoder::Description()
{
    static const std::string desc = "H264(openh264) decoder";
    return desc;
}

} // namespace Codec
} // namespace Mmp