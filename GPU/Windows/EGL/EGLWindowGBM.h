//
// EGLWindowGBM.h
//
// Library: GPU
// Package: Windows
// Module:  EGL
// 

#pragma once

#include "EGLWindow.h"

struct gbm_device;
struct gbm_surface;

namespace Mmp
{

/**
 * @brief     GBM EGL 窗口
 *            使用 GBM 拓展在 linux 上能能有效关联 EGL 和 DRM, 以 dma-buf 作为纽带, 进而有效提升数据在不同处理单元(CPU、GPU、NPU、VENC、VDEC ...)之间的传输效率, 这点在 SOC 上尤其明显
 * @sa        1 - GL_KHR_platform_gbm : https://registry.khronos.org/EGL/extensions/KHR/EGL_KHR_platform_gbm.txt
 *            2 - weston drm backend implemention : https://gitlab.freedesktop.org/wayland/weston/-/tree/main/libweston/backend-drm
 *                                                  https://gitlab.freedesktop.org/wayland/weston/-/tree/main/libweston/renderer-gl
 *            3 - virglrender winsys implemention : https://gitlab.freedesktop.org/virgl/virglrenderer/src/vrend_winsys_gbm.c
 *                git commit : 1cc84f897d68c1b0814123cb88740aa9b5cdd8d1
 * @example   在未使用 GBM 前,将数据从解码器传输到GPU的正常路径是, VDEC->CPU->GPU, 以 CPU 作为数据传输的跳板
 *            而使用 GBM 后,则可以实现 VDEC->GPU 的数据传输,尤其较好地解决了两个问题:
 *                1 - DDR 带宽, 以 1080@60 计算, 节省了 (8M(VDEC->CPU) + (8+32)(CPU->GPU)) * 60 = 2,880 MB/s 带宽
 *                2 - CPU 负载, CPU 上传数据到 GPU , 以 ARGB 为例数据格式由 4 * uint8 转换至 4 * float32, 部分驱动此操作使用 CPU 实现
 *            除此之外, 借助于 linux DRM 的体系, 某些 GPU 能够原生支持 yuv texture, 提供内置 sampler 采样函数
 *            (但是此操作具有很强的平台差异性, 基本上需要针对不同厂商的驱动进行点对点适配)
 */
class EGLWindowGBM : public EGLWindow 
{
public:
    EGLWindowGBM();
    ~EGLWindowGBM();
public:
    void Swap() override;
private:
    std::vector<EGLConfig> GetAllEGLConfigs() override;
private:
    bool Open() override;
    bool Close() override;
    EGLDisplay OnOpenDisplay() override;
    EGLConfig  OnChooseEGLConfig() override;
    EGLSurface OnCreateSurface() override;
private:
    int                 _fd;
    std::string         _renderNode;
    struct gbm_device*  _device;
    struct gbm_surface* _surface;
    bool                _drmOpen;
};

} // namespace Mmp
