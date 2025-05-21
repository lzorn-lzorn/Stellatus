#pragma once
#include "Def.h"
#include <type_traits>
#include <memory>
namespace Stellatus::tools {
    // 类型处理工具, 实际上 标准库内部都有不同的实现, 这里不依赖任何版本的标准库重新实现一遍
    // note: 从一个 std::allocator<Ty> 重新 rebind 到 std::allocator<_node<Ty>>
    template <typename _Alloc, typename _Value_Keype>
    using _rebind_alloc_t = typename ::std::allocator_traits<_Alloc>::template rebind_alloc<_Value_Keype>;

    template <typename _Ptr, typename _Ty>
    using _rebind_pointer_t = typename ::std::pointer_traits<_Ptr>::template rebind<_Ty>;

    template <class _Alloc>
    constexpr bool _is_simple_alloc_v =
        ::std::is_same_v<typename ::std::allocator_traits<_Alloc>::size_type, size_t> &&
        ::std::is_same_v<typename ::std::allocator_traits<_Alloc>::difference_type, ptrdiff_t> &&
        ::std::is_same_v<typename ::std::allocator_traits<_Alloc>::pointer, typename _Alloc::value_type*> &&
        ::std::is_same_v<typename ::std::allocator_traits<_Alloc>::const_pointer, const typename _Alloc::value_type*>;

    template <class _Val_Ty>
    struct _simple_type {
        using value_type      = _Val_Ty;
        using size_type       = size_t;
        using difference_type = ptrdiff_t;
        using pointer         = _Val_Ty*;
        using const_pointer   = const _Val_Ty*;
    };
    // @function: 有序容器的key提取, 用于就地构造
    template <typename _Key, typename... Args>
    struct _in_place_key_extract_ordered_list {
        static constexpr bool _extractable = false;
    };

    template <typename _Key, typename _Second>
    struct _in_place_key_extract_ordered_list<_Key, _Key, _Second>{
        static constexpr bool _extractable = true;
        static const _Key& _Extract(const _Key& key, const _Second&) noexcept{
            return key;
        }
    };
    template <typename _Key, typename _First, typename _Second>
    struct _in_place_key_extract_ordered_list<_Key, ::std::pair<_First, _Second>> {
        static constexpr bool _extractable = ::std::is_same_v<_Key, ::std::remove_cv_t<_First>>;
        static const _Key& _Extract(const ::std::pair<_First, _Second>& p) noexcept {
            return p.first;
        }
    };

    template <typename _Ty, typename... _Types>
    CONSTEXPR void _construct_in_place(_Ty& _Obj, _Types&&... _Args) 
        noexcept(::std::is_nothrow_constructible_v<_Ty, _Types...>) {
    #if __cplusplus >= 202002L 
        if(::std::is_constant_evaluated()){
            ::std::construct_at(::std::addressof(_Obj), ::std::forward<_Types>(_Args)...);
        } else
    #endif
        {
            ::new (static_cast<void*>(::std::addressof(_Obj))) _Ty(::std::forward<_Types>(_Args)...);
        }
    }
}