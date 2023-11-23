#include "VAH264Decoder.h"

#include <cassert>
#include <memory.h>

#include "VAUtil.h"
#include "VATranslator.h"

namespace Mmp
{
namespace Codec
{

class VAH264ByteReader : public AbstractH26xByteReader
{
public:
    VAH264ByteReader(Pack::ptr pack)
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
    Pack::ptr _pack;
};

VAH264Decoder::VAH264Decoder()
{
    _deserialize = std::make_shared<H264Deserialize>();
}

bool VAH264Decoder::Push(Pack::ptr pack)
{
    H26xBinaryReader::ptr br = std::make_shared<H26xBinaryReader>(std::make_shared<VAH264ByteReader>(pack));
    H264NalSyntax::ptr nal = std::make_shared<H264NalSyntax>();
    if (!_deserialize->DeserializeNalSyntax(br, nal))
    {
        assert(false);
        return false;
    }

    return false;
}

void VAH264Decoder::StartFrame(const Any& context)
{
    if (context.type() != typeid(H264StartFrameContext))
    {
        assert(false);
        return;
    }
    static auto fillVAPictureH264 = [this](VAPictureH264& vaPicture, H264SliceHeaderSyntax::ptr slice) -> void
    {
        vaPicture.picture_id = _picContext.surface;
        vaPicture.flags = 0;
        if (!(slice->field_pic_flag == 1 && slice->bottom_field_flag == 1))
        {
            vaPicture.flags = slice->field_pic_flag ? VA_PICTURE_H264_TOP_FIELD : VA_PICTURE_H264_BOTTOM_FIELD;
        }
        if (slice->field_pic_flag == 1 || slice->bottom_field_flag == 1)
        {
            assert(false);
        }
        
    };

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
        decoderParam.width = (sps->pic_width_in_mbs_minus1 + 1) * 16;
        decoderParam.height = (sps->pic_height_in_map_units_minus1 + 1) * 16;
        SetDecoderParams(decoderParam);
    }


    VAPictureParameterBufferH264 picParam = {};
    {
        picParam.picture_width_in_mbs_minus1 = decoderParam.width / 16 - 1;
        picParam.picture_height_in_mbs_minus1 = decoderParam.height / 16 - 1;
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

    assert(false);
    // TODO
}

void VAH264Decoder::DecodedBitStream(const Any& context)
{
    if (context.type() != typeid(H264DecodedBitStreamContext))
    {
        assert(false);
        return;
    }
    static auto fillVaRefPicList = []() -> void
    {

    };
    static auto fillVaPredWeightTable = [](VASliceParameterBufferH264& sliceParam, H264SliceHeaderSyntax::ptr slice) -> void
    {
        H264PredictionWeightTableSyntax::ptr pwt = slice->pwt;
        // num_ref_idx_l0_active_minus1
        for (uint32_t i=0; i<=slice->num_ref_idx_l0_active_minus1; i++)
        {
            if (pwt->luma_weight_l0_flag[i])
            {
                sliceParam.luma_weight_l0[i] = pwt->luma_weight_l0[i];
                sliceParam.luma_offset_l0[i] = pwt->luma_offset_l0[i];
            }
            else
            {
                sliceParam.luma_weight_l0[i] = 1 << pwt->luma_log2_weight_denom;
                sliceParam.luma_offset_l0[i] = 0;
            }
            for (size_t j=0; j<2; j++)
            {
                if (pwt->chroma_weight_l0_flag[i])
                {
                    sliceParam.chroma_weight_l0[i][j] = pwt->chroma_weight_l0[i][j];
                    sliceParam.chroma_offset_l0[i][j] = pwt->chroma_offset_l0[i][j];
                }
                else
                {
                    sliceParam.chroma_weight_l0[i][j] = 1 << pwt->luma_log2_weight_denom;
                    sliceParam.chroma_offset_l0[i][j] = 0;
                }
            }
        }
        // num_ref_idx_l1_active_minus1
        for (uint32_t i=0; i<=slice->num_ref_idx_l1_active_minus1; i++)
        {
            if (pwt->luma_weight_l1_flag[i])
            {
                sliceParam.luma_weight_l1[i] = pwt->luma_weight_l1[i];
                sliceParam.luma_offset_l1[i] = pwt->luma_offset_l1[i];
            }
            else
            {
                sliceParam.luma_weight_l1[i] = 1 << pwt->luma_log2_weight_denom;
                sliceParam.luma_offset_l1[i] = 0;
            }
            for (size_t j=0; j<2; j++)
            {
                if (pwt->chroma_weight_l1_flag[i])
                {
                    sliceParam.chroma_weight_l1[i][j] = pwt->chroma_weight_l1[i][j];
                    sliceParam.chroma_offset_l1[i][j] = pwt->chroma_offset_l1[i][j];
                }
                else
                {
                    sliceParam.chroma_weight_l1[i][j] = 1 << pwt->luma_log2_weight_denom;
                    sliceParam.chroma_offset_l1[i][j] = 0;
                }
            }
        }
    };

    const H264DecodedBitStreamContext& _contex = RefAnyCast<H264DecodedBitStreamContext>(context);
    H264SliceHeaderSyntax::ptr slice = _contex.slice;
    VASliceParameterBufferH264 sliceParameter = {};
    {
        sliceParameter.slice_data_size = 0; // TODO
        sliceParameter.slice_data_offset = 0;
        sliceParameter.slice_data_flag = VA_SLICE_DATA_FLAG_ALL;
        sliceParameter.slice_data_bit_offset = 0; // TODO
        sliceParameter.first_mb_in_slice = slice->first_mb_in_slice;
        sliceParameter.slice_type = H264SliceTypeToVaSliceType((H264SliceType)slice->slice_type);
        sliceParameter.num_ref_idx_l0_active_minus1 = slice->num_ref_idx_l0_active_minus1;
        sliceParameter.num_ref_idx_l1_active_minus1 = slice->num_ref_idx_l1_active_minus1;
        sliceParameter.cabac_init_idc = slice->cabac_init_idc;
        sliceParameter.slice_qp_delta = slice->slice_qp_delta;
        sliceParameter.disable_deblocking_filter_idc = slice->disable_deblocking_filter_idc;
        sliceParameter.slice_alpha_c0_offset_div2 = slice->slice_alpha_c0_offset_div2;
        sliceParameter.slice_beta_offset_div2 = slice->slice_beta_offset_div2;
        sliceParameter.luma_log2_weight_denom = slice->pwt->luma_log2_weight_denom;
        sliceParameter.chroma_log2_weight_denom = slice->pwt->chroma_log2_weight_denom;
    }
    // TODO
    fillVaPredWeightTable(sliceParameter, slice);
    // TODO
    assert(false);
}

void VAH264Decoder::EndFrame(const Any& context)
{
    assert(false);
    // TODO
}

void VAH264Decoder::OnVaDecoderParamsChange(const VaDecoderParams& oldValue, const VaDecoderParams& newValue)
{
    if (_picContext.surface != VA_INVALID_ID)
    {
        DestroyVaSurface(_picContext.surface);
        _picContext.surface = VA_INVALID_ID;
    }
    std::vector<VASurfaceAttrib> attributes;
    if (newValue.flag & MMP_VA_DECODE_FALG_NEED_MEMORY_TYPE)
    {
        VASurfaceAttrib attribute = {};
        {
            attribute.type = VASurfaceAttribMemoryType;
            attribute.flags = VA_SURFACE_ATTRIB_SETTABLE;
            attribute.value.type = VAGenericValueTypeInteger;
            attribute.value.value.i = VA_SURFACE_ATTRIB_MEM_TYPE_VA;
        };
        attributes.push_back(attribute);
    }
    if (newValue.flag && MMP_VA_DECODE_FALG_NEED_MEMORY_TYPE)
    {
        assert(false);
        // TODO
    }
    _picContext.surface == CreateVaSurface(attributes);
    VADecoder::OnVaDecoderParamsChange(oldValue, newValue);
}

} // namespace Codec
} // namespace Mmp