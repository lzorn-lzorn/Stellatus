// skip.h
#pragma once
#include ""
#include "../tools.h"
#include <initializer_list>
#include <iterator>
#include <array>
#include <optional>
#include <type_traits>
#include <memory>
#include <cassert>

namespace Stellatus {    
template <typename _Traits>
class _skiplist;

template <class _List>
struct _skip_const_iterator {
    using iterator_category = STD bidirectional_iterator_tag;
    using value_type        = typename _List::value_type;
    using difference_type   = typename _List::difference_type;
    using pointer           = typename _List::pointer;
    using reference         = value_type&;
    using _node_ptr          = typename _List::_node_ptr;

    [[nodiscard]] reference operator*() const noexcept{
        return this->_m_ptr->_m_Val;
    }

    [[nodiscard]] pointer operator->() const noexcept{
        return STD pointer_traits<pointer>::pointer_to(**this);
    }

    _skip_const_iterator& operator++() noexcept{
        this->_m_ptr = this->_m_ptr->_m_Next;
    }
    _skip_const_iterator operator++(int) noexcept{
        _skip_const_iterator tmp = *this;
        ++*this;
        return tmp;  
    }

    _skip_const_iterator& operator--() noexcept{
        const auto new_ptr = this->_m_ptr->_m_Prev;
        this->_m_ptr = this->_m_ptr->_m_Next;
        return *this;
    }

    _skip_const_iterator operator--(int) noexcept{
        _skip_const_iterator tmp = *this;
        ++*this;
        return tmp;  
    }

    [[nodiscard]] bool operator==(const _skip_const_iterator& other){
        return this->_m_ptr == other._m_ptr;
    }

    [[nodiscard]] bool operator!=(const _skip_const_iterator& other){
        return !(*this == other);
    }

    void _seek_to(const _skip_const_iterator<_List> iter) noexcept{
        this->_m_ptr = iter._m_ptr;
    }

    _node_ptr _m_ptr;
};
template <class _List>
struct _skip_iterator : public _skip_const_iterator<_List> {
    using _base_iter        = _skip_const_iterator<_List>;
    using iterator_category = STD bidirectional_iterator_tag;
    using value_type        = typename _List::value_type;
    using difference_type   = typename _List::difference_type;
    using pointer           = typename _List::pointer;
    using reference         = value_type&;
    using _node_ptr          = typename _List::_node_ptr;

    [[nodiscard]] reference operator*() const noexcept{
        return const_cast<reference>(_base_iter::operator*());
    }

    [[nodiscard]] pointer operator->() const noexcept{
        return STD pointer_traits<pointer>::pointer_to(**this);
    }

    _skip_iterator& operator++() noexcept{
        _base_iter::operator++();
        return *this;
    }

    _skip_iterator operator++(int) noexcept{
        _skip_iterator tmp = *this;
        _base_iter::operator++();
        return tmp;  
    }

    _skip_iterator& operator--() noexcept{
        _base_iter::operator--();
        return *this;
    }

    _skip_iterator operator--(int) noexcept{
        _skip_const_iterator tmp = *this;
        _base_iter::operator--();
        return tmp;  
    }
};

template <class _Value_Keype,
          class _Size_Keype,
          class _Difference_Keype,
          class _Pointer,
          class _Const_Pointer,
          class _node_ptr_Keype>
struct _skip_iter_types {
    using value_type      = _Value_Keype;
    using size_type       = _Size_Keype;
    using difference_type = _Difference_Keype;
    using pointer         = _Pointer;
    using const_pointer   = _Const_Pointer;
    using _node_ptr       = _node_ptr_Keype;
};
/*
 * @function: 跳表节点
 * @members:
 *    - 
 * 
 */
 template <typename _Val_Ty, typename _Voidptr, uint8_t _Level=1>
 struct _skip_node{
 public:
     using _node_ptr = tools::_rebind_pointer_t<_Voidptr, _skip_node>;
     using value_type =_Val_Ty;
 
     //  节点的四种类型: 头部节点, 尾部节点, 中间的正常节点, 被删除节点
     enum class _type { HEAD, TAIL, NORMAL, INVALID };
     struct _control_block {
         STD atomic<_node_ptr> _m_next{}; // 指向的下一个位置的指针
         STD size_t _m_count{0ul};        // 改指针对应最底层跨越了多少个节点
         _control_block() : _m_next(nullptr), _m_count(0ul){}
         _control_block(_node_ptr ptr, STD size_t count) : _m_next(ptr), _m_count(count){}
         _control_block(const _control_block&) = default;
         _control_block& operator=(const _control_block&) = default;
         _control_block(_control_block&&) = delete;
         _control_block& operator=(_control_block&&) = delete;
     };
     public:
     _skip_node()                             = default;
     _skip_node(const _skip_node&)            = delete;
     _skip_node& operator=(const _skip_node&) = delete;
 
 public:
     // @function: get head 时, 会同时构造 tail 节点
     template<class _Alloc>
     static _node_ptr get_head(_Alloc& alloc, _node_ptr& head_ptr, _node_ptr& tail_ptr){
         static_assert(STD is_same_v<typename _Alloc::value_type, _skip_node>, "Error _get_sp_node() call");
         // note: 使用 uninitialized_fill 是低效的, 内部是循环调用 operator(const _Ty&)
         // tail node 
         const auto _mem_tail_ptr = alloc.allocate(1);
         _mem_tail_ptr->_m_type = _type::TAIL;
         for (auto _each: _mem_tail_ptr->_m_nexts){
             _each->_m_next = STD nullopt;
             _each->_m_count = 0ul;
         }
 
         // head node
         const auto _mem_head_ptr = alloc.allocate(1);
         _mem_head_ptr->_m_type = _type::HEAD;
         for (auto _each: _mem_tail_ptr->_m_nexts){
             _each->_m_next = _mem_tail_ptr;
             _each->_m_count = 0ul;
         }
 
         // return
         head_ptr = _mem_head_ptr;
         tail_ptr = _mem_tail_ptr;
         return _mem_head_ptr;
     }
 
 
 public:
     STD array<STD optional<_control_block>, _Level> _m_nexts{STD nullopt}; // 指向一些节点的指针们
     STD atomic<_node_ptr> _m_pre; // 最底层是一个双链表
     _type _m_type{_type::INVALID};
     value_type _m_data{};
 };
 template <class _Val_Ty>
 struct _skip_simple_types : tools::_simple_type< _Val_Ty> {
     using _node = _skip_node<_Val_Ty, void*>;
     using _node_ptr = _node*;
 };
 template <typename _Traits>
 class _skiplist {
 public:
     using key_type        = typename _Traits::key_value;
     using value_type      = typename _Traits::value_type;
     using allocator_type  = typename _Traits::allocator_type;
 protected:
     using _alloc_type        = tools::_rebind_alloc_t<allocator_type, value_type>;
     using _alloc_type_traits = STD allocator_traits<_alloc_type>;
     using _node              = _skip_node<value_type, typename _alloc_type_traits::void_pointer>;
     using _alloc_node        = tools::_rebind_alloc_t<allocator_type, _node>;
     using _alloc_node_traits = STD allocator_traits<_alloc_node>;
     using _node_ptr          = typename _alloc_node_traits::pointer;
     using _val_type          = std::conditional_t<tools::_is_simple_alloc_v<_alloc_node>, // 编译期的类型选择器
         _skip_simple_types<value_type>,
         _skip_iter_types<value_type,
                          typename _alloc_type_traits::size_type,
                          typename _alloc_type_traits::difference_type,
                          typename _alloc_type_traits::pointer,
                          typename _alloc_type_traits::const_pointer,
                          _node_ptr
         >
     >;
     public:
     using key_compare     = typename _Traits::key_compare;
     using value_compare   = typename _Traits::value_compare;
     using difference_type = typename _alloc_type_traits::difference_type;
     using pointer         = typename _alloc_type_traits::pointer;
     using const_pointer   = typename _alloc_type_traits::const_pointer;
     using size_type       = typename _alloc_type_traits::size_type;
     using reference       = value_type&;
     using const_reference = const value_type&;
 
     using iterator               = _skip_iterator<_skiplist>;
     using const_iterator         = _skip_const_iterator<_skiplist> ;
     using reverse_iterator       = STD reverse_iterator<iterator>;
     using const_reverse_iterator = STD reverse_iterator<const_iterator>;
     public:
     _skiplist() : _skiplist(_Alloc()) {};
     explicit _skiplist(const allocator_type& Alloc = _Alloc()) : _skiplist(Alloc) {
         
     }
     template <class Input>
     _skiplist(Input first, Input last, const allocator_type& alloc = _Alloc()) {}
     _skiplist(STD initializer_list<value_type> _init_list, const allocator_type& alloc = _Alloc()){}
 
     _skiplist(const _skiplist& other) {}
     _skiplist(_skiplist&& other){}
 
 public:
     _skiplist& operator=(const _skiplist& other){}
     _skiplist& operator=(_skiplist&& other){}
 
 
 };
 
 }