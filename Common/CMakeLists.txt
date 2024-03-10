cmake_minimum_required(VERSION 3.8)

execute_process(
    COMMAND git rev-parse --short HEAD
    OUTPUT_VARIABLE MMP_GIT_COMMIT
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND git branch --show-current
    OUTPUT_VARIABLE MMP_GIT_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND git describe --tags --abbrev=0
    OUTPUT_VARIABLE MMP_GIT_TAG
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Common.cpp.in ${CMAKE_CURRENT_SOURCE_DIR}/Common.cpp)

set(Common_SRCS)
set(Common_INCS)

list(APPEND Common_INCS
    ${CMAKE_CURRENT_SOURCE_DIR}
)
list(APPEND Common_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/Common.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Common.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractLogger.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractLogger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LogMessage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/LogMessage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Instantiator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/DynamicFactory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/MetaProgramming.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Placeholder.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ValueHolder.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Any.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractAllocateMethod.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractAllocateMethod.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractDeviceAllocateMethod.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractDeviceAllocateMethod.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/NormalAllocateMethod.h
    ${CMAKE_CURRENT_SOURCE_DIR}/NormalAllocateMethod.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ImmutableVectorAllocateMethod.h
    ${CMAKE_CURRENT_SOURCE_DIR}/SegmentAllocateMethod.h
    ${CMAKE_CURRENT_SOURCE_DIR}/SegmentAllocateMethod.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/StringAllocateMethod.h
    ${CMAKE_CURRENT_SOURCE_DIR}/StringAllocateMethod.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AllocateMethodFactory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AllocateMethodFactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractSharedData.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractSharedData.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/NormalSharedData.h
    ${CMAKE_CURRENT_SOURCE_DIR}/NormalSharedData.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SharedDataDecorator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/SharedDataDecorator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractSharedDataPool.h
    ${CMAKE_CURRENT_SOURCE_DIR}/SharedDataPool.h
    ${CMAKE_CURRENT_SOURCE_DIR}/SharedDataPool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PixelFormat.h
    ${CMAKE_CURRENT_SOURCE_DIR}/PixelFormat.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MediaType.h
    ${CMAKE_CURRENT_SOURCE_DIR}/MediaType.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PixelsInfo.h
    ${CMAKE_CURRENT_SOURCE_DIR}/PixelsInfo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractFrame.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractFrame.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/NormalFrame.h
    ${CMAKE_CURRENT_SOURCE_DIR}/NormalFrame.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractPicture.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractPicture.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/NormalPicture.h
    ${CMAKE_CURRENT_SOURCE_DIR}/NormalPicture.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractPack.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractPack.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/NormalPack.h
    ${CMAKE_CURRENT_SOURCE_DIR}/NormalPack.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Task.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Task.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/FuncTask.h
    ${CMAKE_CURRENT_SOURCE_DIR}/FuncTask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Promise.h
    ${CMAKE_CURRENT_SOURCE_DIR}/TrackTask.h
    ${CMAKE_CURRENT_SOURCE_DIR}/TrackTask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TaskQueue.h
    ${CMAKE_CURRENT_SOURCE_DIR}/TaskQueue.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ThreadPool.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ThreadPool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/EventMessage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/EventMessage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/EventDispatcher.h
    ${CMAKE_CURRENT_SOURCE_DIR}/EventDispatcher.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/EventTag.h
    ${CMAKE_CURRENT_SOURCE_DIR}/EventTag.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/NoticeCenter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/NoticeCenter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CommonGeometry.h
    ${CMAKE_CURRENT_SOURCE_DIR}/CommonGeometry.cpp
)

set(USE_DRM FALSE)
if (NOT MSVC)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(DRM libdrm)
endif()
if (DRM_FOUND)
    set(USE_DRM TRUE)
    list(APPEND Codec_INCS ${DRM_INCLUDE_DIRS})
    list(APPEND Codec_LIB_DIR ${DRM_LIBRARY_DIRS})
    list(APPEND Codec_PUB_LIBS ${DRM_LIBRARIES})
    list(APPEND Common_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/DrmAllocateMethod.h
        ${CMAKE_CURRENT_SOURCE_DIR}/DrmAllocateMethod.cpp
    )
endif()

add_library(MMP_Common OBJECT ${Common_SRCS})
add_library(Mmp::Common ALIAS MMP_Common)
target_include_directories(MMP_Common PUBLIC ${Common_INCS})
target_link_libraries(MMP_Common PRIVATE Poco::Foundation) 
if (USE_DRM)
    target_compile_definitions(MMP_Common PRIVATE USE_DRM)
endif()