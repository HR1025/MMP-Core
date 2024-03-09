cmake_minimum_required(VERSION 3.8)

set(PG_SRCS)
set(PG_INCS)
set(PG_LIB_DIR)
set(PG_LIBS)

list(APPEND PG_INCS
    ${MMP_PRO_TOP_DIR}
    ${MMP_CORE_PRO_TOP_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${MMP_CORE_PRO_TOP_DIR}/Extension/eigen
)


## Scene
list(APPEND PG_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/SceneCommon.h
    ${CMAKE_CURRENT_SOURCE_DIR}/SceneCommon.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractSceneItem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractSceneItem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractSceneLayer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractSceneLayer.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractSceneCompositor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractSceneCompositor.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/Scene/SceneItemImpl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Scene/SceneItemImpl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Scene/SceneLayerImpl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Scene/SceneLayerImpl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Scene/SceneCompositorImpl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Scene/SceneCompositorImpl.cpp
)

## Utility
list(APPEND PG_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/Utility/CommonUtility.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Utility/CommonUtility.cpp
)

add_library(MMP_PG OBJECT ${PG_SRCS})
add_library(Mmp::PG ALIAS MMP_PG)
target_include_directories(MMP_PG PUBLIC ${PG_INCS})
target_link_libraries(MMP_PG PUBLIC PRIVATE Poco::Foundation Mmp::Common) 
