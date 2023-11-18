project(lodepng)

set(LODEPNG_ROOT ${MMP_PRO_TOP_DIR}/Extension/lodepng)

set(LODEPNG_INC)
set(LODEPNG_SRCS)
set(LODEPNG_LIBS)

list(APPEND LODEPNG_SRCS
    ${MMP_PRO_TOP_DIR}/Extension/lodepng/lodepng.h
    ${MMP_PRO_TOP_DIR}/Extension/lodepng/lodepng.cpp
)

add_library(lodepng STATIC ${LODEPNG_SRCS})

target_include_directories(lodepng
    PUBLIC
        ${MMP_PRO_TOP_DIR}/Extension/lodepng
)
