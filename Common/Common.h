//
// Common.h
//
// Library: Common
// Package: Common
// Module:  Common
// 
// This file modified from : 
// 1 - https://github.com/hrydgard/MMP/blob/master/MMP_config.h - commit 564c4b38bd562921b9787f630110f25eb0638096

#pragma once

#include <string>

#ifdef _WIN32
#pragma warning(disable:4091)
#pragma warning(disable:4067)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#undef max
#undef min
#endif /* _WIN32 */

namespace Mmp
{

#define MMP_ARCH(MMP_FEATURE) (MMP_ARCH_##MMP_FEATURE)
#define MMP_PLATFORM(MMP_FEATURE) (MMP_PLATFORM_##MMP_FEATURE)

const std::string GetMMPBrach();
const std::string GetMMPTag();
const std::string GetMMPCommit();

#define MMP_NAME      "MMP"
#define MPP_BRACH     (GetMMPBrach())
#define MPP_TAG       (GetMMPTag())
#define MMP_COMMIT    (GetMMPCommit())

// ARCH defines
#if defined(_M_IX86) || defined(__i386__) || defined (__EMSCRIPTEN__)
    #define MMP_ARCH_X86 1
    #define MMP_ARCH_32BIT 1
    //TODO: Remove this compat define
    #ifndef _M_IX86
        #define _M_IX86 600
    #endif
#endif

#if (defined(_M_X64) || defined(__amd64__) || defined(__x86_64__)) && !defined(__EMSCRIPTEN__)
    #define MMP_ARCH_AMD64 1
    #if defined(__ILP32__)
        #define MMP_ARCH_32BIT 1
    #else
        #define MMP_ARCH_64BIT 1
    #endif
    //TODO: Remove this compat define
    #ifndef _M_X64
        #define _M_X64 1
    #endif
#endif

#if defined(__arm__) || defined(_M_ARM)
    #define MMP_ARCH_ARM 1
    #define MMP_ARCH_32BIT 1

    #if defined(__ARM_ARCH_7__) || \
      defined(__ARM_ARCH_7A__) || \
      defined(__ARM_ARCH_7S__)
        #define MMP_ARCH_ARMV7 1
    #endif

    #if defined(__ARM_ARCH_7S__)
        #define MMP_ARCH_ARMV7S 1
    #endif

    #if defined(__ARM_NEON) || defined(__ARM_NEON__)
        #define MMP_ARCH_ARM_NEON 1
    #endif

    #if defined(_M_ARM)
        #define MMP_ARCH_ARMV7 1
        #define MMP_ARCH_ARM_NEON 1
    #endif
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
    #define MMP_ARCH_ARM64 1
    #define MMP_ARCH_64BIT 1
    #define MMP_ARCH_ARM_NEON 1
    #define MMP_ARCH_ARM64_NEON 1
#endif

#if defined(__mips64__)
    #define MMP_ARCH_MIPS64 1
    #define MMP_ARCH_64BIT 1
#elif defined(__mips__)
    #define MMP_ARCH_MIPS 1
    #define MMP_ARCH_32BIT 1
#endif

#if defined(__riscv) && defined(__riscv_xlen) && __riscv_xlen == 64
    //https://github.com/riscv/riscv-c-api-doc/blob/master/riscv-c-api.md
    #define MMP_ARCH_RISCV64 1
    #define MMP_ARCH_64BIT 1
#endif


// PLATFORM defines
#if defined(_WIN32)
    // Covers both 32 and 64bit Windows
    #define MMP_PLATFORM_WINDOWS 1
	#ifdef _M_ARM
        #define MMP_ARCH_ARM_HARDFP 1
	#endif
	// UWP trickery
    #if defined(WINAPI_FAMILY) && defined(WINAPI_FAMILY_PARTITION)
        #if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP) && WINAPI_FAMILY != WINAPI_FAMILY_DESKTOP_APP
            #define MMP_PLATFORM_UWP 1
        #endif
    #endif
#elif defined(__APPLE__)
    #include <TargetConditionals.h>
    #if TARGET_IPHONE_SIMULATOR
        #define MMP_PLATFORM_IOS 1
        #define MMP_PLATFORM_IOS_SIMULATOR 1
    #elif TARGET_OS_IPHONE
        #define MMP_PLATFORM_IOS 1
    #elif TARGET_OS_MAC
        #define MMP_PLATFORM_MAC 1
    #else
        #error "Unknown Apple platform"
    #endif
#elif defined(__SWITCH__)
    #define MMP_PLATFORM_SWITCH 1
#elif defined(__ANDROID__)
    #define MMP_PLATFORM_ANDROID 1
    #define MMP_PLATFORM_LINUX 1
#elif defined(__linux__)
    #define MMP_PLATFORM_LINUX 1
#elif defined(__OpenBSD__)
    #define MMP_PLATFORM_OPENBSD 1
#endif

// EXPORT defines
#if (defined(_WIN32) || defined(_WIN32_WCE))
	#if defined(MMP_EXPORTS)
		#define MMP_API __declspec(dllexport)
	#else
		#define MMP_API __declspec(dllimport)
	#endif
#endif


#if !defined(MMP_API)
	#if !defined(POCO_NO_GCC_API_ATTRIBUTE) && defined (__GNUC__) && (__GNUC__ >= 4)
		#define MMP_API __attribute__ ((visibility ("default")))
	#else
		#define MMP_API
	#endif
#endif

} // namespace Mmp