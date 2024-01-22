#include "VAH264Decoder.h"

#include <cassert>
#include <memory>
#include <memory.h>

#include "VAUtil.h"
#include "VATranslator.h"

namespace Mmp
{
namespace Codec
{

static void FillVAPictureH264(VAH264DecodePictureContext::ptr context, VAPictureH264& vaPicture)
{
    assert(context->surface != VA_INVALID_ID);
    vaPicture.picture_id = context->surface;
    vaPicture.frame_idx = context->referenceFlag & H264PictureContext::used_for_long_term_reference ? context->long_term_frame_idx : context->FrameNum;
    vaPicture.flags = 0;
    if (context->bottom_field_flag == 1)
    {
        vaPicture.flags |= VA_PICTURE_H264_BOTTOM_FIELD;
    }
    else if (/* context->bottom_field_flag == 0 && */ context->field_pic_flag == 1)
    {
        vaPicture.flags |= VA_PICTURE_H264_TOP_FIELD;
    }
    if (context->referenceFlag & H264PictureContext::used_for_long_term_reference)
    {
        vaPicture.flags |= VA_PICTURE_H264_LONG_TERM_REFERENCE;
    }
    else if (context->referenceFlag & H264PictureContext::used_for_short_term_reference)
    {
        vaPicture.flags |= VA_PICTURE_H264_SHORT_TERM_REFERENCE;
    }
    vaPicture.TopFieldOrderCnt = context->TopFieldOrderCnt;
    vaPicture.BottomFieldOrderCnt = context->BottomFieldOrderCnt;
}

class H264StartFrameContext
{
public:
    H264SpsSyntax::ptr         sps;
    H264PpsSyntax::ptr         pps;
    H264SliceHeaderSyntax::ptr slice;
    H264NalSyntax::ptr         nal;
};

class H264DecodedBitStreamContext
{
public:
    H264SliceHeaderSyntax::ptr slice;
    AbstractPack::ptr          pack;
};

class H264EndFrameContext
{
public:
    
};

class VAH264ByteReader : public AbstractH26xByteReader
{
public:
    VAH264ByteReader(AbstractPack::ptr pack)
    {
        _pack = pack;
        _end = _pack->GetSize() + 1;
        _cur = 0;
    }
public:
    size_t Read(void* data, size_t bytes) override
    {
        uint32_t readSize = bytes < (_end - _cur) ? bytes : (_end - _cur);
        if (readSize != 0)
        {
            memcpy(data, (uint8_t*)_pack->GetData() + _cur, readSize);
        }
        _cur += readSize;
        return size_t(readSize);
    }
    bool Seek(size_t offset)  override
    {
        if (offset >= 0 && offset < _end)
        {
            _cur = offset;
            return true;
        }
        else
        {
            return false;
        }
    }
    size_t Tell() override
    {
        return size_t(_cur);
    }
    bool Eof() override
    {
        return _cur == _end;
    }
private:
    uint32_t _cur;
    uint32_t _end;
    AbstractPack::ptr _pack;
};

VAH264Decoder::VAH264Decoder()
{
    _deserialize = std::make_shared<H264Deserialize>();
    _deserializeContext = std::make_shared<H264ContextSyntax>();
    _sliceDecoding = std::make_shared<VaH264SliceDecodingProcess>();
}

bool VAH264Decoder::Push(AbstractPack::ptr pack)
{
    H26xBinaryReader::ptr br = std::make_shared<H26xBinaryReader>(std::make_shared<VAH264ByteReader>(pack));
    H264NalSyntax::ptr nal = std::make_shared<H264NalSyntax>();
    if (!_deserialize->DeserializeNalSyntax(br, nal))
    {
        assert(false);
        return false;
    }
    _sliceDecoding->SliceDecodingProcess(nal);
    switch (nal->nal_unit_type)
    {
        case H264NaluType::MMP_H264_NALU_TYPE_SPS:
            _deserializeContext->spsSet[nal->sps->seq_parameter_set_id] = nal->sps;
        break;
        case H264NaluType::MMP_H264_NALU_TYPE_PPS:
            _deserializeContext->ppsSet[nal->pps->pic_parameter_set_id] = nal->pps;
        break;
        default:
        break;
    }
    if (!(nal->nal_unit_type == H264NaluType::MMP_H264_NALU_TYPE_SLICE || nal->nal_unit_type == H264NaluType::MMP_H264_NALU_TYPE_IDR))
    {
        return true;
    }
    {
        H264StartFrameContext context;
        context.nal = nal;
        context.slice = nal->slice;
        if (_deserializeContext->ppsSet.count(nal->slice->pic_parameter_set_id) == 0)
        {
            return true;
        }
        context.pps = _deserializeContext->ppsSet[nal->slice->pic_parameter_set_id];
        if (_deserializeContext->spsSet.count(context.pps->seq_parameter_set_id) == 0)
        {
            return true;
        }
        context.sps = _deserializeContext->spsSet[context.pps->seq_parameter_set_id];
        StartFrame(context);
    }
    {
        H264DecodedBitStreamContext context;
        context.slice = nal->slice;
        context.pack = pack;
        DecodedBitStream(context);
    }
    {
        H264EndFrameContext context;
        EndFrame(context);
    }

    return true;
}

void VAH264Decoder::StartFrame(const Any& context)
{
    if (context.type() != typeid(H264StartFrameContext))
    {
        assert(false);
        return;
    }

    const H264StartFrameContext& _context = RefAnyCast<H264StartFrameContext>(context);
    H264SpsSyntax::ptr sps = _context.sps;
    H264PpsSyntax::ptr pps = _context.pps;
    H264SliceHeaderSyntax::ptr slice = _context.slice;
    H264NalSyntax::ptr  nal = _context.nal;

    VaDecoderParams decoderParam = GetDecoderParams();
    if (decoderParam.width != (sps->pic_width_in_mbs_minus1 + 1) * 16 || 
        decoderParam.height != (sps->pic_height_in_map_units_minus1 + 1) * 16
    )
    {
        decoderParam.profile = H264ProfileToVaProfile((H264Profile)sps->profile_idc);
        decoderParam.width = (sps->pic_width_in_mbs_minus1 + 1) * 16;
        decoderParam.height = (sps->pic_height_in_map_units_minus1 + 1) * 16;
        SetDecoderParams(decoderParam);
    }

    {
        VAH264DecodePictureContext::ptr picture = std::dynamic_pointer_cast<VAH264DecodePictureContext>(_sliceDecoding->GetCurrentPictureContext());
        if (!InitH264Picture(picture))
        {
            VAAPI_LOG_ERROR << "InitH264Picture fail";
            assert(false);
        }
        _curPic = picture;
    }
    // VAPictureParameterBufferType
    {
        VAPictureParameterBufferH264 picParam = {};
        {
            picParam.picture_width_in_mbs_minus1 = sps->pic_width_in_mbs_minus1;
            picParam.picture_height_in_mbs_minus1 = sps->pic_height_in_map_units_minus1;
            picParam.bit_depth_luma_minus8 = sps->bit_depth_luma_minus8;
            picParam.bit_depth_chroma_minus8 = sps->bit_depth_chroma_minus8;
            picParam.num_ref_frames = sps->max_num_ref_frames;
            picParam.seq_fields.bits = 
            {
                .chroma_format_idc = sps->chroma_format_idc,
                .residual_colour_transform_flag = sps->separate_colour_plane_flag,
                .gaps_in_frame_num_value_allowed_flag = sps->gaps_in_frame_num_value_allowed_flag,
                .frame_mbs_only_flag = sps->frame_mbs_only_flag,
                .mb_adaptive_frame_field_flag = sps->mb_adaptive_frame_field_flag,
                .direct_8x8_inference_flag = sps->direct_8x8_inference_flag,
                .MinLumaBiPredSize8x8 = sps->level_idc >= 31, /* Reference : FFmpeg 6.x */
                .log2_max_frame_num_minus4 = sps->log2_max_frame_num_minus4,
                .pic_order_cnt_type = sps->pic_order_cnt_type,
                .log2_max_pic_order_cnt_lsb_minus4 = sps->log2_max_pic_order_cnt_lsb_minus4,
                .delta_pic_order_always_zero_flag = sps->delta_pic_order_always_zero_flag
            };
            picParam.pic_init_qp_minus26 = pps->pic_init_qp_minus26;
            picParam.pic_init_qs_minus26 = pps->pic_init_qs_minus26;
            picParam.chroma_qp_index_offset = pps->chroma_qp_index_offset;
            picParam.second_chroma_qp_index_offset = pps->second_chroma_qp_index_offset;
            picParam.pic_fields.bits = 
            {
                .entropy_coding_mode_flag = pps->entropy_coding_mode_flag,
                .weighted_pred_flag = pps->weighted_pred_flag,
                .weighted_bipred_idc = pps->weighted_bipred_idc,
                .transform_8x8_mode_flag = pps->transform_8x8_mode_flag,
                .field_pic_flag = slice->field_pic_flag,
                .constrained_intra_pred_flag = pps->constrained_intra_pred_flag,
                .pic_order_present_flag = pps->bottom_field_pic_order_in_frame_present_flag,
                .deblocking_filter_control_present_flag = pps->deblocking_filter_control_present_flag,
                .redundant_pic_cnt_present_flag = pps->redundant_pic_cnt_present_flag,
                .reference_pic_flag = nal->nal_ref_idc != 0
            };
            picParam.frame_num = slice->frame_num;
        }
        // VAPictureParameterBufferH264::CurrPic
        {
            FillVAPictureH264(_curPic, picParam.CurrPic);
        }
        // VAPictureParameterBufferH264::ReferenceFrames
        {
            for (size_t i=0; i<sizeof(picParam.ReferenceFrames)/sizeof(VAPictureH264); i++)
            {
                picParam.ReferenceFrames[i].picture_id = VA_INVALID_ID;
                picParam.ReferenceFrames[i].flags = VA_PICTURE_H264_INVALID;
                picParam.ReferenceFrames[i].TopFieldOrderCnt = 0;
                picParam.ReferenceFrames[i].BottomFieldOrderCnt = 0;
            }
            auto __pictures = _sliceDecoding->GetAllPictures();
            size_t index = 0;
            for (auto picture : __pictures)
            {
                VAH264DecodePictureContext::ptr vaPicture = std::dynamic_pointer_cast<VAH264DecodePictureContext>(picture);
                if (vaPicture->referenceFlag & H264PictureContext::used_for_short_term_reference && _sliceDecoding->GetCurrentPictureContext()->id != vaPicture->id)
                {
                    FillVAPictureH264(vaPicture, picParam.ReferenceFrames[index]);
                    index++;
                }
            }
            for (auto picture : __pictures)
            {
                VAH264DecodePictureContext::ptr vaPicture = std::dynamic_pointer_cast<VAH264DecodePictureContext>(picture);
                if (vaPicture->referenceFlag & H264PictureContext::used_for_long_term_reference && _sliceDecoding->GetCurrentPictureContext()->id != vaPicture->id)
                {
                    FillVAPictureH264(vaPicture, picParam.ReferenceFrames[index]);
                    index++;
                }
            }
        }
        _curPic->paramBuffers.push_back(CreateVaParamBuffer(VAPictureParameterBufferType, &picParam, sizeof(VAPictureParameterBufferH264)));
    }
    // VAIQMatrixBufferType
    {
        VAIQMatrixBufferH264 iqMatix = {};
        for (size_t i=0; i<pps->ScalingList4x4.size() && i<6; i++)
        {
            const std::vector<int32_t>& ScalingList4 = pps->ScalingList4x4[i];
            for (size_t j=0; j<ScalingList4.size() && j<sizeof(iqMatix.ScalingList4x4); j++)
            {
                iqMatix.ScalingList4x4[i][j] = ScalingList4[j];
            }
        }
        for (size_t i=0; i<pps->ScalingList8x8.size() && i<2; i++)
        {
            const std::vector<int32_t>& ScalingList8 = pps->ScalingList8x8[i];
            for (size_t j=0; j<ScalingList8.size() && j<sizeof(iqMatix.ScalingList8x8); j++)
            {
                iqMatix.ScalingList8x8[i][j] = ScalingList8[j];
            }
        }
        _curPic->paramBuffers.push_back(CreateVaParamBuffer(VAIQMatrixBufferType, &iqMatix, sizeof(VAIQMatrixBufferH264)));
    }
}

void VAH264Decoder::DecodedBitStream(const Any& context)
{
    if (context.type() != typeid(H264DecodedBitStreamContext))
    {
        assert(false);
        return;
    }
    const H264DecodedBitStreamContext& _contex = RefAnyCast<H264DecodedBitStreamContext>(context);
    H264SliceHeaderSyntax::ptr slice = _contex.slice;
    AbstractPack::ptr pack = _contex.pack;
    VASliceParameterBufferH264 sliceParameter = {};
    {
        sliceParameter.slice_data_size = pack->GetSize();
        sliceParameter.slice_data_offset = 0;
        sliceParameter.slice_data_flag = VA_SLICE_DATA_FLAG_ALL;
        sliceParameter.slice_data_bit_offset = slice->slice_data_bit_offset;
        sliceParameter.first_mb_in_slice = slice->first_mb_in_slice;
        sliceParameter.slice_type = H264SliceTypeToVaSliceType((H264SliceType)slice->slice_type);
        sliceParameter.num_ref_idx_l0_active_minus1 = slice->num_ref_idx_l0_active_minus1;
        sliceParameter.num_ref_idx_l1_active_minus1 = slice->num_ref_idx_l1_active_minus1;
        sliceParameter.cabac_init_idc = slice->cabac_init_idc;
        sliceParameter.slice_qp_delta = slice->slice_qp_delta;
        sliceParameter.disable_deblocking_filter_idc = slice->disable_deblocking_filter_idc;
        sliceParameter.slice_alpha_c0_offset_div2 = slice->slice_alpha_c0_offset_div2;
        sliceParameter.slice_beta_offset_div2 = slice->slice_beta_offset_div2;
        sliceParameter.luma_log2_weight_denom = slice->pwt ? slice->pwt->luma_log2_weight_denom : 0;
        sliceParameter.chroma_log2_weight_denom = slice->pwt ? slice->pwt->chroma_log2_weight_denom : 0;
    }
    // VASliceParameterBufferH264::RefPicList0
    {
        for (size_t i=0; i<sizeof(sliceParameter.RefPicList0)/sizeof(VAPictureH264); i++)
        {
            sliceParameter.RefPicList0[i].picture_id = VA_INVALID_ID;
            sliceParameter.RefPicList0[i].flags = VA_PICTURE_H264_INVALID;
            sliceParameter.RefPicList0[i].TopFieldOrderCnt = 0;
            sliceParameter.RefPicList0[i].BottomFieldOrderCnt = 0;
        }
        auto RefPicList0 = _sliceDecoding->GetRefPicList0();
        size_t index = 0;
        for (const auto& RefPic : RefPicList0)
        {
            VAH264DecodePictureContext::ptr vaRefPic = std::dynamic_pointer_cast<VAH264DecodePictureContext>(RefPic);
            FillVAPictureH264(vaRefPic, sliceParameter.RefPicList0[index]);
            index++;
        }
    }
    // VASliceParameterBufferH264::RefPicList1
    {
        for (size_t i=0; i<sizeof(sliceParameter.RefPicList1)/sizeof(VAPictureH264); i++)
        {
            sliceParameter.RefPicList1[i].picture_id = VA_INVALID_ID;
            sliceParameter.RefPicList1[i].flags = VA_PICTURE_H264_INVALID;
            sliceParameter.RefPicList1[i].TopFieldOrderCnt = 0;
            sliceParameter.RefPicList1[i].BottomFieldOrderCnt = 0;
        }
        auto RefPicList1 = _sliceDecoding->GetRefPicList1();
        size_t index = 0;
        for (const auto& RefPic : RefPicList1)
        {
            VAH264DecodePictureContext::ptr vaRefPic = std::dynamic_pointer_cast<VAH264DecodePictureContext>(RefPic);
            FillVAPictureH264(vaRefPic, sliceParameter.RefPicList1[index]);
            index++;
        }
    }
    if (slice->pwt)
    {
        H264PredictionWeightTableSyntax::ptr pwt = slice->pwt;
        for (uint32_t i=0; i<=slice->num_ref_idx_l0_active_minus1; i++)
        {
            if (!pwt->luma_weight_l0_flag.empty() && pwt->luma_weight_l0_flag[i])
            {
                sliceParameter.luma_weight_l0[i] = pwt->luma_weight_l0[i];
                sliceParameter.luma_offset_l0[i] = pwt->luma_offset_l0[i];
            }
            else
            {
                sliceParameter.luma_weight_l0[i] = 1 << pwt->luma_log2_weight_denom;
                sliceParameter.luma_offset_l0[i] = 0;
            }
            for (size_t j=0; j<2; j++)
            {
                if (!pwt->chroma_weight_l0_flag.empty() && pwt->chroma_weight_l0_flag[i])
                {
                    sliceParameter.chroma_weight_l0[i][j] = pwt->chroma_weight_l0[i][j];
                    sliceParameter.chroma_offset_l0[i][j] = pwt->chroma_offset_l0[i][j];
                }
                else
                {
                    sliceParameter.chroma_weight_l0[i][j] = 1 << pwt->luma_log2_weight_denom;
                    sliceParameter.chroma_offset_l0[i][j] = 0;
                }
            }
        }
        for (uint32_t i=0; i<=slice->num_ref_idx_l1_active_minus1 && slice->slice_type == H264SliceType::MMP_H264_B_SLICE; i++)
        {
            if (pwt->luma_weight_l1_flag[i])
            {
                sliceParameter.luma_weight_l1[i] = pwt->luma_weight_l1[i];
                sliceParameter.luma_offset_l1[i] = pwt->luma_offset_l1[i];
            }
            else
            {
                sliceParameter.luma_weight_l1[i] = 1 << pwt->luma_log2_weight_denom;
                sliceParameter.luma_offset_l1[i] = 0;
            }
            for (size_t j=0; j<2; j++)
            {
                if (pwt->chroma_weight_l1_flag[i])
                {
                    sliceParameter.chroma_weight_l1[i][j] = pwt->chroma_weight_l1[i][j];
                    sliceParameter.chroma_offset_l1[i][j] = pwt->chroma_offset_l1[i][j];
                }
                else
                {
                    sliceParameter.chroma_weight_l1[i][j] = 1 << pwt->luma_log2_weight_denom;
                    sliceParameter.chroma_offset_l1[i][j] = 0;
                }
            }
        }
    }
    _curPic->sliceBuffers.push_back(CreateVaSliceParamBuffer(VASliceDataBufferType, &sliceParameter, sizeof(VASliceParameterBufferH264)));
    _curPic->sliceBuffers.push_back(CreateVaSliceParamBuffer(VASliceDataBufferType, pack->GetData(), pack->GetSize()));
}

void VAH264Decoder::EndFrame(const Any& context)
{
    CommitVaDecodeCommand(_curPic);
    _curPic = nullptr;
}

bool VAH264Decoder::InitH264Picture(VAH264DecodePictureContext::ptr picture)
{
    picture->SetVADecoder(shared_from_this());
    VaDecoderParams decoderParam = GetDecoderParams();
    std::vector<VASurfaceAttrib> attributes;
    if (decoderParam.flag & MmpVaDecodeFlag::MMP_VA_DECODE_FALG_NEED_MEMORY_TYPE)
    {
        VASurfaceAttrib attribute;
        attribute.type = VASurfaceAttribMemoryType;
        attribute.flags = VA_SURFACE_ATTRIB_SETTABLE;
        attribute.value.type = VAGenericValueTypeInteger;
        attribute.value.value.i = VA_SURFACE_ATTRIB_MEM_TYPE_VA;
        attributes.push_back(attribute);
    }
    if (decoderParam.flag & MmpVaDecodeFlag::MMP_VA_DECODE_FALG_NEED_PIX_FORMAT)
    {
        // TODO
        assert(false);
    }
    picture->surface = CreateVaSurface(attributes);
    return picture->surface != VA_INVALID_ID ? true : false;
}

void VAH264Decoder::OnVaDecoderParamsChange(const VaDecoderParams& oldValue, const VaDecoderParams& newValue)
{
    VADecoder::OnVaDecoderParamsChange(oldValue, newValue);
}

} // namespace Codec
} // namespace Mmp