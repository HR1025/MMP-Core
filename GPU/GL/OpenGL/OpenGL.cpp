#include "OpenGL.h"

#include <sstream>
#include <algorithm>
#include <Poco/Timestamp.h>

#include "OpenGLUtil.h"
#include "OpenGLContex.h"
#include "OpenGLStepData.h"
#include "OpenGLInitStepData.h"
#include "OpenGLRenderData.h"

namespace Mmp
{

/**
 * @sa 1 - es_spec_3.2.pdf - Table 8.2: Valid combinations of format, type, and sized internal format.
 *     2 - es_spec_3.2.pdf - Table 8.3: Valid combinations of format, type, and unsized internal format.
 */
bool OpenGL::DataFormatToGLFormatAndType(DataFormat dataFormat, OpenGLFeature& glFeature, GLuint& internalFormat, GLuint& format, GLuint& type, int& alignment)
{
    alignment = 4;
    switch (dataFormat)
    {
        case DataFormat::R16_UNORM:
        {
            internalFormat = GL_RGBA;
            format = GL_RED;
            type = GL_UNSIGNED_SHORT;
            alignment = 2;
            break;
        }
        case DataFormat::R8_UNORM:
        {
            if (glFeature.IsGLES)
            {
                internalFormat = GL_LUMINANCE;
                format = GL_LUMINANCE;
            }
            else if (OpenGLVersionGreater(glFeature, 3, 0))
            {
                internalFormat = GL_RED;
                format = GL_RED;
            }
            else
            {
                internalFormat = GL_RGBA;
                format = GL_RED;
            }
            type = GL_UNSIGNED_BYTE;
            alignment = 1;
            break;
        }
        case DataFormat::R8G8_UNORM:
        {
            internalFormat = GL_RG8;
            format = GL_RG;
            type = GL_UNSIGNED_BYTE;
            alignment = 2;
            break;
        }
        case DataFormat::R8G8B8A8_UNORM:
        {
            internalFormat = GL_RGBA;
            format = GL_RGBA;
            type = GL_UNSIGNED_BYTE;
            break;
        }
        case DataFormat::D32F:
        {
            internalFormat = GL_DEPTH_COMPONENT;
            format = GL_DEPTH_COMPONENT;
            type = GL_FLOAT;
            break;
        }
        case DataFormat::R8G8B8_UNORM:
        {
            internalFormat = GL_RGB;
            format = GL_RGB;
            type = GL_UNSIGNED_BYTE;
            alignment = 3;
            break;
        }
        case DataFormat::R4G4B4A4_UNORM_PACK16:
        {
            internalFormat = GL_RGBA;
            format = GL_RGBA;
            type = GL_UNSIGNED_SHORT_4_4_4_4;
            alignment = 2;
            break;
        }
        case DataFormat::R5G6B5_UNORM_PACK16:
        {
            internalFormat = GL_RGB;
            format = GL_RGB;
            type = GL_UNSIGNED_SHORT_5_6_5;
            alignment = 2;
            break;
        }
        case DataFormat::R5G5B5A1_UNORM_PACK16:
        {
            internalFormat = GL_RGBA;
            format = GL_RGBA;
            type = GL_UNSIGNED_SHORT_5_5_5_1;
            alignment = 2;
            break;
        }
        case DataFormat::R32G32B32A32_FLOAT:
        {
            internalFormat = GL_RGBA32F;
            format = GL_RGBA;
            type = GL_FLOAT;
            alignment = 16;
            break;
        }
        case DataFormat::Y8_UNORM: /* pass through */
        case DataFormat::U8_UNORM: /* pass through */
        case DataFormat::V8_UNORM:
        {
            if (glFeature.IsGLES && !glFeature.GLES3)
            {
                internalFormat = GL_LUMINANCE;
                format = GL_LUMINANCE;
            }
            else if (OpenGLVersionGreater(glFeature, 3, 0))
            {
                internalFormat = GL_RED;
                format = GL_RED;
            }
            else
            {
                internalFormat = GL_RGBA;
                format = GL_RED;
            }
            type = GL_UNSIGNED_BYTE;
            alignment = 1;
            break;
        }
        case DataFormat::U8V8_UNORM:
        {
            internalFormat = GL_RG8;
            format = GL_RG;
            type = GL_UNSIGNED_BYTE;
            alignment = 2;
            break;
        }
        default:
        {
            assert(false);
            return false;
        }
    }
    return true;
}


OpenGL::OpenGL()
{
    CheckOpenGLExtensions();
    _openGLFeature = GetOpenGLFeature();

    _bufferStategy = OpenGLBufferStrategy::SUBDATA;
    _curStep       = nullptr;
}

OpenGL::~OpenGL()
{
}

void OpenGL::ThreadStart()
{
    GL_LOG_INFO << "Render Thread Start";
    CHECK_GL_ERROR_IF_DEBUG();
    CreateDeviceObjects();
    _renderTID = std::this_thread::get_id();
    bool mapBuffers = true;
    {
        bool hasBufferStorge = _openGLFeature.ARB_buffer_storage || _openGLFeature.EXT_buffer_storage;
        if (!OpenGLVersionGreater(_openGLFeature, 3, 0) && _openGLFeature.IsGLES && !hasBufferStorge)
        {
            mapBuffers = false;
        }
    }
    _bufferStategy = OpenGLBufferStrategy::SUBDATA; /* default value */
    // See also : https://github.com/hrydgard/ppsspp/commit/7c983a6842bcebdec7f83a9d8fe88d90b47f33ff
    if (mapBuffers)
    {
        switch (_openGLFeature.gpuVendor)
        {
            case GpuVendor::NVIDIA:
                _bufferStategy = OpenGLBufferStrategy::FRAME_UNMAP;
                break;
            default:
                break;
        }
    }
    _running = true;
    CHECK_GL_ERROR_IF_DEBUG();
}

void OpenGL::ThreadEnd()
{
    GL_LOG_INFO << "Render Thread End";
    DestroyDeviceObjects();
    std::vector<OpenGLInitStep>    initSteps;
    std::vector<OpenGLRenderStep>  steps;
    _currentFrameBuffer.reset();
    _initSteps.swap(initSteps);
    _steps.swap(steps);
}

void OpenGL::ThreadStop()
{
    GL_LOG_INFO << "Render Thread Stop";
    // Hint : 处理渲染器线程退出特殊操作
    if (_renderTID != std::this_thread::get_id())
    {
        if (_running)
        {
            _running = false;
            std::lock_guard<std::mutex> lock(_renderMtx);
            OpenGLThreadTask exitTask;
            exitTask.runType = OpenGLRenderRunType::EXIT;
            _tasks.push(exitTask);
            _renderCond.notify_one();
        }
        else
        {
            GL_LOG_WARN << "Render Thread is already in pausing state, not need to stop again, waiting for exit";
        }
        FlushSync();
    }
    else
    {
        // TODO : 写得有点恶心,之后想想怎么处理
        std::lock_guard<std::mutex> lock(_renderMtx);
        _running = false;
        std::queue<OpenGLThreadTask>   tasks;
        _tasks.swap(tasks);
        // * 资源回收
        {
            // Hint : 实际上还是借助于 C++ RAII 的特性
            std::lock_guard<std::recursive_mutex> locker(_recyclingMtx);
            std::vector<Any> dummy;
            _recycling.swap(dummy);
            glFinish();
        }
    }
}

GpuTaskStatus OpenGL::ThreadFrame()
{
    OpenGLThreadTask task;
    while (true)
    {
        // 获取一个渲染任务
        {
            std::unique_lock<std::mutex> lock(_renderMtx);
            while (_tasks.empty())
            {
                _renderCond.wait(lock);
            }
            task = _tasks.front();
            _tasks.pop();
        }
        // * 资源回收
        {
            // Hint : 实际上还是借助于 C++ RAII 的特性
            std::lock_guard<std::recursive_mutex> lock(_recyclingMtx);
            bool needClean = !_recycling.empty();
            if (needClean)
            {
                std::vector<Any> dummy;
                _recycling.swap(dummy);
            }
            if (needClean)
            {
                glFinish();
            }
        }
        // * 处理渲染线程退出请求
        {
            if (task.runType == OpenGLRenderRunType::EXIT)
            {
                GL_LOG_INFO << "Receive Exit Request, free sync operation";
                {
                    std::unique_lock<std::mutex> lock(_renderMtx);
                    std::queue<OpenGLThreadTask>   tasks;
                    _tasks.swap(tasks);
                }
                std::lock_guard<std::mutex> lock(_syncMtx);
                _syncDone = true;
                _syncCond.notify_one();
                break;
            }
        }
        GL_LOG_TRACE << "Frame " << task.frame << " RUN";
        if (Run(task))
        {
            if (task.runType == OpenGLRenderRunType::SYNC)
            {
                std::lock_guard<std::mutex> lock(_syncMtx);
                _syncDone = true;
                _syncCond.notify_one();
            }
            break;
        }
    }
    if (task.runType == OpenGLRenderRunType::EXIT)
    {
        return GpuTaskStatus::EXIT;
    }
    else if (task.runType == OpenGLRenderRunType::PRESENT && !_currentFrameBuffer)
    {
        return GpuTaskStatus::PRESENT;
    }
    else
    {
        return GpuTaskStatus::NORMAL;
    }
    
}

bool OpenGL::Run(OpenGLThreadTask& task)
{
    RunInitStep(task.initSteps);
    RunRenderStep(task.steps);
    // more thing to check
    return true;
}

void OpenGL::FlushSync()
{
    Poco::Timestamp timeStamp;
    GL_LOG_TRACE << "Sync Begin";
    // 提交 SYNC 任务
    {
        OpenGLThreadTask task;
        task.runType = OpenGLRenderRunType::SYNC;
        std::lock_guard<std::mutex> lock(_renderMtx);
        task.initSteps = std::move(_initSteps);
        task.steps     = std::move(_steps);
        _tasks.push(task);
        _renderCond.notify_one();
    }
    // 等待 SYNC 任务执行完成 (区分跨线程和非跨线程)
    if (_renderTID != std::this_thread::get_id())
    {
        std::unique_lock<std::mutex> lock(_syncMtx);
        while (!_syncDone)
        {
            _syncCond.wait(lock);
        }
        GL_LOG_TRACE << "Sync End";
        auto useTimeMs = timeStamp.elapsed() / 1000;
        // Hint : 耗时超过 2 second 添加打印
        if (useTimeMs > 2 * 1000)
        {
            GL_LOG_WARN << "Sync wait too long, use " << useTimeMs << " ms";
        }
        _syncDone = false;
    }
    else
    {
        // Hint : product 环境中实际上不会走到这里来,渲染线程不会自己调用 FlushSync,
        //        在 Sample 测试环境中会走到这个逻辑
        ThreadFrame();
    }
}

void OpenGL::ReleaseResource(Any any)
{
    std::lock_guard<std::recursive_mutex> lock(_recyclingMtx);
    _recycling.push_back(any);
}

void OpenGL::CreateDeviceObjects()
{
    CHECK_GL_ERROR_IF_DEBUG();
    if (_openGLFeature.EXT_texture_filter_anisotropic)
    {
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &_maxAnisotropyLevel);
    }
    else
    {
        _maxAnisotropyLevel = 0.0f;
    }
    if (_openGLFeature.ARB_vertex_array_object)
    {
        glGenVertexArrays(1, &_globalVAO);
    }
    CHECK_GL_ERROR_IF_DEBUG();
}

void OpenGL::DestroyDeviceObjects()
{
    CHECK_GL_ERROR_IF_DEBUG();
    if (_openGLFeature.ARB_vertex_array_object)
    {
        glDeleteVertexArrays(1, &_globalVAO);
    }
    delete[] _readBackBuffer;
    _readBackBufferSize = 0;
    CHECK_GL_ERROR_IF_DEBUG();
}

void OpenGL::RunInitStep(const std::vector<OpenGLInitStep>& steps)
{
    size_t stepSize = steps.size();
    GLuint boundTexture = (GLuint) - 1;
    glActiveTexture(GL_TEXTURE0);
    
    for (size_t i = 0; i < stepSize; i++)
    {
        const OpenGLInitStep& step = steps[i];
        switch (step.type)
        {
            case OpenGLInitStepType::CREATE_TEXTURE:
            {
                boundTexture = InitStepCreateTexture(step.data);
                break;
            }
            case OpenGLInitStepType::CREATE_BUFFER:
            {
                InitStepCreateBuffer(step.data);
                break;
            }
            case OpenGLInitStepType::BUFFER_SUBDATA:
            {
                InitStepBufferSubdata(step.data);
                break;
            }
            case OpenGLInitStepType::CREATE_PROGRAM:
            {
                InitStepCreateProgram(step.data);
                break;
            }
            case OpenGLInitStepType::CREATE_SHADER:
            {
                InitStepCreateShader(step.data);
                break;
            }
            case OpenGLInitStepType::CREATE_INPUT_LAYOUT:
            {
                InitStepCreateInputLayout(step.data);
                break;
            }
            case OpenGLInitStepType::CREATE_FRAMEBUFFER:
            {
                boundTexture = (GLuint) - 1;
                InitStepCreateFrameBuffer(step.data);
                break;
            }
            case OpenGLInitStepType::TEXTURE_IMAGE:
            {
                InitStepTextureImage(step.data, boundTexture);
                break;
            }
            case OpenGLInitStepType::TEXTURE_FINALIZE:
            {
                InitStepTextureFinalize(step.data, boundTexture);
                break;
            }
            default:
            {
                GL_LOG_WARN << "Unknown init step type, type is: " << step.type;
                assert(false);
                break;
            }
        }
    }
    CHECK_GL_ERROR_IF_DEBUG();
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return;
}

void OpenGL::RunRenderStep(std::vector<OpenGLRenderStep>& steps)
{
    auto skipEmptyRenderStep = [](std::vector<OpenGLRenderStep>& steps) -> void
    {
        for (auto& step : steps)
        {
            if (step.stepType == OpenGLRenderStepType::RENDER && step.commands.empty())
            {
                step.stepType = OpenGLRenderStepType::RENDER_SKIP;
            }
        }
    };
    auto getTotalRenderStepNum = [](const std::vector<OpenGLRenderStep>& steps) -> size_t
    {
        size_t num = 0;
        for (const auto& step : steps)
        {
            if (step.stepType == OpenGLRenderStepType::RENDER)
            {
                num++;
            }
        }
        return num;
    };
    skipEmptyRenderStep(steps);
    size_t totalRenderCount = getTotalRenderStepNum(steps);
    size_t renderCount = 0;
    for (auto& step : steps)
    {
        switch (step.stepType)
        {
            case OpenGLRenderStepType::RENDER:
                renderCount++;
                RenderStepRender(step, renderCount == 1, renderCount == totalRenderCount);
                break;
            case OpenGLRenderStepType::COPY:
                RenderStepCopy(step);
                break;
            case OpenGLRenderStepType::BLIT:
                RenderStepBlit(step);
                break;
            case OpenGLRenderStepType::READBACK:
                RenderStepReadback(step);
                break;
            case OpenGLRenderStepType::READBACK_IMAGE:
                RenderStepReadbackImage(step);
                break;
            case OpenGLRenderStepType::RENDER_SKIP:
                break;
            default:
                GL_LOG_WARN << "Unknown render step type, type is: " << step.stepType;
                assert(false);
                break;
        }
    }
}

void OpenGL::Resize(int width, int height)
{
    _targetWidth = width;
    _targeHeight = height;
}

} // namespace Mmp