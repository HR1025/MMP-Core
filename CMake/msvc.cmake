macro(MMP_MSVC_AUTO_CONFIG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /source-charset:utf-8")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /source-charset:utf-8")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /EHsc")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")

    set(USE_SDL ON)
    set(USE_OPENGL ON)
    set(USE_D3D11 ON)

    # Hint : disable poco test
    set(ENABLE_TESTS OFF)

    if (MMP_SYSTEM_DEVICE MATCHES "x86")
        set(SDL2_DIR "${MMP_CORE_PRO_TOP_DIR}/Extension/vcpkg/packages/sdl2_x86-windows/share/sdl2/")
    elseif (MMP_SYSTEM_DEVICE MATCHES "x64")
        set(SDL2_DIR "${MMP_CORE_PRO_TOP_DIR}/Extension/vcpkg/packages/sdl2_x64-windows/share/sdl2/")
    endif()
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Extension/vcpkg/scripts/buildsystems/vcpkg.cmake" 
    CACHE STRING "Vcpkg toolchain file")
endmacro()