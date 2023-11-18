//
// OpenGL.h
//
// Library: GPU
// Package: GL
// Module:  OpenGL
//

#pragma once

#include <mutex>
#include <memory>
#include <vector>
#include <thread>
#include <atomic>
#include <queue>
#include <condition_variable>

#include "OpenGLCommon.h"

namespace Mmp
{

class OpenGL
{
public:
    using ptr = std::shared_ptr<OpenGL>;
public:
    OpenGL();
    ~OpenGL();
public:
    /**
     * @brief 标志渲染线程开始工作
     * @note  需在渲染线程调用
     */
    void ThreadStart();
    /**
     * @brief 标志渲染线程结束工作
     * @note  需在渲染线程调用
     */
    void ThreadEnd();
    /**
     * @brief 停止渲染线程(异步)
     * @note  可以在非渲染线程调用
     */
    void ThreadStop();
    /**
     * @brief   调度渲染线程
     * @note    阻塞接口
     */
    GpuTaskStatus ThreadFrame();
public:
    /**
     * @brief 初始化步骤
     */
    void RunInitStep(const std::vector<OpenGLInitStep>& steps);
    /**
     * @brief 渲染步骤
     */
    void RunRenderStep(std::vector<OpenGLRenderStep>& steps);
    /**
     * @brief      设置窗口大小
     * @param[in]  width
     * @param[in]  height
     * @note       使用 default framebuffer 时生效
     */
    void Resize(int width, int height);
    /**
     * @brief 同步阻塞等待直到渲染任务完成
     */
    void FlushSync();
    /**
     * @brief     资源释放
     * @param[in] any : OpenGL 资源
     * @note      OpenGL (ES) 规定所有操作都需要在渲染线程中进程,故不能直接使用 C++ RAII,
     *            而应将资源统一抛递到渲染进程,由渲染进程统一进行回收
     */
    void ReleaseResource(Any any);
private:
    void CreateDeviceObjects();
    void DestroyDeviceObjects();
private: /* Init Step, see OpenGLInitStepType */
    GLuint InitStepCreateTexture(Any data);
    void   InitStepCreateBuffer(Any data);
    void   InitStepBufferSubdata(Any data);
    void   InitStepCreateProgram(Any data);
    void   InitStepCreateShader(Any data);
    void   InitStepCreateInputLayout(Any data);
    void   InitStepCreateFrameBuffer(Any data);
    void   InitStepTextureImage(Any data, GLuint& boundTexture);
    void   InitStepTextureFinalize(Any data, GLuint& boundTexture);
private: /* Render Step, see OpenGLRenderStepType */
    void   RenderStepRender(OpenGLRenderStep& step, bool isFrist, bool isLast);
    void   RenderStepCopy(OpenGLRenderStep& step);
    void   RenderStepBlit(OpenGLRenderStep& step);
    void   RenderStepReadback(OpenGLRenderStep& step);
    void   RenderStepReadbackImage(OpenGLRenderStep& step);
private: /* FrameBuffer Utility */
    void      PerformBindFramebufferAsRenderTarget(const OpenGLRenderStep& pass);
    void      FBOExtCreate(Any data);
    void      FBOBindFBTarget(bool read /* !read -> write */, GLuint name);
    GLenum    FBOGetFBTarget(bool read /* !read -> write */, GLuint** cached);
    void      FBOUnbind();
private:
    bool      Run(OpenGLThreadTask& task);
private:
    /**
     * @param[in]     dataFormat
     * @param[in]     glFeature
     * @param[out]    internalFormat
     * @param[out]    format
     * @param[out]    type
     * @param[out]    alignment
     * @sa    1 - es_spec_3.2.pdf - 8.4.2 Transfer of Pixel Rectangles
     *               Table 8.2: Valid combinations of format, type, and sized internalformat.
     *               Table 8.3: Valid combinations of format, type, and unsized internalformat.
     */
    bool DataFormatToGLFormatAndType(DataFormat dataFormat, OpenGLFeature& glFeature, GLuint& internalFormat, GLuint& format, GLuint& type, int& alignment);
private:
    std::shared_ptr<OpenGLRenderFrameBuffer>   _currentFrameBuffer = nullptr;
    int32_t  _currentFrameBufferWidth  = 0;
    int32_t  _currentFrameBufferHeight = 0;
    int32_t  _targetWidth              = 0;
    int32_t  _targeHeight              = 0;
    GLuint   _currentDrawHandle        = 0;
    GLuint   _currentReadHandle        = 0;

    GLuint   _globalVAO                = 0;
private: /* read back buffer */
    uint8_t*     _readBackBuffer           = nullptr;
    size_t       _readBackBufferSize       = 0;
    uint32_t     _readBackAspectMask       = 0;
private: /* render status */
    std::thread::id                _renderTID;
    std::atomic<bool>              _running;
    std::mutex                     _syncMtx;
    std::condition_variable        _syncCond;
    bool                           _syncDone = false;
public:
    std::mutex                     _renderMtx;
    std::condition_variable        _renderCond;
    std::queue<OpenGLThreadTask>   _tasks;
    std::vector<OpenGLRenderStep>  _steps;
    OpenGLRenderStep*              _curStep;
    std::vector<OpenGLInitStep>    _initSteps;
private:
    std::recursive_mutex           _recyclingMtx;
    std::vector<Any>               _recycling;
private:
    OpenGLFeature                  _openGLFeature;
    float                          _maxAnisotropyLevel = 0.0f;
    OpenGLBufferStrategy           _bufferStategy;
};

} // namespace Mmp