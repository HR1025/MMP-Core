cmake_minimum_required(VERSION 3.8)

set(SL_SRCS)
set(SL_INCS)
set(SL_LIB_DIR)
set(SL_LIBS)

list(APPEND SL_INCS
    ${MMP_CORE_PRO_TOP_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

list(APPEND SL_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/SLCommon.h
    ${CMAKE_CURRENT_SOURCE_DIR}/SLCommon.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractSLConverter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractSLFactory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractSLFactory.cpp
)

list(APPEND ${SL_LIBS}
    Poco::Foundation
    Mmp::Common
)

if (USE_SPRIV_CROSS)
    list(APPEND ${SL_LIBS}
        SPIRV-Cross
    )
endif()


add_library(MMP_SL OBJECT ${SL_SRCS})
add_library(Mmp::SL ALIAS MMP_SL)
target_include_directories(MMP_SL PUBLIC ${SL_INCS})
target_link_libraries(MMP_SL PRIVATE ${SL_LIBS}) 
