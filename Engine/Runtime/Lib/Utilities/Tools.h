#pragma once
#include "../../../Stellatus.h"
#include <type_traits>
#include <random>
#include <memory>
namespace Stellatus::tools {
    // 类型处理工具, 实际上 标准库内部都有不同的实现, 这里不依赖任何版本的标准库重新实现一遍
    // note: 从一个 std::allocator<Ty> 重新 rebind 到 std::allocator<_node<Ty>>
    template <typename _Alloc, typename _Value_Keype>
    using _rebind_alloc_t = typename STD allocator_traits<_Alloc>::template rebind_alloc<_Value_Keype>;

    template <typename _Ptr, typename _Ty>
    using _rebind_pointer_t = typename STD pointer_traits<_Ptr>::template rebind<_Ty>;

    template <class _Alloc>
    constexpr bool _is_simple_alloc_v =
        STD is_same_v<typename STD allocator_traits<_Alloc>::size_type, size_t>&&
        STD is_same_v<typename STD allocator_traits<_Alloc>::difference_type, ptrdiff_t>&&
        STD is_same_v<typename STD allocator_traits<_Alloc>::pointer, typename _Alloc::value_type*>&&
        STD is_same_v<typename STD allocator_traits<_Alloc>::const_pointer, const typename _Alloc::value_type*>;

    template <class _Val_Ty>
    struct _simple_type {
        using value_type = _Val_Ty;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using pointer = _Val_Ty*;
        using const_pointer = const _Val_Ty*;
    };
    // @function: 有序容器的key提取, 用于就地构造
    template <typename _Key, typename... Args>
    struct _in_place_key_extract_ordered_list {
        static constexpr bool _extractable = false;
    };

    template <typename _Key, typename _Second>
    struct _in_place_key_extract_ordered_list<_Key, _Key, _Second> {
        static constexpr bool _extractable = true;
        static const _Key& _Extract(const _Key& key, const _Second&) noexcept {
            return key;
        }
    };
    template <typename _Key, typename _First, typename _Second>
    struct _in_place_key_extract_ordered_list<_Key, STD pair<_First, _Second>> {
        static constexpr bool _extractable = STD is_same_v<_Key, STD remove_cv_t<_First>>;
        static const _Key& _Extract(const STD pair<_First, _Second>& p) noexcept {
            return p.first;
        }
    };

    template <class _Alloc>
    using _alloc_ptr_t = typename STD allocator_traits<_Alloc>::pointer;

    template <class _Alloc>
    using _alloc_size_t = typename STD allocator_traits<_Alloc>::size_type;


    
    template <typename _Ty, typename... _Types>
    CONSTEXPR void _construct_in_place(_Ty& _Obj, _Types&&... _Args)
        noexcept(STD is_nothrow_constructible_v<_Ty, _Types...>) {
#if HAS_CPP20
        if (STD is_constant_evaluated()) {
            STD construct_at(STD addressof(_Obj), STD forward<_Types>(_Args)...);
        }
        else
#endif
        {
            ::new (static_cast<void*>(STD addressof(_Obj))) _Ty(STD forward<_Types>(_Args)...);
        }
    }

    /*
     * @note: _destory_range 和 _destroy_in_place 不会递归调用的原因:
     * 数组类型总是有长度限制的, 编译期生成的代码是不会无限的递归下去
     */
    template <class _NoThrowFwdIt, class _NoThrowSentinel>
    CONSTEXPR void _destory_range(_NoThrowFwdIt _first, const _NoThrowSentinel _last) noexcept {
#if HAS_CPP20
        if (STD is_constant_evaluated()) {
            for (; _first != _last; ++_first) {
                _destroy_in_place(*_first);
            }
        }
        else
#endif
        {
            if constexpr (!STD is_trivially_destructible_v<STD iter_value_t<_NoThrowFwdIt>>) {
                for (; _first != _last; ++_first) {
                    _destroy_in_place(*_first);
                }
                            
            } 
        }
    }

    template <class _Ty>
    CONSTEXPR void _destroy_in_place(_Ty& _obj) {
        if constexpr (STD is_array_v<_Ty>) {
            _destroy_range(_obj, _obj + STD extent_v <_Ty>);
        }
        else {
            _obj.~_Ty();
        }
    }

    /*
     * @function: 这个类的存在是为了保证异常安全(exception safety) 
     * @note: 如果直接 Allocator<_Ty>().allocate(1); 如果出现异常就会返回 nullptr 
     * @note: 导致在析构函数析构的时候出现重复释放
     *
     */
    template <class _Alloc>
    struct _alloc_construct_smart_ptr {
        using pointer = _alloc_ptr_t<_Alloc>;
        _Alloc& _m_al;
        pointer _m_ptr;

        CONSTEXPR explicit _alloc_construct_smart_ptr(_Alloc& _al)
            : _m_al(_al), _m_ptr(nullptr) {
        }

        NODISCARD CONSTEXPR pointer release() noexcept {
            return STD exchange(_m_ptr, nullptr);
        }

        CONSTEXPR void _allocate() {
            _m_ptr = nullptr;
            _m_ptr = _m_al.allocate(1);
        }

        CONSTEXPR ~_alloc_construct_smart_ptr() {
            if (_m_ptr) {
                _m_al.deallocate(_m_ptr, 1);
            }
        }
    };

    struct _fake_allocator {};

    // 容器的空基类优化
    class _container_base {
        // 在容器被修改或销毁时，标记所有关联的迭代器为"孤儿"状态（失效状态）
        // Debug: 遍历并断开所有指向该容器的迭代器链接
        // Release: 空操作（被完全优化掉）
        CONSTEXPR void _orphan_all() noexcept {}

        // 交换两个容器的迭代器跟踪状态
        // Debug: 交换内部的 _Myproxy 指针（迭代器链表头）
        // Release: 空操作
        CONSTEXPR void _swap_proxy_and_iterators(_container_base&) noexcept {}

        // 初始化容器的迭代器跟踪基础设施
        // Debug: 用给定的分配器构造 _Myproxy 对象
        // Release: 空操作
        CONSTEXPR void _alloc_proxy(const _fake_allocator&) noexcept {}

        // 在容器分配器被替换时，重建迭代器跟踪系统
        // Debug: 用新分配器重新创建 _Myproxy，迁移现有迭代器
        // Release: 空操作
        CONSTEXPR void _reload_proxy(const _fake_allocator&, const _fake_allocator&) noexcept {}
    };

    // 迭代器的空基类优化
    // 用于 debug 模式下的调试
    // release 模式下，空积累会被优化掉
    struct _iterator_base {
        // Debug 模式下将迭代器与容器关联（用于验证有效性）
        // release 模式下, 空操作（参数和调用都会被优化掉）
        CONSTEXPR void _adopt(const void*) noexcept {}

        // 获取迭代器关联的容器（用于边界检查）
        // release 模式: 始终返回 nullptr（无检查）
        CONSTEXPR const _container_base* _getcnt() const noexcept {
            return nullptr;
        }

        // 控制迭代器在未验证时的解引用行为, 
        // true：允许直接解引用（发布模式默认）
        // false：在调试模式下触发验证（如边界检查、失效检查）
        static constexpr bool _unwrap_when_unverified = true;
    };

    double get_random_number() {
        STD random_device rd;
        STD mt19937 gen(rd());

        STD uniform_real_distribution<double> dis(0.0, 1.0);

        return dis(gen);
    }
}