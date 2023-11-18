cmake_minimum_required(VERSION 3.8)

set(Config_SRCS)
set(Config_INCS)
set(Config_LIBS)

list(APPEND Config_INCS
    ${MMP_CORE_PRO_TOP_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)
list(APPEND Config_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractConfig.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractConfig.cpp
)
list(APPEND Config_LIBS
    Poco::Foundation
    Poco::Util
    Mmp::Common
    Mmp::GL
    Mmp::Windows
    Mmp::GPU_Utility
)

if (USE_API_RPC)
    list(APPEND Config_LIBS
        Poco::Net
        Mmp::RPC
    )
endif()

add_library(MMP_Config OBJECT ${Config_SRCS})
add_library(Mmp::Config ALIAS MMP_Config)
target_include_directories(MMP_Config PUBLIC ${Config_INCS})
target_link_libraries(MMP_Config PRIVATE Poco::Foundation Poco::Util Mmp::Common Mmp::GL Mmp::Windows Mmp::GPU_Utility)
if (USE_API_RPC)
    target_compile_definitions(MMP_Config PRIVATE USE_API_RPC)
endif()