cmake_minimum_required(VERSION 3.8)

option(ENABLE_OPENGL_DEBUG "OpenGL(ES) Debug Mode" ON)

set(GL_SRCS)
set(GL_INCS)
set(GL_LIB_DIR)
set(GL_LIBS)

list(APPEND GL_INCS
    ${MMP_CORE_PRO_TOP_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

list(APPEND GL_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/GLCommon.h
    ${CMAKE_CURRENT_SOURCE_DIR}/GLCommon.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GLContexDesc.h
    ${CMAKE_CURRENT_SOURCE_DIR}/GLContexDesc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GLContex.h
    ${CMAKE_CURRENT_SOURCE_DIR}/GLContex.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GLDrawContex.h
    ${CMAKE_CURRENT_SOURCE_DIR}/GLDrawContex.cpp
)

list(APPEND GL_LIBS
    Poco::Foundation 
    Mmp::Common 
    Mmp::SL
)

if (USE_OPENGL)
    # see also : https://cmake.org/cmake/help/latest/module/FindOpenGL.html
    if (NOT OPENGL_FOUND) # 交叉编译时需外部设置
        if (CMAKE_HOST_UNIX)
            set(OpenGL_GL_PREFERENCE GLVND)
        endif()
        find_package(OpenGL REQUIRED)
    endif()
    list(APPEND GL_INCS 
        ${OPENGL_INCLUDE_DIR}
        # TODO : a better way (may be like poco)
        ${MMP_CORE_PRO_TOP_DIR}/Extension/GLEW/include
    )
    list(APPEND GL_LIB_DIR ${OpenGL_LIBRARY_DIRS})
    # TODO : a better way
    if (NOT MSVC)
        list(APPEND GL_LIBS Mmp::GLEW ${OPENGL_LIBRARIES})
    else()
    list(APPEND GL_LIBS Mmp::GLEW ${OPENGL_LIBRARIES})
    endif()
    message("-- Found OPENGL")
    message("  -- OpenGL INCS : ${OPENGL_INCLUDE_DIR}")
    message("  -- OpenGL LIB DIR : ${OpenGL_LIBRARY_DIRS}")
    message("  -- OpenGL LIBS : ${OPENGL_LIBRARIES}")
endif()


if (USE_OPENGL)
    list(APPEND GL_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/OpenGL.h
        ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/OpenGL.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/OpenGLInitStep.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/OpenGLFrameBuffer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/OpenGLRenderStep.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/OpenGLCommon.h 
        ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/OpenGLCommon.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/OpenGLContex.h
        ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/OpenGLContex.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/OpenGLDrawContex.h
        ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/OpenGLDrawContex.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/OpenGLRenderContex.h
        ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/OpenGLRenderContex.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/OpenGLFeature.h
        ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/OpenGLFeature.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/OpenGLUtil.h
        ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/OpenGLUtil.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/OpenGLRenderData.h
        ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/OpenGLInitStepData.h
        ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/OpenGLStepData.h
        ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/OpenGLTranslator.h
        ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/OpenGLTranslator.cpp
    )
endif()

if (USE_D3D11)
    list(APPEND GL_INCS 
        ${MMP_CORE_PRO_TOP_DIR}/Extension/DirectX/Include
    )
endif()


if (USE_D3D11)
    list(APPEND GL_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/D3D11/D3D11Common.h
        ${CMAKE_CURRENT_SOURCE_DIR}/D3D11/D3D11Common.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/D3D11/D3D11Translator.h
        ${CMAKE_CURRENT_SOURCE_DIR}/D3D11/D3D11Translator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/D3D11/D3D11Contex.h
        ${CMAKE_CURRENT_SOURCE_DIR}/D3D11/D3D11Contex.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/D3D11/D3D11DrawContex.h
        ${CMAKE_CURRENT_SOURCE_DIR}/D3D11/D3D11DrawContex.cpp
    )
endif()

if (USE_VULKAN)
    list(APPEND GL_INCS
        ${MMP_CORE_PRO_TOP_DIR}/Extension/Vulkan-Headers/include
    )
endif()

if (USE_VULKAN)
    list(APPEND GL_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/VulkanCommon.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/VulkanCommon.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/VulkanStepData.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/VulkanStepData.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/VulkanRenderData.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/VulkanRenderData.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/VulkanImpl.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/VulkanImpl.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/VulkanInterface.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/VulkanInterface.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/VulkanRenderContex.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/VulkanRenderContex.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/VulkanBarrier.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/VulkanBarrier.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/VulkanImage.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/VulkanImage.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/VulkanTranslator.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/VulkanTranslator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/VulkanContex.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/VulkanContex.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/VulkanDrawContex.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/VulkanDrawContex.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/VulkanFrameBuffer.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/VulkanFrameBuffer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/VulkanRenderPass.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/VulkanRenderPass.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/VulkanUtil.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/VulkanUtil.cpp
    )
endif()

add_library(MMP_GL STATIC ${GL_SRCS})
add_library(Mmp::GL ALIAS MMP_GL)
if (USE_OPENGL)
    target_compile_definitions(MMP_GL PRIVATE USE_OPENGL)
    if (ENABLE_OPENGL_DEBUG)
        target_compile_definitions(MMP_GL PRIVATE ENABLE_OPENGL_DEBUG)
    endif()
endif()
if (USE_D3D11)
    target_compile_definitions(MMP_GL PRIVATE USE_D3D11)
    if (ENABLE_D3D11_DEBUG)
        target_compile_definitions(MMP_GL PRIVATE ENABLE_D3D11_DEBUG)
    endif()
endif()
if (USE_VULKAN)
    target_compile_definitions(MMP_GL PRIVATE USE_VULKAN)
endif()
target_include_directories(MMP_GL PUBLIC ${GL_INCS})
target_link_libraries(MMP_GL PRIVATE ${GL_LIBS}) 
list(LENGTH GL_LIB_DIR GL_LIB_DIR_LEN)
if (NOT(${GL_LIB_DIR_LEN} EQUAL 0))
    link_directories(MMP_GL ${GL_LIB_DIR})
endif()