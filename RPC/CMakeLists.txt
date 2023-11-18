set(RPC_SRCS)
set(RPC_INCS)
set(RPC_LIBS)

option(RPC_WITH_HTTP "RPC with the method of HTTP" ON)

list(APPEND RPC_INCS
    ${MMP_CORE_PRO_TOP_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

list(APPEND RPC_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/RPCCommon.h
    ${CMAKE_CURRENT_SOURCE_DIR}/RPCCommon.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractRPC.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractRPC.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractRPCDispatcher.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractRPCDispatcher.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractRPCRequest.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractRPCRequest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractRPCResponse.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractRPCResponse.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractRPCProcess.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractRPCProcess.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RPCProxy.h
    ${CMAKE_CURRENT_SOURCE_DIR}/RPCProxy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RPCUTils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/RPCUTils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RPCFactory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/RPCFactory.cpp
)

if (RPC_WITH_HTTP)
    list(APPEND RPC_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/HTTP/HTTPRPC.h
        ${CMAKE_CURRENT_SOURCE_DIR}/HTTP/HTTPRPC.cpp
    )
endif()

list(APPEND RPC_LIBS
    Mmp::Common
    Poco::Foundation
)

if (RPC_WITH_HTTP)
    list(APPEND RPC_LIBS
        Poco::Net
    )
endif()

add_library(MMP_RPC OBJECT ${RPC_SRCS})
add_library(Mmp::RPC ALIAS MMP_RPC)
target_include_directories(MMP_RPC PUBLIC ${RPC_INCS})
target_link_libraries(MMP_RPC PRIVATE ${RPC_LIBS})
if (RPC_WITH_HTTP)
    target_compile_definitions(MMP_RPC PRIVATE RPC_WITH_HTTP)
endif()