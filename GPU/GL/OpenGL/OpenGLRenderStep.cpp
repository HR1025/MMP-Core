#include "OpenGL.h"

#include <memory.h>

#include "OpenGLUtil.h"
#include "OpenGLContex.h"
#include "OpenGLStepData.h"
#include "OpenGLRenderData.h"

namespace Mmp
{

constexpr int MAX_GL_TEXTURE_SLOTS = 8;

void OpenGL::RenderStepRender(OpenGLRenderStep& step, bool isFrist, bool isLast)
{
    constexpr size_t clipDistanceArraySize = 8;

    GL_LOG_TRACE << "[render step] render";

    CHECK_GL_ERROR_IF_DEBUG();

    PerformBindFramebufferAsRenderTarget(step);

    if (isFrist)
    {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_BLEND);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DITHER);
        glEnable(GL_SCISSOR_TEST);
        if (!_openGLFeature.IsGLES)
        {
            glDisable(GL_COLOR_LOGIC_OP);
        }
        if (_openGLFeature.ARB_vertex_array_object)
        {
            glBindVertexArray(_globalVAO);
        }
    }

    OpenGLRenderProgram::ptr  curProgram = nullptr;
    int                       activeSlot = -1;

    // State filtering tracking.
	int attrMask = 0;
	int colorMask = -1;
	int depthMask = -1;
	int depthFunc = -1;
	GLuint curArrayBuffer = (GLuint)0;
	GLuint curElemArrayBuffer = (GLuint)0;
	bool depthEnabled = false;
	bool stencilEnabled = false;
	bool blendEnabled = false;
	bool cullEnabled = false;
	bool ditherEnabled = false;
	bool depthClampEnabled = false;
	int logicOp = -1;
	bool logicEnabled = false;
	bool clipDistanceEnabled[clipDistanceArraySize]{};
	GLuint blendEqColor = (GLuint)-1;
	GLuint blendEqAlpha = (GLuint)-1;

    std::vector<OpenGLRenderTexture::ptr> curTex;
    curTex.resize(MAX_GL_TEXTURE_SLOTS);

    CHECK_GL_ERROR_IF_DEBUG();

    auto& commonds = step.commands;
    for (const auto& command : commonds)
    {
        switch (command.cmd)
        {
            case OpenGLRenderCommond::DEPTH:
            {
                assert(command.data.type() == typeid(OpenGLDepthRenderData));
                GL_LOG_TRACE << "-- [render] DEPTH";
                // es_spec_3.2 - 15.1.3 Depth Buffer Test
                // The depth buffer test discards the incoming fragment if a depth comparison fails.
                const OpenGLDepthRenderData depthData = RefAnyCast<OpenGLDepthRenderData>(command.data);
                if (depthData.enbaled)
                {
                    if (!depthEnabled)
                    {
                        glEnable(GL_DEPTH_TEST);
                        depthEnabled = true;
                    }
                    if (depthData.write != depthMask)
                    {
                        glDepthMask(depthMask);
                        depthMask = depthData.write;
                    }
                    if (depthData.func != (GLenum)depthFunc)
                    {
                        glDepthFunc(depthData.func);
                        depthFunc = depthData.func;
                    }
                }
                else if (/* !depthData.enbaled && */ depthEnabled)
                {
                    glDisable(GL_DEPTH_TEST);
                    depthEnabled = false;
                }
                break;
            }
            case OpenGLRenderCommond::STENCILFUNC:
            {
                assert(command.data.type() == typeid(OpenGLStencilFuncRenderData));
                GL_LOG_TRACE << "-- [render] STENCILFUNC";
                // es_spec_3.2 - 15.1.2 Stencil Test
                // The stencil test conditionally discards a fragment based on the outcome of a com-
                // parison between the value in the stencil buffer at location (xw,yw) and a reference
                // value.
                const OpenGLStencilFuncRenderData& stencilFuncData = RefAnyCast<OpenGLStencilFuncRenderData>(command.data);
                if (stencilFuncData.enabled)
                {
                    if (!stencilEnabled)
                    {
                        glEnable(GL_STENCIL_TEST);
                        stencilEnabled = true;
                    }
                    glStencilFunc(stencilFuncData.func, stencilFuncData.ref, stencilFuncData.compareMask);
                }
                else if (/* !stencilFuncData.enabled && */ stencilEnabled)
                {
                    glDisable(GL_STENCIL_TEST);
                    stencilEnabled = false;
                }
                CHECK_GL_ERROR_IF_DEBUG();
                break;
            }
            case OpenGLRenderCommond::STENCILOP:
            {
                assert(command.data.type() == typeid(OpenGLStencilOpRenderData));
                GL_LOG_TRACE << "-- [render] STENCILOP";
                // es_spec_3.2 - 15.1.2 Stencil Test
                const OpenGLStencilOpRenderData& stencilopData = RefAnyCast<OpenGLStencilOpRenderData>(command.data);
                glStencilOp(stencilopData.sFail, stencilopData.zFail, stencilopData.pass);
                glStencilMask(stencilopData.writeMask);
                break;
            }
            case OpenGLRenderCommond::BLEND:
            {
                assert(command.data.type() == typeid(OpenGLBlendRenderData));
                GL_LOG_TRACE << "-- [render] BLEND";
                // es_spec_3.2 - 15.1.5 Blending
                // Blending combines the incoming source fragment’s R, G, B, and A values with
                // the destination R, G, B, and A values stored in the framebuffer at the fragment’s
                // (xw,yw) location.
                const OpenGLBlendRenderData& blendData = RefAnyCast<OpenGLBlendRenderData>(command.data);
                if (blendData.enabled)
                {
                    if (!blendEnabled)
                    {
                        glEnable(GL_BLEND);
                        blendEnabled = true;
                    }
                    // es_spec_3.2 - Table 15.1: RGB and alpha blend equations.
                    if (blendEqColor != blendData.funcColor || blendEqAlpha != blendData.funcAlpha)
                    {
                        glBlendEquationSeparate(blendData.funcColor, blendData.funcAlpha);
                        blendEqColor = blendData.funcColor;
                        blendEqAlpha = blendData.funcAlpha;
                    }
                    glBlendFuncSeparate(blendData.srcColor, blendData.dstColor, blendData.srcAlpha, blendData.dstAlpha);
                }
                else if (/* !blendData.enabled && */ blendEnabled)
                {
                    glDisable(GL_BLEND);
                    blendEnabled = false;
                }
                if (blendData.mask != colorMask)
                {
                    // es_spec_3.2 - 15.2.2 Fine Control of Buffer Updates
                    // ColorMask and ColorMaski are used to mask the writing of R, G, B and A
                    // values to the draw buffer or buffers. 
                    glColorMask(blendData.mask & 1, (blendData.mask >> 1) & 1, (blendData.mask >> 2) & 1, (blendData.mask >> 3) & 1);
                    colorMask = blendData.mask;
                }
                CHECK_GL_ERROR_IF_DEBUG();
                break;
            }
            case OpenGLRenderCommond::LOGICOP:
            {
                assert(command.data.type() == typeid(OpenGLogicRenderData));
                GL_LOG_TRACE << "-- [render] LOGICOP";
                const OpenGLogicRenderData& logicData = RefAnyCast<OpenGLogicRenderData>(command.data);
                if (logicData.enabled)
                {
                    if (!logicEnabled)
                    {
                        glEnable(GL_COLOR_LOGIC_OP);
                        logicEnabled = true;
                    }
                    if ((GLenum)logicOp != logicData.logicOp)
                    {
                        glLogicOp(logicData.logicOp);
                    }
                }
                else if (logicEnabled)
                {
                    glDisable(GL_COLOR_LOGIC_OP);
                    logicEnabled = false;
                }
                CHECK_GL_ERROR_IF_DEBUG();
                break;
            }
            case OpenGLRenderCommond::CLEAR:
            {
                assert(command.data.type() == typeid(OpenGLClearRenderData));
                GL_LOG_TRACE << "-- [render] CLEAR";
                const OpenGLClearRenderData& clearData = RefAnyCast<OpenGLClearRenderData>(command.data);
                if (clearData.scissorW == 0)
                {
                    // Hint : disable scissor temporary then resume
                    glDisable(GL_SCISSOR_TEST);
                }
                else
                {
                    glScissor(clearData.scissorX, clearData.scissorY, clearData.scissorW, clearData.scissorH);
                }
                if (clearData.colorMask != colorMask)
                {
                    glColorMask(clearData.colorMask & 1, (clearData.colorMask >> 1) & 1, (clearData.colorMask >> 2) & 1, (clearData.colorMask >> 3) & 1);
                }
                if (clearData.clearMask & GL_COLOR_BUFFER_BIT)
                {
                    float color[4];
                    Uint8x4ToFloat(color, clearData.clearColor);
                    // es_spec_3.2 - 15.2.3 Clearing the Buffers
                    // ClearColor sets the clear value for fixed-point and floating-point color buffers.
                    // The specified scomponents are stored as floating-point values.
                    glClearColor(color[0], color[1], color[2], color[3]);
                }
                if (clearData.clearMask & GL_DEPTH_BUFFER_BIT)
                {
                    if (_openGLFeature.IsGLES)
                    {
                        if (glClearDepthf)
                        glClearDepthf(clearData.clearZ);
                    }
                    else
                    {
                        if (glClearDepth)
                        glClearDepth(clearData.clearZ);
                    }
                }
                if (clearData.clearMask & GL_STENCIL_BUFFER_BIT)
                {
                    glClearStencil(clearData.clearStencil);
                }
                glClear(clearData.clearMask);
                // Hint : 某些 gl context 可能没有 default framebuffer (例如 EGL surfaceless 模式), 故强制忽略掉此异常
                IGNORE_GL_ERROR_IF_DEBUG();
                // es_spec_3.2 - Restore the color mask if it was different.
                if (clearData.colorMask != colorMask)
                {
                    glColorMask(colorMask & 1, (colorMask >> 1) & 1, (colorMask >> 2) & 1, (colorMask >> 3) & 1);
                }
                if (clearData.scissorW == 0)
                {
                    glEnable(GL_SCISSOR_TEST);
                }
                CHECK_GL_ERROR_IF_DEBUG();
                break;
            }
            case OpenGLRenderCommond::BLENDCOLOR:
            {
                assert(command.data.type() == typeid(OpenGLBlendColorRenderData));
                GL_LOG_TRACE << "-- [render] BLENDCOLOR";
                const OpenGLBlendColorRenderData& blendColorData = RefAnyCast<OpenGLBlendColorRenderData>(command.data);
                glBlendColor(blendColorData.color[0], blendColorData.color[1], blendColorData.color[2], blendColorData.color[3]);
                break;
            }
            case OpenGLRenderCommond::VIEWPORT:
            {
                assert(command.data.type() == typeid(OpenGLViewportRenderData));
                GL_LOG_TRACE << "-- [render] VIEWPORT";
                // es_spec_3.2 - 12.6 Coordinate Transformations
                const OpenGLViewportRenderData& viewportData = RefAnyCast<OpenGLViewportRenderData>(command.data);
                float y = viewportData.vp.y;
                if (!_currentFrameBuffer)
                {
                    y = _currentFrameBufferHeight - y - viewportData.vp.h;
                }
                // where x and y give the x and y window coordinates of the viewport’s lower left
                // corner and w and h give the viewport’s width and height, respectively. The viewport
                // parameters shown in the above equations are found from these values as
                //      ox = x + w/2
                //      oy = y + h/2
                //      px = w
                //      py = h
                glViewport((GLint)viewportData.vp.x, (GLint)y, (GLsizei)viewportData.vp.w, (GLsizei)(viewportData.vp.h));
                if (_openGLFeature.IsGLES)
                {
                    if (glDepthRangef)
                    glDepthRangef(viewportData.vp.minZ, viewportData.vp.maxZ);
                }
                else
                {
                    if (glDepthRange)
                    glDepthRange(viewportData.vp.minZ, viewportData.vp.maxZ);
                }
                CHECK_GL_ERROR_IF_DEBUG();
                break;
            }
            case OpenGLRenderCommond::SCISSOR:
            {
                assert(command.data.type() == typeid(OpenGLScissorRenderData));
                GL_LOG_TRACE << "-- [render] SCISSOR";
                // es_spec_3.2 - 13.8.2 Scissor Test
                // The scissor test determines if (xw,yw) lies within the scissor rectangle defined by
                // four values for each viewport.
                const OpenGLScissorRenderData& scissorData = RefAnyCast<OpenGLScissorRenderData>(command.data);
                int y = scissorData.rc.y;
                if (!_currentFrameBuffer)
                {
                    // Hint : convert top to bottom, note glScissor second parameter
                    y = _currentFrameBufferHeight - y - scissorData.rc.h;
                }
                glScissor(scissorData.rc.x, y, scissorData.rc.w, scissorData.rc.h);
                CHECK_GL_ERROR_IF_DEBUG();
                break;
            }
            case OpenGLRenderCommond::UNIFORM4F:
            {
                assert(curProgram);
                assert(command.data.type() == typeid(OpenGLUniform4RenderData));
                GL_LOG_TRACE << "-- [render] UNIFORM4F";
                const OpenGLUniform4RenderData& uniform4fData = RefAnyCast<OpenGLUniform4RenderData>(command.data);
                int loc = uniform4fData.loc ? *uniform4fData.loc : -1;
                if (!uniform4fData.name.empty())
                {
                    loc = curProgram->GetUniformLoc(uniform4fData.name);
                }
                if (loc >= 0)
                {
                    switch (uniform4fData.count)
                    {
                        case 1:
                            glUniform1f(loc, uniform4fData.v[0]);
                            break;
                        case 2:
                            glUniform2fv(loc, 1, uniform4fData.v);
                            break;
                        case 3:
                            glUniform3fv(loc, 1, uniform4fData.v);
                            break;
                        case 4:
                            glUniform4fv(loc, 1, uniform4fData.v);
                            break;
                        default: assert(false); break;
                    }
                }
                CHECK_GL_ERROR_IF_DEBUG();
                break;
            }
            case OpenGLRenderCommond::UNIFORM4UI:
            {
                assert(curProgram);
                assert(command.data.type() == typeid(OpenGLUniform4RenderData));
                GL_LOG_TRACE << "-- [render] UNIFORM4UI";
                const OpenGLUniform4RenderData& uniform4fData = RefAnyCast<OpenGLUniform4RenderData>(command.data);
                int loc = uniform4fData.loc ? *uniform4fData.loc : -1;
                if (!uniform4fData.name.empty())
                {
                    loc = curProgram->GetUniformLoc(uniform4fData.name);
                }
                if (loc >= 0)
                {
                    switch (uniform4fData.count)
                    {
                        case 1:
                            glUniform1uiv(loc, 1,  reinterpret_cast<const GLuint*>(uniform4fData.v));
                            break;
                        case 2:
                            glUniform2uiv(loc, 1,  reinterpret_cast<const GLuint*>(uniform4fData.v));
                            break;
                        case 3:
                            glUniform3uiv(loc, 1,  reinterpret_cast<const GLuint*>(uniform4fData.v));
                            break;
                        case 4:
                            glUniform4uiv(loc, 1,  reinterpret_cast<const GLuint*>(uniform4fData.v));
                            break;
                        default: assert(false); break;
                    }
                }
                CHECK_GL_ERROR_IF_DEBUG();
                break;
            }
            case OpenGLRenderCommond::UNIFORM4I:
            {
                assert(curProgram);
                assert(command.data.type() == typeid(OpenGLUniform4RenderData));
                GL_LOG_TRACE << "-- [render] UNIFORM4I";
                const OpenGLUniform4RenderData& uniform4fData = RefAnyCast<OpenGLUniform4RenderData>(command.data);
                int loc = uniform4fData.loc ? *uniform4fData.loc : -1;
                if (!uniform4fData.name.empty())
                {
                    loc = curProgram->GetUniformLoc(uniform4fData.name);
                }
                if (loc >= 0)
                {
                    switch (uniform4fData.count)
                    {
                        case 1:
                            glUniform1iv(loc, 1,  reinterpret_cast<const GLint *>(uniform4fData.v));
                            break;
                        case 2:
                            glUniform2iv(loc, 1,  reinterpret_cast<const GLint *>(uniform4fData.v));
                            break;
                        case 3:
                            glUniform3iv(loc, 1,  reinterpret_cast<const GLint *>(uniform4fData.v));
                            break;
                        case 4:
                            glUniform4iv(loc, 1,  reinterpret_cast<const GLint *>(uniform4fData.v));
                            break;
                        default: assert(false); break;
                    }
                }
                CHECK_GL_ERROR_IF_DEBUG();
                break;
            }
            case OpenGLRenderCommond::UNIFORMSTEREOMATRIX:
            {
                assert(curProgram);
                assert(command.data.type() == typeid(OpenGLUniformMatrix4RenderData));
                GL_LOG_TRACE << "-- [render] UNIFORMSTEREOMATRIX";
                const OpenGLUniformMatrix4RenderData& uniformMatrixData = RefAnyCast<OpenGLUniformMatrix4RenderData>(command.data);
                int loc = uniformMatrixData.loc ? *uniformMatrixData.loc : -1;
                if (!uniformMatrixData.name.empty())
                {
                    loc = curProgram->GetUniformLoc(uniformMatrixData.name);
                }
                if (loc >= 0)
                {
                    glUniformMatrix4fv(loc, 1, false, uniformMatrixData.m);
                }
                CHECK_GL_ERROR_IF_DEBUG();
                break;
            }
            case OpenGLRenderCommond::UNIFORMMATRIX:
            {
                assert(curProgram);
                assert(command.data.type() == typeid(OpenGLUniformMatrix4RenderData));
                GL_LOG_TRACE << "-- [render] UNIFORMMATRIX";
                const OpenGLUniformMatrix4RenderData& uniformMatrixData = RefAnyCast<OpenGLUniformMatrix4RenderData>(command.data);
                int loc = uniformMatrixData.loc ? *uniformMatrixData.loc : -1;
                if (!uniformMatrixData.name.empty())
                {
                    loc = curProgram->GetUniformLoc(uniformMatrixData.name);
                }
                if (loc >= 0)
                {
                    glUniformMatrix4fv(loc, 1, false, uniformMatrixData.m);
                }
                CHECK_GL_ERROR_IF_DEBUG();
                break;
            }
            case OpenGLRenderCommond::BINDTEXTURE:
            {
                assert(command.data.type() == typeid(OpenGLTextureRenderData));
                const OpenGLTextureRenderData& textureData = RefAnyCast<OpenGLTextureRenderData>(command.data);
                GL_LOG_TRACE << "-- [render] BINDTEXTURE";
                GLint slot = textureData.solt;
                OpenGLRenderTexture::ptr texture = textureData.texture;
                if ((int)slot != activeSlot)
                {
                    glActiveTexture(GL_TEXTURE0 + slot);
                    activeSlot = slot;
                }
                if (texture)
                {
                    if (curTex[slot] != texture)
                    {
                        glBindTexture(texture->target, texture->texture);
                        curTex[slot] = texture;
                    }
                }
                else
                {
                    // TODO : specify target according to the cache
                    glBindTexture(GL_TEXTURE_2D, 0);
                    curTex[slot] = nullptr;
                }
                CHECK_GL_ERROR_IF_DEBUG();
                break;
            }
            case OpenGLRenderCommond::BIND_FB_TEXTURE:
            {
                assert(command.data.type() == typeid(OpenGLBindFbTextureRenderData));
                const OpenGLBindFbTextureRenderData& bindFbTextureData = RefAnyCast<OpenGLBindFbTextureRenderData>(command.data);
                GL_LOG_TRACE << "-- [render] BIND_FB_TEXTURE";
                OpenGLRenderFrameBuffer::ptr fb = bindFbTextureData.framebuffer;
                GLuint slot = bindFbTextureData.slot;
                if ((int)slot != activeSlot)
                {
                    glActiveTexture(GL_TEXTURE0 + slot);
                    activeSlot = slot;
                }
                if (bindFbTextureData.aspect == GL_COLOR_BUFFER_BIT)
                {
                    if (curTex[slot] != fb->colorTextures[0])
                    {
                        glBindTexture(GL_TEXTURE_2D, fb->colorTextures[0]->texture);
                        curTex[slot] = fb->colorTextures[0];
                    }
                }
                else if (bindFbTextureData.aspect == GL_DEPTH_BUFFER_BIT)
                {
                    if (curTex[slot] != fb->zStencilTexture)
                    {
                        glBindTexture(GL_TEXTURE_2D, fb->zStencilTexture->texture);
                        curTex[slot] = fb->zStencilTexture;
                    }
                    glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_STENCIL_TEXTURE_MODE, GL_DEPTH_COMPONENT);
                }
                else if (bindFbTextureData.aspect == GL_STENCIL_BUFFER_BIT)
                {
                    if (curTex[slot] != fb->zStencilTexture)
                    {
                        glBindTexture(GL_TEXTURE_2D, fb->zStencilTexture->texture);
                        curTex[slot] = fb->zStencilTexture;
                    }
                    glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_STENCIL_TEXTURE_MODE, GL_STENCIL_INDEX);
                }
                else
                {
                    curTex[slot] = nullptr;
                }
                CHECK_GL_ERROR_IF_DEBUG();
                break;
            }
            case OpenGLRenderCommond::BINDPROGRAM:
            {
                assert(command.data.type() == typeid(OpenGLProgramRenderData));
                const OpenGLProgramRenderData& bindProgramData = RefAnyCast<OpenGLProgramRenderData>(command.data);
                GL_LOG_TRACE << "-- [render] BINDPROGRAM";
                OpenGLRenderProgram::ptr program = bindProgramData.program;
                if (curProgram != program)
                {
                    curProgram = program;
                    glUseProgram(program->program);
                    for (size_t i=0; i<clipDistanceArraySize; i++)
                    {
                        if (program->use_clip_distance[i] == clipDistanceEnabled[i])
                            continue;
                        
                        if (program->use_clip_distance[i])
                        {
                            glEnable(GL_CLIP_DISTANCE0 + (GLenum) i);
                        }
                        else
                        {
                            glDisable(GL_CLIP_DISTANCE0 + (GLenum) i);
                        }
                        clipDistanceEnabled[i] = program->use_clip_distance[i];
                    }
                }
                CHECK_GL_ERROR_IF_DEBUG();
                break;
            }
            case OpenGLRenderCommond::BIND_VERTEX_BUFFER:
            {
                assert(command.data.type() == typeid(OpenGLBindVertexBufferRenderData));
                const OpenGLBindVertexBufferRenderData& bindVertexbufferData = RefAnyCast<OpenGLBindVertexBufferRenderData>(command.data);
                GL_LOG_TRACE << "-- [render] BIND_VERTEX_BUFFER";
                OpenGLRenderInputLayout::ptr inputLayout = bindVertexbufferData.inputLayout;
                OpenGLRenderBuffer::ptr      buffer      = bindVertexbufferData.buffer;
                if (!buffer || buffer->Mapped())
                {
                    assert(false);
                }
                GLuint buf = buffer ? buffer->buffer : 0;
                if (buf != curArrayBuffer)
                {
                    glBindBuffer(GL_ARRAY_BUFFER, buf);
                    curArrayBuffer = buf;
                }
                if (attrMask != inputLayout->semanticsMask)
                {
                    int enable  = inputLayout->semanticsMask & ~attrMask;
                    int disable = (~inputLayout->semanticsMask) & attrMask;
                    // TODO : refine it use a more comfortable way
                    // see aslo : enum class Semantic
                    for (int i = 0; i < 7 /* SEM_MAX */; i++) 
                    {
                        if (enable & (1 << i)) 
                        {
                            glEnableVertexAttribArray(i);
                        }
                        if (disable & (1 << i)) 
                        {
                            glDisableVertexAttribArray(i);
                        }
                    }
                    attrMask = inputLayout->semanticsMask;
                }
                for (size_t i=0; i<inputLayout->entries.size(); i++)
                {
                    auto& entry = inputLayout->entries[i];
                    // Hint : 
                    // es_spec_3.2.pdf - 10.3.1 Specifying Arrays for Generic Vertex Attributes
                    // (1) normalized means that either normalize or cast
                    // behavior applies, as described below, depending on whether the normalized flag
                    // to the command is TRUE or FALSE, respectively. normalize means that values are
                    // converted to floating-point by normalizing to [0,1] (for unsigned types) or [−1,1]
                    // (for signed types)
                    // (2) type specifies the data type of the values stored in the array. (see also Table 10.3)
                    // (3) stride : Pointers to the ith and (i + 1)st elements of the array 
                    // differ by stride basic machine units
                    // es_spec_3.2.pdf - 10.3.7 Vertex Arrays in Buffer Objects (for pointer parameter, this way is more efficient)
                    glVertexAttribPointer(entry.location, entry.count, entry.type, entry.normalized,
                        entry.stride, reinterpret_cast<const void*>(bindVertexbufferData.offset + entry.offset)
                    );
                }
                CHECK_GL_ERROR_IF_DEBUG();
                break;
            }
            case OpenGLRenderCommond::BIND_BUFFER:
            {
                assert(command.data.type() == typeid(OpenGLBindBufferRenderData));
                const OpenGLBindBufferRenderData& bindBufferData = RefAnyCast<OpenGLBindBufferRenderData>(command.data);
                GL_LOG_TRACE << "-- [render] BIND_BUFFER";
                if (bindBufferData.target == GL_ARRAY_BUFFER)
                {
                    assert(false);
                }
                else if (bindBufferData.target == GL_ELEMENT_ARRAY_BUFFER)
                {
                    GLuint buf = bindBufferData.buffer ? bindBufferData.buffer->buffer : 0;
                    if (bindBufferData.buffer && bindBufferData.buffer->Mapped())
                    {
                        assert(false);
                    }
                    if (buf != curElemArrayBuffer)
                    {
                        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf);
                        curElemArrayBuffer = buf;
                    }
                }
                else
                {
                    GLuint buf = bindBufferData.buffer ? bindBufferData.buffer->buffer : 0;
                    if (bindBufferData.buffer && bindBufferData.buffer->Mapped())
                    {
                        assert(false);
                    }
                    glBindBuffer(bindBufferData.target, buf);                    
                }
                CHECK_GL_ERROR_IF_DEBUG();
            }
            case OpenGLRenderCommond::GENMIPS:
            {
                GL_LOG_TRACE << "-- [render] GENMIPS";
                glGenerateMipmap(GL_TEXTURE_2D);
                break;
            }
            case OpenGLRenderCommond::DRAW:
            {
                assert(command.data.type() == typeid(OpenGLDrawRenderData));
                const OpenGLDrawRenderData& drawData = RefAnyCast<OpenGLDrawRenderData>(command.data);
                GL_LOG_TRACE << "-- [render] DRAW";
                // TODO:
                // try to find a way to trace draw vertex, may be a method describe in 
                // es_spec_3.2 - 12.2.2 Transform Feedback Primitive Capture
                // (for debug purpose)
                // See aslo :
                // (1) es_spec_3.2 - Chapter 10 Vertex Specification and Drawing Commands
                //     Most geometric primitives are drawn by specifying a series of generic attribute sets
                //     corresponding to the vertices of a primitive using DrawArrays or one of the other
                //     drawing commands defined in section 10.5. Points, lines, polygons, and a variety
                //     of related geometric primitives (see section 10.1) can be drawn in this way.
                // (2) es_spec_3.2 - Figure 10.1. Vertex processing and primitive assembly.
                // (3) es_spec_3.2 - 10.5 Drawing Commands Using Vertex Arrays
                glDrawArrays(drawData.mode, drawData.frist, drawData.count);
                break;
            }
            case OpenGLRenderCommond::DRAW_INDEXED:
            {
                assert(command.data.type() == typeid(OpenGLDrawIndexedRenderData));
                GL_LOG_TRACE << "-- [render] DRAW_INDEXED";
                const OpenGLDrawIndexedRenderData& drawIndexData = RefAnyCast<OpenGLDrawIndexedRenderData>(command.data);
                if (drawIndexData.instances == 1)
                {
                    glDrawElements(drawIndexData.mode, drawIndexData.count, drawIndexData.indexType, drawIndexData.indices);
                }
                else
                {
                    glDrawElementsInstanced(drawIndexData.mode, drawIndexData.count, drawIndexData.indexType, drawIndexData.indices, drawIndexData.instances);
                }
                CHECK_GL_ERROR_IF_DEBUG();
                break;
            }
            case OpenGLRenderCommond::TEXTURESAMPLER:
            {
                assert(command.data.type() == typeid(OpenGLTextureSamplerRenderData));
                GL_LOG_TRACE << "-- [render] TEXTURESAMPLER";
                const OpenGLTextureSamplerRenderData& textureSamplerData = RefAnyCast<OpenGLTextureSamplerRenderData>(command.data);
                CHECK_GL_ERROR_IF_DEBUG();
                GLint slot = textureSamplerData.slot;
                if ((int)slot != activeSlot)
                {
                    glActiveTexture(GL_TEXTURE0 + slot);
                    activeSlot = slot;
                }
                OpenGLRenderTexture::ptr tex = curTex[slot];
                if (!tex)
                {
                    break;
                }
                CHECK_GL_ERROR_IF_DEBUG();
                if (tex->canWrap)
                {
                    if (tex->wrapS != textureSamplerData.wrapS)
                    {
                        glTexParameteri(tex->target, GL_TEXTURE_WRAP_S, textureSamplerData.wrapS);
                        tex->wrapS = textureSamplerData.wrapS;
                    }
                    if (tex->wrapT != textureSamplerData.wrapT)
                    {
                        glTexParameteri(tex->target, GL_TEXTURE_WRAP_T, textureSamplerData.wrapT);
                        tex->wrapT = textureSamplerData.wrapT;
                    }
                }
                CHECK_GL_ERROR_IF_DEBUG();
                if (tex->magFilter != textureSamplerData.magFilter)
                {
                    glTexParameteri(tex->target, GL_TEXTURE_MAG_FILTER, textureSamplerData.magFilter);
                    tex->magFilter = textureSamplerData.magFilter;
                }
                CHECK_GL_ERROR_IF_DEBUG();
                if (tex->minFilter != textureSamplerData.minFilter)
                {
                    glTexParameteri(tex->target, GL_TEXTURE_MIN_FILTER, textureSamplerData.minFilter);
                }
                CHECK_GL_ERROR_IF_DEBUG();
                if (tex->anisotropy != textureSamplerData.anisotropy)
                {
                    if (textureSamplerData.anisotropy != 0.0f)
                    {
                        glTexParameterf(tex->texture, GL_TEXTURE_MAX_ANISOTROPY_EXT, textureSamplerData.anisotropy);
                    }
                    tex->anisotropy = textureSamplerData.anisotropy;
                }
                break;
            }
            case OpenGLRenderCommond::TEXTURELOD:
            {
                assert(command.data.type() == typeid(OpenGLTextureLodRenderData));
                GL_LOG_TRACE << "-- [render] TEXTURELOD";
                const OpenGLTextureLodRenderData& textureLodData = RefAnyCast<OpenGLTextureLodRenderData>(command.data);
                GLint slot = textureLodData.slot;
                if ((int)slot != activeSlot)
                {
                    glActiveTexture(GL_TEXTURE0 + slot);
                    activeSlot = slot;
                }
                OpenGLRenderTexture::ptr tex = curTex[slot];
                if (!tex)
                {
                    break;
                }
                if (tex->lodBias != textureLodData.lodBias && !_openGLFeature.IsGLES)
                {
                    glTexParameterf(tex->target, GL_TEXTURE_LOD_BIAS, textureLodData.lodBias);
                    tex->lodBias = textureLodData.lodBias;
                }
                if (tex->minLod != textureLodData.minLod)
                {
                    glTexParameterf(tex->target, GL_TEXTURE_MIN_LOD, textureLodData.minLod);
                    tex->minLod = textureLodData.minLod;
                }
                if (tex->maxLod != textureLodData.maxLod)
                {
                    glTexParameterf(tex->target, GL_TEXTURE_MAX_LOD, textureLodData.maxLod);
                    tex->maxLod = textureLodData.maxLod;
                }
                break;
            }
            case OpenGLRenderCommond::TEXTURE_SUBIMAGE:
            {
                assert(command.data.type() == typeid(OpenGLTextureSubImageRenderData));
                GL_LOG_TRACE << "-- [render] TEXTURE_SUBIMAGE";
                const OpenGLTextureSubImageRenderData& textureSubimageData = RefAnyCast<OpenGLTextureSubImageRenderData>(command.data);
                GLint slot = textureSubimageData.slot;
                if ((int)slot != activeSlot)
                {
                    glActiveTexture(GL_TEXTURE0 + slot);
                    activeSlot = slot;
                }
                OpenGLRenderTexture::ptr tex = textureSubimageData.texture;
                if (textureSubimageData.data->GetSize() == 0 || tex->target != GL_TEXTURE_2D)
                {
                    assert(false);
                }
                GLuint internalFormat, format, type;
                int alignment;
                DataFormatToGLFormatAndType(textureSubimageData.format, _openGLFeature, internalFormat, format, type, alignment);
                glTexSubImage2D(tex->target, textureSubimageData.level, textureSubimageData.x, textureSubimageData.y,
                    textureSubimageData.width, textureSubimageData.height, format, type, 
                    textureSubimageData.data->GetData()
                );
                CHECK_GL_ERROR_IF_DEBUG();
                break;
            }
            case OpenGLRenderCommond::RASTER:
            {
                assert(command.data.type() == typeid(OpenGLRasterRenderData));
                GL_LOG_TRACE << "-- [render] RASTER";
                const OpenGLRasterRenderData& rasterData = RefAnyCast<OpenGLRasterRenderData>(command.data);
                if (rasterData.cullEnable)
                {
                    if (!cullEnabled)
                    {
                        glEnable(GL_CULL_FACE);
                        cullEnabled = true;
                    }
                    glFrontFace(rasterData.frontFace);
                    glCullFace(rasterData.cullFace);
                }
                else if (/* !rasterData.cullEnable &&*/ cullEnabled)
                {
                    glDisable(GL_CULL_FACE);
                    cullEnabled = false;
                }
                if (rasterData.ditherEnable)
                {
                    if (!ditherEnabled)
                    {
                        glEnable(GL_DITHER);
                        ditherEnabled = true;
                    }
                }
                else if (/* !rasterData.ditherEnable &&*/ ditherEnabled)
                {
                    glDisable(GL_DITHER);
                    ditherEnabled = false;
                }
                if (rasterData.depthClampEnable)
                {
                    if (!depthClampEnabled)
                    {
                        glEnable(GL_DEPTH_CLAMP);
                        depthClampEnabled = true;
                    }
                }
                else if (/* !rasterData.depthClampEnable && */ depthClampEnabled)
                {
                    glDisable(GL_DEPTH_CLAMP);
                    depthClampEnabled = false;
                }
                CHECK_GL_ERROR_IF_DEBUG();
                break;
            }
            default:
                GL_LOG_WARN << "Unknown render command type, command type is: " << command.cmd;
                assert(false);
                break;
        }
    }

	for (int i = 0; i < 7; i++) 
    {
		if (attrMask & (1 << i)) 
        {
			glDisableVertexAttribArray(i);
		}
	}

	if (activeSlot != 0) 
    {
		glActiveTexture(GL_TEXTURE0);
		activeSlot = 0;
	}
    CHECK_GL_ERROR_IF_DEBUG();

    // Wipe out the current state.
	if (curArrayBuffer != 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
	if (curElemArrayBuffer != 0)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
	if (isLast && _openGLFeature.ARB_vertex_array_object) 
    {
		glBindVertexArray(0);
	}
	if (isLast)
    {
        glDisable(GL_SCISSOR_TEST);
    }
	if (depthEnabled)
    {
        glDisable(GL_DEPTH_TEST);
    }
	if (stencilEnabled)
    {
        glDisable(GL_STENCIL_TEST);
    }
	if (blendEnabled)
    {
        glDisable(GL_BLEND);
    }
	if (cullEnabled)
    {
        glDisable(GL_CULL_FACE);
    }
	if (depthClampEnabled)
    {
        glDisable(GL_DEPTH_CLAMP);
    }
	if (!_openGLFeature.IsGLES && logicEnabled) 
    {
		glDisable(GL_COLOR_LOGIC_OP);
	}
	for (size_t i = 0; i < clipDistanceArraySize; ++i) 
    {
		if (clipDistanceEnabled[i])
        {
            glDisable(GL_CLIP_DISTANCE0 + (GLenum)i);
        }
	}
	if ((colorMask & 15) != 15)
    {
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }
    CHECK_GL_ERROR_IF_DEBUG();
}

void OpenGL::RenderStepCopy(OpenGLRenderStep& step)
{
    assert(step.data.type() == typeid(OpenGLCopyData));
    const OpenGLCopyData& copyData = RefAnyCast<OpenGLCopyData>(step.data);

    GL_LOG_TRACE << "[render step] copy";

    CHECK_GL_ERROR_IF_DEBUG();

    GLuint srcTex  = 0;
    GLuint dstTex  = 0;
    GLuint target  = GL_TEXTURE_2D;

    const GLRect2D&    srcRect     = copyData.srcRect;
    const OpenGLOffset2D&  dstPos  = copyData.dstPos;

    OpenGLRenderFrameBuffer::ptr     src     = copyData.src;
    OpenGLRenderFrameBuffer::ptr     dst     = copyData.dst;

    int  srcLevel = 0;
    int  dstLevel = 0;
    int  srcZ     = 0;
    int  dstZ     = 0;
    int  depth    = 1;

    switch (copyData.aspectMask)
    {
        case GL_COLOR_BUFFER_BIT:
        {
            srcTex = src->colorTextures[0]->texture;
            dstTex = dst->colorTextures[0]->texture;
            break;
        }
        case GL_DEPTH_BUFFER_BIT: /* not support for now, pass through */
            GL_LOG_ERROR << "GL_DEPTH_BUFFER_BIT not yet supported";
        default:
            assert(false); 
        break;
    }

    assert(srcTex);
    assert(dstTex);

    if (_openGLFeature.ARB_copy_image)
    {
        glCopyImageSubData(
            srcTex, target, srcLevel, srcRect.x, srcRect.y, srcZ,
            dstTex, target, dstLevel, dstPos.x, dstPos.y, dstZ,
            srcRect.w, srcRect.h, depth
        );
    }
    else if (_openGLFeature.NV_copy_image)
    {
		glCopyImageSubDataNV(
			srcTex, target, srcLevel, srcRect.x, srcRect.y, srcZ,
			dstTex, target, dstLevel, dstPos.x, dstPos.y, dstZ,
			srcRect.w, srcRect.h, depth);
    }

    CHECK_GL_ERROR_IF_DEBUG();
}

void OpenGL::RenderStepBlit(OpenGLRenderStep& step)
{
    assert(step.data.type() == typeid(OpenGLBlitData));
    const OpenGLBlitData& blitData = RefAnyCast<OpenGLBlitData>(step.data);

    GL_LOG_TRACE << "[render step] blit";

    CHECK_GL_ERROR_IF_DEBUG();

    FBOBindFBTarget(false, blitData.dst->handle);
    FBOBindFBTarget(true, blitData.src->handle);

    int srcX1 = blitData.srcRect.x;
    int srcY1 = blitData.srcRect.y;
    int srcX2 = blitData.srcRect.x + blitData.srcRect.w;
    int srcY2 = blitData.srcRect.y + blitData.srcRect.h;
    int dstX1 = blitData.dstRect.x;
    int dstY1 = blitData.dstRect.y;
    int dstX2 = blitData.dstRect.x + blitData.dstRect.w;
    int dstY2 = blitData.dstRect.y + blitData.dstRect.h;

    if (_openGLFeature.GLES3 || _openGLFeature.ARB_framebuffer_object)
    {
        glBlitFramebuffer(srcX1, srcY1, srcX2, srcY2, dstX1, dstY1, dstX2, dstY2, blitData.aspectMask, blitData.filter ? GL_LINEAR : GL_NEAREST);
        CHECK_GL_ERROR_IF_DEBUG();
    }
    else
    {
        GL_LOG_ERROR << "Tried to blit without the capability";
    }
}

void OpenGL::RenderStepReadback(OpenGLRenderStep& step)
{
    assert(step.data.type() == typeid(OpenGLReadbackData));
    const OpenGLReadbackData& readbackData = RefAnyCast<OpenGLReadbackData>(step.data);

    GL_LOG_TRACE << "[render step] readback";

    OpenGLRenderFrameBuffer::ptr fb = readbackData.src;

    FBOBindFBTarget(true, fb ? fb->handle : 0);

    if (fb && (_openGLFeature.GLES3 || _openGLFeature.IsGLES))
    {
        glReadBuffer(GL_COLOR_ATTACHMENT0);
    }

    CHECK_GL_ERROR_IF_DEBUG();

    // Always read back in 8888 format for the color aspect.
    GLuint internalFormat = GL_RGBA;
    GLuint format         = GL_RGBA;
    GLuint type           = GL_UNSIGNED_BYTE;
    int    srcAlignment   = 4;

    if (readbackData.aspectMask & GL_DEPTH_BUFFER_BIT)
    {
        internalFormat = GL_DEPTH_COMPONENT;
        format         = GL_DEPTH_COMPONENT;
        type           = GL_FLOAT;
        srcAlignment   = 4;
    }
    else if (readbackData.aspectMask & GL_STENCIL_BUFFER_BIT)
    {
        internalFormat = GL_STENCIL_INDEX;
        format         = GL_STENCIL_INDEX;
        type           = GL_UNSIGNED_BYTE;
        srcAlignment   = 1;
    }
    // Hint : for no warning
    internalFormat = internalFormat;

    _readBackAspectMask = readbackData.aspectMask;

    int pixelStride = readbackData.srcRect.w;
    glPixelStorei(GL_PACK_ALIGNMENT, srcAlignment);
    // Apply the correct alignment.
    if (!_openGLFeature.IsGLES || _openGLFeature.GLES3)
    {
        glPixelStorei(GL_PACK_ROW_LENGTH, pixelStride);
    }

    GLRect2D rect = readbackData.srcRect;

    // TODO : 通过查阅资料确认 glReadPixels 为耗时且阻塞接口
    //        进行过相关资料的查阅, PBO 可以提高回写效率,但是实际测试却发现使用 PBO 的效率还不如 glReadPixels, 是因为 OpenGL ES 版本问题还是芯片平台的差异???
    //        参考资料 : [OpenGL ES —— PBO 使用](https://blog.csdn.net/cgwang_1580/article/details/110992323)
    //        测试平台 : Mali G610, 1080P 情况下 PBO 耗时 8ms, glReadPixels 耗时 2 ms, 60Hz 回写 GPU 使用率达到百分之十五左右

    /**
     * @brief 获取像素内存指针
     */
    auto getPixelsPtr = [&]() -> void*
    {
        size_t readBackSize  = srcAlignment * rect.w * rect.h;
        if (readbackData.pixel)
        {
            // TODO : check pixel format, only support RGBA8888
            if ((size_t)readbackData.pixel->GetSize() < readBackSize)
            {
                GL_LOG_ERROR << "Custom readback buffer size is: " << readbackData.pixel->GetSize() 
                            << ", require readback buffer size is: " << readBackSize
                            << ", can not readback";
                assert(false);
                return nullptr;
            }
            else
            {
                return readbackData.pixel->GetData();
            }
        }
        else
        {
            if (readBackSize > _readBackBufferSize)
            {
                delete[] _readBackBuffer;
                _readBackBuffer = new uint8_t[readBackSize];
                _readBackBufferSize = readBackSize;
            }
            return _readBackBuffer;
        }
    };

    uint8_t* pixels = (uint8_t *)getPixelsPtr();
    if (pixels)
    {
        glReadPixels(rect.x, rect.y, rect.w, rect.h, format, type, pixels);
    }
    else
    {
        assert(false);
    }

    if (!_openGLFeature.IsGLES || _openGLFeature.GLES3)
    {
        glPixelStorei(GL_PACK_ROW_LENGTH, 0);
    }

    CHECK_GL_ERROR_IF_DEBUG();
}

void OpenGL::RenderStepReadbackImage(OpenGLRenderStep& step)
{
    assert(step.data.type() == typeid(OpenGLReadbackImageData));
    const OpenGLReadbackImageData& readBackImageData = RefAnyCast<OpenGLReadbackImageData>(step.data);

    GL_LOG_TRACE << "[render step] readback image";

    std::vector<OpenGLRenderTexture::ptr> texs   = readBackImageData.textures;
    std::vector<GLRect2D>   rects  = readBackImageData.srcRects;

    /**
     * @brief 获取像素内存指针
     */
    auto getPixelsPtr = [&](uint64_t size, uint64_t offset) -> void*
    {    
        if (readBackImageData.picture)
        {
            if ((size_t)readBackImageData.picture->GetSize() < size + offset)
            {
                GL_LOG_ERROR << "Custom readback image buffer size is: " << readBackImageData.picture->GetSize() 
                            << ", require readback buffer size is: " << size + offset
                            << ", can not readback";
                assert(false);
                return nullptr;
            }
            else
            {
                return readBackImageData.picture->GetData(offset);
            }
        }
        else
        {
            if (size + offset > _readBackBufferSize)
            {
                uint8_t* tmp = new uint8_t[(size_t)(size + offset)];
                memcpy(tmp, _readBackBuffer, _readBackBufferSize);
                delete[] _readBackBuffer;
                _readBackBuffer = tmp;
                _readBackBufferSize = (size_t)(size + offset);
            }
            return _readBackBuffer + offset;
        }
    };

    uint64_t offset = 0;
    for (size_t i=0; i< texs.size(); i++)
    {
        auto tex = texs[i];
        GLenum internalFormat, format, type; 
        int alignment;
        DataFormatToGLFormatAndType(tex->format, _openGLFeature, internalFormat, format, type, alignment);
        GLRect2D   _rect = rects[i];
        size_t readBackSize  = alignment * (_rect.w - _rect.x)  * (_rect.h - _rect.y);

        uint8_t* pixels = (uint8_t *)getPixelsPtr(readBackSize, offset);
        if (!pixels)
        {
            assert(false);
            return;
        }
        // TODO : OpenGL 尝试使用 glGetTextureSubImage, 报错 GL_INVALID_VALUE, 未找到具体原因
        // if (OpenGLVersionGreater(_openGLFeature, 4, 5))
        // {
            // ...
        // }
        // WORKAROUND : 在某一 MAIL 芯片上使用 PBO 出现内存泄露,目前暂不确定是否是驱动程序问题,
        //              回滚至非 PBO 读取 FrameBuffer 模式
        if (_openGLFeature.gpuVendor == GpuVendor::ARM /* &&  _openGLFeature.IsGLES */)
        {
            GLuint fbo;
            glGenFramebuffers(1, &fbo);
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex->texture, 0);
            glViewport(0, 0, tex->w, tex->h);
            glReadPixels(_rect.x, _rect.y, _rect.w, _rect.h, format, type, pixels);
            CHECK_GL_ERROR_IF_DEBUG();
            glDeleteFramebuffers(1, &fbo);
            CHECK_GL_ERROR_IF_DEBUG();

        }
        else if (OpenGLVersionGreater(_openGLFeature, 4, 1) || _openGLFeature.IsGLES)
        {
            // Hint : OpenGL ES 使用 PBO 相比于直接使用 glReadPixels 效率会好一点

            GLuint fbo;
            GLuint pbo;

            glGenFramebuffers(1, &fbo);
            glGenBuffers(1, &pbo);
            glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo);
            glBufferData(GL_PIXEL_PACK_BUFFER, (_rect.w - _rect.x) * (_rect.h - _rect.y) * alignment, nullptr, GL_STATIC_READ);
            CHECK_GL_ERROR_IF_DEBUG();

            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex->texture, 0);
            glViewport(0, 0, tex->w, tex->h);
            glReadPixels(_rect.x, _rect.y, _rect.w, _rect.h, format, type, nullptr);
            CHECK_GL_ERROR_IF_DEBUG();

            uint8_t* data = reinterpret_cast<uint8_t*>(glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, (_rect.w - _rect.x) * (_rect.h - _rect.y) * alignment, GL_MAP_READ_BIT));
            CHECK_GL_ERROR_IF_DEBUG();
            if (!data)
            {
                assert(false);
                GL_LOG_WARN << "glMapBufferRange fail, read texture image fail";
            }
            else
            {
                memcpy((uint8_t *)pixels, data, (_rect.w - _rect.x) * (_rect.h - _rect.y) * alignment);
            }
            glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
            glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
            glDeleteBuffers(1, &pbo);
            glDeleteFramebuffers(1, &fbo);
            CHECK_GL_ERROR_IF_DEBUG();
        }
        else
        {
            assert(false);
            GL_LOG_WARN << "Unsupport operation, please update GL version (more than OpenGL 4.5)";
            return;
        }
        offset += readBackSize;
    }

    CHECK_GL_ERROR_IF_DEBUG();
}

} // namespace Mmp