#pragma once
#include "../Utilities/Tools.h"
#include <random>
#include <initializer_list>
#include <iterator>
#include <vector>
#include <optional>
#include <type_traits>
#include <memory>
#include <cassert>
#include <bit>

namespace Stellatus {
    template <typename _Traits>
    class _skiplist;

    static size_t _get_level() {
        thread_local STD mt19937 gen(STD random_device{}());
        // 使用32位随机数的低16位
        constexpr uint32_t threshold = 0x4000;
        uint32_t bits = STD uniform_int_distribution<uint32_t>{}(gen);
        return STD countr_zero(bits & 0xFFFF) / 2 + 1; // 利用CTZ优化
    }

    template <class _List>
    struct _skip_const_iterator {
        using iterator_category = STD bidirectional_iterator_tag;
        using value_type = typename _List::value_type;
        using difference_type = typename _List::difference_type;
        using pointer = typename _List::pointer;
        using reference = value_type&;
        using _node_ptr = typename _List::_node_ptr;

        [[nodiscard]] reference operator*() const noexcept {
            return this->_m_ptr->_m_Val;
        }

        [[nodiscard]] pointer operator->() const noexcept {
            return STD pointer_traits<pointer>::pointer_to(**this);
        }

        _skip_const_iterator& operator++() noexcept {
            this->_m_ptr = this->_m_ptr->_m_Next;
        }
        _skip_const_iterator operator++(int) noexcept {
            _skip_const_iterator tmp = *this;
            ++*this;
            return tmp;
        }

        _skip_const_iterator& operator--() noexcept {
            const auto new_ptr = this->_m_ptr->_m_Prev;
            this->_m_ptr = this->_m_ptr->_m_Next;
            return *this;
        }

        _skip_const_iterator operator--(int) noexcept {
            _skip_const_iterator tmp = *this;
            ++*this;
            return tmp;
        }

        [[nodiscard]] bool operator==(const _skip_const_iterator& other) {
            return this->_m_ptr == other._m_ptr;
        }

        [[nodiscard]] bool operator!=(const _skip_const_iterator& other) {
            return !(*this == other);
        }

        void _seek_to(const _skip_const_iterator<_List> iter) noexcept {
            this->_m_ptr = iter._m_ptr;
        }

        _node_ptr _m_ptr;
    };
    template <class _List>
    struct _skip_iterator : public _skip_const_iterator<_List> {
        using _base_iter = _skip_const_iterator<_List>;
        using iterator_category = STD bidirectional_iterator_tag;
        using value_type = typename _List::value_type;
        using difference_type = typename _List::difference_type;
        using pointer = typename _List::pointer;
        using reference = value_type&;
        using _node_ptr = typename _List::_node_ptr;

        [[nodiscard]] reference operator*() const noexcept {
            return const_cast<reference>(_base_iter::operator*());
        }

        [[nodiscard]] pointer operator->() const noexcept {
            return STD pointer_traits<pointer>::pointer_to(**this);
        }

        _skip_iterator& operator++() noexcept {
            _base_iter::operator++();
            return *this;
        }

        _skip_iterator operator++(int) noexcept {
            _skip_iterator tmp = *this;
            _base_iter::operator++();
            return tmp;
        }

        _skip_iterator& operator--() noexcept {
            _base_iter::operator--();
            return *this;
        }

        _skip_iterator operator--(int) noexcept {
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
        using value_type = _Value_Keype;
        using size_type = _Size_Keype;
        using difference_type = _Difference_Keype;
        using pointer = _Pointer;
        using const_pointer = _Const_Pointer;
        using _node_ptr = _node_ptr_Keype;
    };

    // 代表三种不同类型的节点 头节点, 尾节点, 中间节点
    enum class _skip_node_type { HEAD, TAIL, NORMAL, INVALID };

    template <typename _Val_Ty, typename _Voidptr, uint8_t _Level = 16>
    struct _skip_node {
    public:
        using _node_ptr = tools::_rebind_pointer_t<_Voidptr, _skip_node>;
        using value_type = _Val_Ty;

        struct _control_block {
            STD atomic<_node_ptr> _m_next{};   // 指向的下一个位置的指针
            STD atomic<_node_ptr> _m_prev{};
            STD size_t _m_count{ 0ul };        // 改指针对应最底层跨越了多少个节点
            CONSTEXPR _control_block() noexcept
                : _m_next(nullptr), _m_prev(nullptr), _m_count(0ul) {
            }
            CONSTEXPR _control_block(_node_ptr _next, _node_ptr _prev, STD size_t count) noexcept
                : _m_next(_next), _m_prev(_prev), _m_count(count) {
            }
            _control_block(const _control_block&) = delete;
            _control_block& operator=(const _control_block&) = delete;
            _control_block(_control_block&&) = delete;
            _control_block& operator=(_control_block&&) = delete;
        };
    public:
        explicit _skip_node(_skip_node_type _type) noexcept
            : _m_type(_type), _m_nexts{} {
            if (_type == _skip_node_type::HEAD) {
                _m_val._m_max_level = _Level;
            }
        }
        ~_skip_node() {
            if (_m_type == _skip_node_type::NORMAL) {
                _m_val._m_data.~value_type();
            }
        }
        _skip_node(const _skip_node&) = delete;
        _skip_node& operator=(const _skip_node&) = delete;

    public:
        // @function: 成对构造 head 和 tail
        template<class _Alloc>
        static STD pair<_node_ptr, _node_ptr> create_head_and_tail(_Alloc& alloc) {
            static_assert(STD is_same_v<typename _Alloc::value_type, _skip_node>, "Error _get_sp_node() call");
            // note: 使用 uninitialized_fill 是低效的, 内部是循环调用 operator(const _Ty&)
            // tail node 
            const auto _mem_tail_ptr = alloc.allocate(1);
            tools::_construct_in_place(_mem_tail_ptr, _skip_node_type::TAIL);
            _mem_tail_ptr->_m_nexts.fill(_control_block{});
            /*for (auto _each : _mem_tail_ptr->_m_nexts) {
                _each->_m_next = STD nullopt;
                _each->_m_count = 0ul;
            }*/

            // head node
            const auto _mem_head_ptr = alloc.allocate(1);
            tools::_construct_in_place(_mem_head_ptr, _skip_node_type::HEAD);
            _mem_head_ptr->_m_nexts.fill(_control_block{});
            for (auto& _each : _mem_head_ptr->_m_nexts) {
                _each._m_next.store(_mem_tail_ptr, STD memory_order_relaxed);
            }

            return { _mem_head_ptr, _mem_tail_ptr };
        }

        template<class _Alloc, class... _ValTy>
        static _node_ptr create_normal_node(_Alloc& alloc, _ValTy&& ..._val) {
            static_assert(STD is_same_v<typename _Alloc::value_type, _skip_node>, "Error get_node() call");
            // 1. 分配内存
            tools::_alloc_construct_smart_ptr<_Alloc> _new_node(alloc);
            _new_node._allocate();

            // 2. 初始化数据成员
            // note: 使用分配器 alloc 在_m_data的地址上用 _val 就地构造 _m_data 对象
            STD allocator_traits<_Alloc>::construct(
                alloc,
                STD addressof(_new_node._m_ptr->_m_val._m_data),
                STD forward <_ValTy>(_val)...
            );

            tools::_construct_in_place(_new_node->_m_ptr, _skip_node_type::NORMAL);

            // 3. 初始化层级信息
            const auto _level = _get_level();
            _new_node._m_ptr->_m_nexts.reserve(_level);
            for (size_t i = 0; i < _level; ++i) {
                _new_node._m_ptr->_m_nexts.emplace_back( 
                    STD make_optional <_control_block>(
                        nullptr, // 初始化 next 指针
                        nullptr, // 初始化 prev 指针
                        0        // 初始化 count 为 0;
                    )
                );
            }

            // 4. 移交指针的控制权
            return _new_node.release();
        }

        // @function: 普通的删除只是修改标记
        void _mark_invalid() noexcept {
            if (this->_m_type == _skip_node_type::NORMAL) {
                this->_m_type.store(_skip_node_type::INVALID, STD memory_order_release);
                return;
            }

        }

        template <class _Alloc>
        static void _free_node(_Alloc& _al, _node_ptr _ptr) noexcept {
            static_assert(STD is_same_v<typename _Alloc::value_type, _skip_node>, "Error _free_node() call");
            if (!_ptr || _ptr->_m_type == _skip_node_type::HEAD || _ptr->_m_type == _skip_node_type::TAIL) {
                return;
            }
            if (_ptr->_m_type == _skip_node_type::NORMAL) {
                STD allocator_traits<_Alloc>::destroy(_al, STD addressof(_ptr->_m_val._m_data));
                STD allocator_traits<_Alloc>::deallocate(_al, _ptr, 1);
            }
        }

        // @function: 释放 head 和 tail 指针
        template <class _Alloc>
        static void _free_head_and_tail(_Alloc& _al, _node_ptr head, _node_ptr tail) noexcept {
            if (head) {
                // note: head 和 tail 都没有 _m_val._m_data; 
                // note: 无法 STD allocator_traits<_Alloc>::destroy(_al, STD addressof(head->_m_data));
                // note: 而且析构函数和 alloc 是无关的
                STD destroy_at(head);
                STD allocator_traits<_Alloc>::deallocate(_al, head, 1);
            }
            if (tail) {
                STD destroy_at(tail);
                STD allocator_traits<_Alloc>::deallocate(_al, tail, 1);
            }
        }

    public:
        STD vector<STD optional<_control_block>> _m_nexts{ STD nullopt }; // 指向一些节点的指针们
        STD atomic<_skip_node_type> _m_type{ _skip_node_type::INVALID };
        union {
            value_type _m_data{};
            struct {
                uint8_t _m_max_level;
            };
        } _m_val;
    };
    template <class _Val_Ty>
    struct _skip_simple_types : tools::_simple_type< _Val_Ty> {
        using _node = _skip_node<_Val_Ty, void*>;
        using _node_ptr = _node*;
    };

    template <class _Val_Types>
    class _skiplist_val {
    public:
        using _node_ptr = typename _Val_Types::_node_ptr;
        using value_type = typename _Val_Types::value_type;
        using size_type = typename _Val_Types::size_type;
        using difference_type = typename _Val_Types::difference_type;
        using pointer = typename _Val_Types::pointer;
        using const_pointer = typename _Val_Types::const_pointer;
        using reference = value_type&;
        using const_reference = const value_type&;
        using const_iterator = _skip_const_iterator<_skiplist_val>;

        template <class _AllocNode>
        struct NODISCARD _erase_skip_and_orphan_guard {
            _skiplist_val* _val_ptr;
            _AllocNode& _al;
            _node_ptr _new_root;

            _erase_skip_and_orphan_guard& operator=(const _erase_skip_and_orphan_guard&) = delete;
            ~_erase_skip_and_orphan_guard() noexcept {
                if (_val_ptr != nullptr) {
                    _val_ptr->_erase_skiplist_and_orphan(_al, _new_root);
                }
            }
        };

        template <class _Alnode>
        void _erase_skiplist_and_orphan(_Alnode& _al, _node_ptr _root_node) noexcept {
            while (!_root_node->_is_nil) {
                _erase_skiplist_and_orphan();
            }
        }

        void _orphan_ptr(const _node_ptr _ptr) noexcept {
#if _DEBUG

#else
            (void)_ptr;
#endif 
        }

        // @function 插入函数
        // @param: prevs 是新节点的所有前驱, (prevs 中所有突出来的层和 new_node 的层高是相同的)
        _node_ptr _insert_node(STD vector<_node_ptr> prevs, _node_ptr new_node) noexcept {

            return new_node;
        }

        // @function: 只是修改标签
        void _remove(_node_ptr _node) {

        }

        // @function: 真正的删除节点
        void _real_remove(_node_ptr _node){}


    };
    template <typename _Traits>
    class _skiplist {
    public:
        using key_type       = typename _Traits::key_value;
        using value_type = typename _Traits::value_type;
        using allocator_type = typename _Traits::allocator_type;
    protected:
        using _alloc_type = tools::_rebind_alloc_t<allocator_type, value_type>;
        using _alloc_type_traits = STD allocator_traits<_alloc_type>;
        using _node = _skip_node<value_type, typename _alloc_type_traits::void_pointer>;
        using _alloc_node = tools::_rebind_alloc_t<allocator_type, _node>;
        using _alloc_node_traits = STD allocator_traits<_alloc_node>;
        using _node_ptr = typename _alloc_node_traits::pointer;
        using _val_type = _skiplist_val< STD conditional_t<tools::_is_simple_alloc_v<_alloc_node>, 
            _skip_simple_types<value_type>,
            _skip_iter_types<
                value_type,
                typename _alloc_type_traits::size_type,
                typename _alloc_type_traits::difference_type,
                typename _alloc_type_traits::pointer,
                typename _alloc_type_traits::const_pointer,
                _node_ptr
            >>
        >;
    public:
        using key_compare = typename _Traits::key_compare;
        using value_compare = typename _Traits::value_compare;
        using difference_type = typename _alloc_type_traits::difference_type;
        using pointer = typename _alloc_type_traits::pointer;
        using const_pointer = typename _alloc_type_traits::const_pointer;
        using size_type = typename _alloc_type_traits::size_type;
        using reference = value_type&;
        using const_reference = const value_type&;

        using iterator = _skip_iterator<_skiplist>;
        using const_iterator = _skip_const_iterator<_skiplist>;
        using reverse_iterator = STD reverse_iterator<iterator>;
        using const_reverse_iterator = STD reverse_iterator<const_iterator>;
    public:
        _skiplist() : _skiplist(_Alloc()) {};
        explicit _skiplist(const allocator_type& Alloc = _Alloc()) : _skiplist(Alloc) {

        }
        template <class Input>
        _skiplist(Input first, Input last, const allocator_type& alloc = _Alloc()) {}
        _skiplist(STD initializer_list<value_type> _init_list, const allocator_type& alloc = _Alloc()) {}

        _skiplist(const _skiplist& other) {}
        _skiplist(_skiplist&& other) {}

    public:
        
    public:
        _skiplist& operator=(const _skiplist& other) {}
        _skiplist& operator=(_skiplist&& other) {}


    };

}
