#pragma once
#define STELLATUS_API
#define STD ::std::
#pragma once

/* 编译器识别 */
#if defined(_MSC_VER)
#define COMPILER_MSVC _MSC_VER
#elif defined(__INTEL_COMPILER)
#define COMPILER_ICC __INTEL_COMPILER
#elif defined(__clang__)
#define COMPILER_CLANG (__clang_major__ * 100 + __clang_minor__)
#elif defined(__GNUC__)
#define COMPILER_GCC (__GNUC__ * 100 + __GNUC_MINOR__)
#endif

/* C++标准版本检测（跨平台修正） */
#if defined(_MSVC_LANG) && !defined(__clang__)
#define CPP_STANDARD _MSVC_LANG // MSVC需特殊处理
#else
#define CPP_STANDARD __cplusplus
#endif

/* 标准版本常量 */
#define CPP11 201103L
#define CPP14 201402L
#define CPP17 201703L
#define CPP20 202002L
#define CPP23 202302L
// #define CPP26 202600L // 预计值

/* 精确版本检测宏 */
#define HAS_CPP11 ( CPP_STANDARD >= CPP11)
#define HAS_CPP14 ( CPP_STANDARD >= CPP14)
#define HAS_CPP17 ( CPP_STANDARD >= CPP17)
#define HAS_CPP20 ( CPP_STANDARD >= CPP20)
#define HAS_CPP23 ( CPP_STANDARD >= CPP23)
// #define HAS_CPP26 ( CPP_STANDARD >= CPP26)

/* 编译器特性检测 */
#if defined(__has_cpp_attribute)
#define HAS_CPP_ATTR(attr) __has_cpp_attribute(attr)
#else
#define HAS_CPP_ATTR(attr) 0
#endif


#if HAS_CPP20 && HAS_CPP_ATTR(nodiscard) >= 201907L
#define NODISCARD(msg) [[nodiscard(msg)]]
#define NODISCARD [[nodiscard]]
#elif HAS_CPP17 && HAS_CPP_ATTR(nodiscard)
#define NODISCARD [[nodiscard]]
#else
#define NODISCARD
#endif


#if HAS_CPP20
#define CONSTEXPR constexpr
#else 
#define CONSTEXPR inline
#endif
