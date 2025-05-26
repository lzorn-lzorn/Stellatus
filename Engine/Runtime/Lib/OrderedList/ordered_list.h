// ordered_list.h
#include "../Include/Stellatus.h"
#include "skiplist.h"
#include <initializer_list>
#include <iterator>
#include <array>
#include <type_traits>
#include <optional>
#include <memory>
namespace Stellatus {    
    template <class _List>
    struct _list_const_iterator {
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
    
        _list_const_iterator& operator++() noexcept{
            this->_m_ptr = this->_m_ptr->_m_Next;
        }
    
        _list_const_iterator operator++(int) noexcept{
            _list_const_iterator tmp = *this;
            ++*this;
            return tmp;  
        }
    
        _list_const_iterator& operator--() noexcept{
            const auto new_ptr = this->_m_ptr->_m_Prev;
            this->_m_ptr = this->_m_ptr->_m_Next;
            return *this;
        }
    
        _list_const_iterator operator--(int) noexcept{
            _list_const_iterator tmp = *this;
            ++*this;
            return tmp;  
        }
    
        [[nodiscard]] bool operator==(const _list_const_iterator& other){
            return this->_m_ptr == other._m_ptr;
        }
    
        [[nodiscard]] bool operator!=(const _list_const_iterator& other){
            return !(*this == other);
        }
    
        void _seek_to(const _list_const_iterator<_List> iter) noexcept{
            this->_m_ptr = iter._m_ptr;
        }
    
        _node_ptr _m_ptr;
    };
    template <class _List>
struct _list_iterator : public _list_const_iterator<_List> {
    using _base_iter        = _list_const_iterator<_List>;
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

    _list_iterator& operator++() noexcept{
        _base_iter::operator++();
        return *this;
    }

    _list_iterator operator++(int) noexcept{
        _list_iterator tmp = *this;
        _base_iter::operator++();
        return tmp;  
    }

    _list_iterator& operator--() noexcept{
        _base_iter::operator--();
        return *this;
    }

    _list_iterator operator--(int) noexcept{
        _list_const_iterator tmp = *this;
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
struct _list_iter_types {
    using value_type      = _Value_Keype;
    using size_type       = _Size_Keype;
    using difference_type = _Difference_Keype;
    using pointer         = _Pointer;
    using const_pointer   = _Const_Pointer;
    using _node_ptr       = _node_ptr_Keype;
};
// @function: 实际的链表节点
template <typename _Val_Ty, typename _Voidptr>
struct _list_node{};


template <class _Val_Ty>
struct _list_simple_types : tools::_simple_type< _Val_Ty> {
    using _node = _list_node<_Val_Ty, void*>;
    using _node_ptr = _node*;
};


template <typename _Key, 
          typename _Val, 
          typename _Pr ,
          typename _Alloc
         >
struct ordered_list;

template <typename _Key,
          typename _Val,
          typename _Pr,
          typename _Alloc
         >
struct _ordered_list_helper{
    using key_value       = _Key;
    using value_type      = _Val;
    using key_compare     = _Pr;
    using allocatorr_type = _Alloc;

    template <typename... _Args>
    using _in_place_key_extractor = tools::_in_place_key_extract_ordered_list<_Key, _Args...>;

    struct value_compare {
    public:
        [[nodiscard]] bool operator()(const value_type& _Left, const value_type& _Right) const {
            return cmp (_Left.first, _Right.first);
        }
    protected:
        friend ordered_list<_Key, _Val, _Pr, _Alloc>;
        value_compare(key_compare _Pred) : cmp(_Pred) {}
        key_compare cmp;
    };
    // 用于某种扩展, 如果将来底层不用 STD pair 了, 换成 tuple 一样实现这个函数, 可以提取 key (get<0>())
    template <typename _Ty1, typename _Ty2>
    static const _Key& _Kfn(const STD pair<_Ty1, _Ty2>& val){
        return val.first;
    }

};
template <typename _Key, 
          typename _Val, 
          typename _Pr    = std::less<_Key>,
          typename _Alloc = std::allocator<STD pair<const _Key, _Val>>
         >
struct ordered_list : public _skiplist<_ordered_list_helper<_Key, _Val, _Pr, _Alloc>> {
private:
    using _base_type         = _skiplist<_ordered_list_helper<_Key, _Val, _Pr, _Alloc>>;
    using _alloc_type        = tools::_rebind_alloc_t<_Alloc, STD pair<_Key, STD optional<_Val>>>;
    using _alloc_type_traits = STD allocator_traits<_alloc_type>;
    using _node              = _list_node<STD pair<_Key, STD optional<_Val>>, typename std::allocator_traits<_Alloc>::void_pointer>;
    using _alloc_node        = tools::_rebind_alloc_t<_Key, _node>;
    using _alloc_node_traits = STD allocator_traits<_alloc_node>;
    using _node_ptr          = typename _alloc_node_traits::pointer;
    using _val_type          = std::conditional_t<tools::_is_simple_alloc_v<_alloc_node>, // 编译期的类型选择器
        _list_simple_types<_Key>,
        _list_iter_types<_Key,
                         typename _alloc_type_traits::size_type,
                         typename _alloc_type_traits::difference_type,
                         typename _alloc_type_traits::pointer,
                         typename _alloc_type_traits::const_pointer,
                         _node_ptr
        >
    >;
    public:
    // note: 这里之所以用 ::std::optional<_Val> 是因为对于跳表的头节点是不存数据的, 
    // note: 但是头阶段初始化的时候 需要给 _m_data 初始化为内存, 
    // note: 不同编译器对这个问题的处理是不一样的, 尤其是对于不支持默认构造的类型, 
    // note: 所以这里为了不依赖具体的编译器, 直接使用了 optional 做统一处理
    using intern_type     = _Val;
    using value_type      = STD pair<_Key, STD optional<_Val>>;
    using value_compare   = typename _base_type::value_compare;
    using key_type        = _Key;
    using key_compare     = _Pr;
    using difference_type = typename _base_type::difference_type;
    using pointer         = typename _base_type::pointer;
    using const_pointer   = typename _base_type::const_pointer;
    using size_type       = typename _base_type::size_type;
    using reference       = value_type&;
    using const_reference = const value_type&;

    using iterator               = _base_type::iterator;
    using const_iterator         = _base_type::const_iterator;
    using reverse_iterator       = STD reverse_iterator<iterator>;
    using const_reverse_iterator = STD reverse_iterator<const_iterator>;

    using _alloc_node        = typename _base_type::_alloc_node;
    using _alloc_node_traits = typename _base_type::_alloc_node_traits;
    public:
    ordered_list() : ordered_list(_Alloc()) {};
    explicit ordered_list(const _Alloc& Alloc = _Alloc()) : ordered_list(Alloc) {};
    template <class Input>
    ordered_list(Input first, Input last, const _Alloc& alloc = _Alloc()) {}
    ordered_list(STD initializer_list<value_type> _init_list, const _Alloc& alloc = _Alloc()){}

    ordered_list(const ordered_list& other) {}
    ordered_list(ordered_list&& other){}

public:
    ordered_list& operator=(const ordered_list& other){}
    ordered_list& operator=(ordered_list&& other){}

public:
    void assign(size_type count, const value_type& val){}
    template <class Input>
    void assign(Input first, Input last){}
    void assign(STD initializer_list<value_type> init_list){}

public:
    allocator_type get_allocator() const {}

public:
    reference front();
    const_reference front() const;
    reference back();
    const_reference back() const;
    public:
    iterator beign();
    const_iterator begin() const;
    const_iterator cbegin() const noexcept;
    reverse_iterator rbegin() const;
    const_reverse_iterator crbegin() const noexcept;
    iterator end();
    const_iterator end() const;
    const_iterator cend() const noexcept;
    iterator rend();
    const_iterator rend() const;
    const_iterator crend() const noexcept;

public:
    bool empty() const;
    size_type size() const;  //return std::distance(begin(), end())
    size_type max_size() const; 

public:
    void clear();
    public:
    iterator insert(const_iterator pos, const value_type& value);
    iterator insert(const_iterator pos, value_type&& value);
    iterator insert(const_iterator pos, size_type count, const value_type& value);
    template <class Input>
    iterator insert(const_iterator pos, Input first, Input last);
    iterator insert(const_iterator pos, std::initializer_list<value_type> init_list);
public:
    template <class... Args>
    iterator emplace(const_iterator pos, Args&&... args);
public:
    iterator erase(iterator pos);
    iterator erase(const_iterator pos);
    iterator erase(iterator first, iterator last);
    iterator erase(const_iterator first, const_iterator last);
public:
    void push_back(const value_type& value);
    void push_back(value_type&& value);

public:
    template <class... Args>
    void emplace_back(Args&&... args);
    template <class... Args>
    reference emplace_back(Args... args);

public:
    void pop_back();

public: 
    void push_front(const value_type& val);
    void push_front(value_type&& val);

public:
    template <class... Args>
    void emplace_front(Args&&... args);
    template <class... Args>
    reference emplace_front(Args&&... args);

public:
    void pop_front();
public:
    void resize(size_type count);
    void resize(size_type count, const value_type& val);

public:
    void swap(ordered_list& other) noexcept;
    public:
    void merge(ordered_list& other);
    void merge(ordered_list&& other);
    template <typename Compare>
    void merge(ordered_list& other, Compare cmp);
    template <typename Compare>
    void merge(ordered_list&& other, Compare cmp);

public:
    void splice(const_iterator pos, ordered_list& other);
    void splice(const_iterator pos, ordered_list&& other);
    void splice(const_iterator pos, ordered_list& other, const_iterator iter);
    void splice(const_iterator pos, ordered_list&& other, const_iterator iter);
    void splice(const_iterator pos, ordered_list& other, const_iterator first, const_iterator last);
    void splice(const_iterator pos, ordered_list&& other, const_iterator first, const_iterator last);

public:
    size_type remove(const value_type& val);
    template <class UnaryPred>
    size_type remove_if(UnaryPred pred);

public:
    size_type unique();
    template <typename BinaryPred>
    size_type unique(BinaryPred pred);
    public:
    // @function: 查找val的排名
    // @note: val可以不存在于表中
    size_type get_order_of(const value_type& val);
    // @function: 通过排名拿到元素
    value_type& get_val_by_order(const size_t& order);
    const value_type& get_val_by_order(const size_t& order) const;

    // @function: 查询 val 的前驱, val 的前驱为小于x的数中最大的数, 不存在返回整数最小值
    value_type& get_next(const value_type& val);
    const value_type& get_next(const value_type& val) const;    
    // @function: 查询 val 的后继, val 的后继为大于x的数中最大的数, 不存在返回整数最大值
    value_type& get_prev(const value_type& val);
    const value_type& get_prev(const value_type& val) const;    
private:


};

}