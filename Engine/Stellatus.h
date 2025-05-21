
#define STELLATUS_API
#pragma once

#if __cplusplus >= 202002L
    #define CONSTEXPR constexpr
#else 
    #define CONSTEXPR inline
#endif

#define STD ::std::