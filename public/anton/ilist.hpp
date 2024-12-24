#pragma once

#include <anton/iterators.hpp>
#include <anton/swap.hpp>
#include <anton/types.hpp>

namespace anton {
  // TODO: Change all external functions to have a template parameter for the
  //       type within the IList_Node instead of parameter Base.

  // IList_Node
  //
  // A tagged POD base type of IList's nodes.
  //
  template<typename T>
  struct IList_Node {
    IList_Node* prev;
    IList_Node* next;
  };

  using IList_DNode = IList_Node<void>;

  template<typename Base = IList_DNode, typename T>
  T* ilist_next(T* node)
  {
    return static_cast<T*>(static_cast<Base*>(node)->next);
  }

  template<typename Base = IList_DNode, typename T>
  T const* ilist_next(T const* node)
  {
    return static_cast<T const*>(static_cast<Base const*>(node)->next);
  }

  template<typename Base = IList_DNode, typename T>
  T* ilist_prev(T* node)
  {
    return static_cast<T*>(static_cast<Base*>(node)->prev);
  }

  template<typename Base = IList_DNode, typename T>
  T const* ilist_prev(T const* node)
  {
    return static_cast<T const*>(static_cast<Base const*>(node)->prev);
  }

  // ilist_size
  //
  // Calculates the size of the list by traversing the nodes.
  //
  // Returns:
  // The number of nodes in the list.
  //
  // Complexity:
  // O(n) where n is the number of nodes in the list.
  //
  template<typename Base = IList_DNode, typename T>
  isize ilist_size(T const* node)
  {
    isize size = 0;
    while(node != nullptr) {
      node = ilist_next(node);
      ++size;
    }
    return size;
  }

  // ilist_begin
  //
  // Find the begin of an intrusive list, that is a node whose the prev member
  // is nullptr. This is an external management function. The node must not be
  // a part of a list object.
  //
  // Parameters:
  // node - node of an intrusive list. May be nullptr.
  //
  // Returns:
  // The node whose the prev member is nullptr.
  //
  template<typename Base = IList_DNode, typename T>
  T* ilist_begin(T* node)
  {
    T* previous = node;
    while(node != nullptr) {
      previous = node;
      node = ilist_prev<Base>(node);
    }
    return previous;
  }

  template<typename Base = IList_DNode, typename T>
  T const* ilist_begin(T const* node)
  {
    T const* previous = node;
    while(node != nullptr) {
      previous = node;
      node = ilist_prev<Base>(node);
    }
    return previous;
  }

  // ilist_end
  //
  // Find the end of an intrusive list, that is a node whose the next member is
  // nullptr. This is an external management function. The node must not be a
  // part of a list object.
  //
  // Parameters:
  // node - node of an intrusive list. May be nullptr.
  //
  // Returns:
  // The node whose the next member is nullptr.
  //
  template<typename Base = IList_DNode, typename T>
  T* ilist_end(T* node)
  {
    T* previous = node;
    while(node != nullptr) {
      previous = node;
      node = ilist_next<Base>(node);
    }
    return previous;
  }

  template<typename Base = IList_DNode, typename T>
  T const* ilist_end(T const* node)
  {
    T const* previous = node;
    while(node != nullptr) {
      previous = node;
      node = ilist_next<Base>(node);
    }
    return previous;
  }

  // ilist_insert_before
  //
  // Insert a node into an intrusive list before position. This is an external
  // management function and does not require the list object.
  //
  // Parameters:
  // position - node before which to insert the new node.
  //     node - new node to be inserted.
  //
  template<typename T>
  void ilist_insert_before(IList_Node<T>* const position,
                           IList_Node<T>* const node)
  {
    IList_Node<T>* const prev = position->prev;
    position->prev = node;
    node->prev = prev;
    node->next = position;
    if(prev != nullptr) {
      prev->next = node;
    }
  }

  // ilist_insert_after
  //
  // Insert a node into an intrusive list after position. This is an external
  // management function and does not require the list object.
  //
  // Parameters:
  // position - node after which to insert the new node.
  //     node - new node to be inserted.
  //
  template<typename T>
  void ilist_insert_after(IList_Node<T>* const position,
                          IList_Node<T>* const node)
  {
    IList_Node<T>* const next = position->next;
    position->next = node;
    node->next = next;

    node->prev = position;
    if(next != nullptr) {
      next->prev = node;
    }
  }

  // ilist_erase
  //
  // Unlink node from its list. The links of the node are assumed to be valid.
  // If the links are invalid, the behaviour is undefined. This is an external
  // management function and does not require the list object.
  //
  // Parameters:
  // node - node to be removed.
  //
  template<typename T>
  void ilist_erase(IList_Node<T>* const node)
  {
    IList_Node<T>* const next = node->next;
    IList_Node<T>* const prev = node->prev;
    if(prev != nullptr) {
      prev->next = next;
    }
    if(next != nullptr) {
      next->prev = prev;
    }
  }

  // ilist_splice
  //
  // Attach the nodes of list2 to the end of list1. The nodes must point to
  // disjoint lists. This is an external management function. The nodes must
  // not be a part of any list object.
  //
  template<typename Base = IList_DNode, typename T>
  void ilist_splice(T* list1, T* list2)
  {
    Base* const end = ilist_end<Base>(list1);
    Base* const begin = ilist_begin<Base>(list2);
    end->next = begin;
    begin->prev = end;
  }

  // ilist_splice_after
  //
  // Attach the nodes of list2 after the node list1. The nodes must point to
  // disjoint lists. This is an external management function. The nodes must
  // not be a part of any list object.
  //
  template<typename Base = IList_DNode, typename T>
  void ilist_splice_after(T* list1, T* list2)
  {
    Base* const begin = ilist_begin<Base>(list2);
    Base* const end = ilist_end<Base>(list2);
    Base* const next = ilist_next<Base>(list1);
    Base* const prev = ilist_prev<Base>(list1);
    if(next != nullptr) {
      end->next = next;
      next->prev = end;
    }

    if(prev != nullptr) {
      begin->prev = prev;
      prev->next = begin;
    }
  }

  // IList_Iterator
  //
  // Default constructed iterator is invalid. Since this is an intrusive
  // iterator list, the value type is also the node type.
  //
  // Parameters:
  //      Node_Type - node type of the list.
  // Base_Node_Type - base type of the node type.
  //
  template<typename Node_Type, typename Base_Node_Type>
  struct IList_Iterator {
  public:
    using value_type = Node_Type;
    using node_type = Node_Type;
    using pointer = value_type*;
    using reference = value_type&;
    using difference_type = i64;
    using iterator_category = Bidirectional_Iterator_Tag;

    Base_Node_Type* node = nullptr;

    IList_Iterator() = default;
    IList_Iterator(Base_Node_Type* node): node(node) {}
    IList_Iterator(IList_Iterator const&) = default;
    IList_Iterator(IList_Iterator&&) = default;
    ~IList_Iterator() = default;
    IList_Iterator& operator=(IList_Iterator const&) = default;
    IList_Iterator& operator=(IList_Iterator&&) = default;

    // Conversion operator to the const version of the iterator.
    [[nodiscard]]
    operator IList_Iterator<Node_Type const, Base_Node_Type const>() const
    {
      return node;
    }

    [[nodiscard]] pointer operator->() const
    {
      return static_cast<pointer>(node);
    }

    [[nodiscard]] reference operator*() const
    {
      return *static_cast<pointer>(node);
    }

    IList_Iterator& operator++()
    {
      node = node->next;
      return *this;
    }

    [[nodiscard]] IList_Iterator operator++(int)
    {
      IList_Iterator v{node};
      node = node->next;
      return v;
    }

    IList_Iterator& operator--()
    {
      node = node->prev;
      return *this;
    }

    [[nodiscard]] IList_Iterator operator--(int)
    {
      IList_Iterator v{node};
      node = node->prev;
      return v;
    }

    [[nodiscard]] bool operator==(IList_Iterator const& other) const
    {
      return node == other.node;
    }

    [[nodiscard]] bool operator!=(IList_Iterator const& other) const
    {
      return node != other.node;
    }
  };

  // IList
  //
  // Similarly to List, the container provides address-stable node-based
  // storage with bidirectional iteration. However, there are differences:
  // - The node information is stored directly in the data structures.
  // - Allocation and deallocation of the nodes is managed by the user - the
  //   container does no memory allocations and does not deallocate anything
  //   as it works with user-supplied data.
  // - Does not support copy construction or assignment.
  // - Unlinks the nodes from itself on destruction making no further changes
  //   to the list. This preserves the chain structure of the list with ends
  //   unlinked.
  //
  template<typename Node, typename Tag = void>
  struct IList {
  public:
    using size_type = i64;
    using value_type = Node;
    using node_type = Node;
    using base_node_type = IList_Node<Tag>;
    using reference = Node&;
    using pointer = Node*;
    using iterator = IList_Iterator<node_type, base_node_type>;
    using const_iterator =
      IList_Iterator<node_type const, base_node_type const>;

  private:
    base_node_type _internal_node;

  public:
    IList();
    IList(IList const& other) = delete;
    IList(IList&& other);
    IList& operator=(IList const& other) = delete;
    IList& operator=(IList&& other);
    ~IList();

    // IList
    //
    // Construct an IList from an unlinked chain of nodes.
    //
    IList(base_node_type* first, base_node_type* last);

    [[nodiscard]] iterator begin();
    [[nodiscard]] iterator end();
    [[nodiscard]] const_iterator begin() const;
    [[nodiscard]] const_iterator end() const;
    [[nodiscard]] const_iterator cbegin() const;
    [[nodiscard]] const_iterator cend() const;

    [[nodiscard]] pointer front() const;
    [[nodiscard]] pointer back() const;

    // size
    //
    // Calculates the size of the list by traversing the nodes.
    //
    // Returns:
    // The number of nodes in the list.
    //
    // Complexity:
    // O(n) where n is the number of nodes in the list.
    //
    [[nodiscard]] size_type size() const;

    // empty
    //
    // Checks whether the list contains no elements, i.e. size() == 0.
    //
    // Complexity:
    // O(1)
    //
    // Returns:
    // true when the list is empty (contains no elements).
    //
    [[nodiscard]] bool empty() const;

    // insert
    //
    // Insert a node before position.
    //
    // Parameters:
    // position - iterator to the node before which to insert the new nodes.
    //     node - the new node to be inserted.
    //
    // Returns:
    // Iterator to the inserted node.
    //
    iterator insert(const_iterator position, base_node_type& node);

    // insert_front
    //
    // Insert a node at the front of the list.
    //
    // Parameters:
    // node - the new node to be inserted.
    //
    // Returns:
    // Iterator to the inserted node.
    //
    iterator insert_front(base_node_type& node);
    iterator insert_front(base_node_type* node);

    // insert_back
    //
    // Insert a node at the back of the list.
    //
    // Parameters:
    // node - the new node to be inserted.
    //
    // Returns:
    // Iterator to the inserted node.
    //
    iterator insert_back(base_node_type& node);
    iterator insert_back(base_node_type* node);

    // splice
    //
    // Attach the nodes of the other list to the end of this list. Unlinks the
    // other list.
    //
    void splice(IList& other);
    void splice(base_node_type* other);

    // erase
    //
    // Erase the node at position.
    //
    // Parameters:
    // position - iterator to the node to erase.
    //
    void erase(const_iterator position);

    // erase
    //
    // Erase a range of nodes defined by [first, last[.
    //
    // Parameters:
    // first - the iterator to the beginning of the range.
    //  last - the iterator past the end of the range.
    //
    void erase(const_iterator first, const_iterator last);

    // erase_front
    //
    // Erase the first node from the list.
    // If the list is empty, the behaviour is undefined.
    //
    void erase_front();

    // erase_back
    //
    // Erase the last node from the list.
    // If the list is empty, the behaviour is undefined.
    //
    void erase_back();

    // unlink
    //
    // Unlink (disconnect) the nodes from the list while maintaining their chain
    // structure.
    //
    // Returns:
    // The first node of the unlinked list.
    //
    [[nodiscard]] pointer unlink();

    // clear
    //
    // Unlink all nodes present in the list.
    //
    void clear();

    // swap
    //
    // Swaps the contents of 2 lists.
    //
    // Compexity:
    // O(1)
    //
    friend void swap(IList& list1, IList& list2)
    {
      using anton::swap;
      // We swap the internal nodes and then fix up the addresses.
      swap(list1._internal_node, list2._internal_node);
      if(list1._internal_node.next == &list2._internal_node) {
        list1._internal_node.next = &list1._internal_node;
        list1._internal_node.prev = &list1._internal_node;
      } else {
        list1._internal_node.next->prev = &list1._internal_node;
        list1._internal_node.prev->next = &list1._internal_node;
      }

      if(list2._internal_node.next == &list1._internal_node) {
        list2._internal_node.next = &list2._internal_node;
        list2._internal_node.prev = &list2._internal_node;
      } else {
        list2._internal_node.next->prev = &list2._internal_node;
        list2._internal_node.prev->next = &list2._internal_node;
      }
    }
  };

  template<typename Node, typename Tag>
  IList<Node, Tag>::IList()
  {
    base_node_type* const internal_node = &_internal_node;
    _internal_node.next = internal_node;
    _internal_node.prev = internal_node;
  }

  template<typename Node, typename Tag>
  IList<Node, Tag>::IList(IList&& other)
  {
    base_node_type* const internal_node = &_internal_node;
    _internal_node.next = internal_node;
    _internal_node.prev = internal_node;
    swap(*this, other);
  }

  template<typename Node, typename Tag>
  IList<Node, Tag>& IList<Node, Tag>::operator=(IList&& other)
  {
    clear();
    swap(*this, other);
  }

  template<typename Node, typename Tag>
  IList<Node, Tag>::~IList()
  {
    clear();
  }

  template<typename Node, typename Tag>
  IList<Node, Tag>::IList(base_node_type* first, base_node_type* last)
  {
    base_node_type* const internal_node = &_internal_node;
    first->prev = internal_node;
    internal_node->next = first;
    last->next = internal_node;
    internal_node->prev = last;
  }

  template<typename Node, typename Tag>
  auto IList<Node, Tag>::begin() -> iterator
  {
    return static_cast<node_type*>(_internal_node.next);
  }

  template<typename Node, typename Tag>
  auto IList<Node, Tag>::end() -> iterator
  {
    return static_cast<node_type*>(&_internal_node);
  }

  template<typename Node, typename Tag>
  auto IList<Node, Tag>::begin() const -> const_iterator
  {
    return static_cast<node_type const*>(_internal_node.next);
  }

  template<typename Node, typename Tag>
  auto IList<Node, Tag>::end() const -> const_iterator
  {
    return static_cast<node_type const*>(&_internal_node);
  }

  template<typename Node, typename Tag>
  auto IList<Node, Tag>::cbegin() const -> const_iterator
  {
    return static_cast<node_type const*>(_internal_node.next);
  }

  template<typename Node, typename Tag>
  auto IList<Node, Tag>::cend() const -> const_iterator
  {
    return static_cast<node_type const*>(&_internal_node);
  }

  template<typename Node, typename Tag>
  auto IList<Node, Tag>::front() const -> pointer
  {
    ANTON_ASSERT(&_internal_node != _internal_node.next,
                 "front called on empty IList");
    return static_cast<node_type const*>(&_internal_node->next);
  }

  template<typename Node, typename Tag>
  auto IList<Node, Tag>::back() const -> pointer
  {
    ANTON_ASSERT(&_internal_node != _internal_node.prev,
                 "back called on empty IList");
    return static_cast<node_type const*>(&_internal_node->prev);
  }

  template<typename Node, typename Tag>
  auto IList<Node, Tag>::size() const -> size_type
  {
    size_type _size = 0;
    base_node_type const* const internal_node = &_internal_node;
    base_node_type const* node = internal_node->next;
    while(node != internal_node) {
      node = node->next;
      ++_size;
    }
    return _size;
  }

  template<typename Node, typename Tag>
  bool IList<Node, Tag>::empty() const
  {
    base_node_type const* const internal_node = &_internal_node;
    return internal_node->next == internal_node;
  }

  template<typename Node, typename Tag>
  auto IList<Node, Tag>::insert(const_iterator position,
                                base_node_type& node) -> iterator
  {
    base_node_type* const next = position.node;
    base_node_type* const prev = next->prev;
    node.prev = prev;
    prev->next = &node;
    node.next = next;
    next->prev = &node;
    return &node;
  }

  template<typename Node, typename Tag>
  auto IList<Node, Tag>::insert_front(base_node_type& node) -> iterator
  {
    base_node_type* const next = _internal_node.next;
    base_node_type* const prev = next->prev;
    node.prev = prev;
    prev->next = &node;
    node.next = next;
    next->prev = &node;
    return &node;
  }

  template<typename Node, typename Tag>
  auto IList<Node, Tag>::insert_front(base_node_type* node) -> iterator
  {
    ANTON_ASSERT(node != nullptr, "cannot insert nullptr into IList");
    base_node_type* const next = _internal_node.next;
    base_node_type* const prev = next->prev;
    node->prev = prev;
    prev->next = node;
    node->next = next;
    next->prev = node;
    return node;
  }

  template<typename Node, typename Tag>
  auto IList<Node, Tag>::insert_back(base_node_type& node) -> iterator
  {
    base_node_type* const next = &_internal_node;
    base_node_type* const prev = next->prev;
    node.prev = prev;
    prev->next = &node;
    node.next = next;
    next->prev = &node;
    return &node;
  }

  template<typename Node, typename Tag>
  auto IList<Node, Tag>::insert_back(base_node_type* node) -> iterator
  {
    ANTON_ASSERT(node != nullptr, "cannot insert nullptr into IList");
    base_node_type* const next = &_internal_node;
    base_node_type* const prev = next->prev;
    node->prev = prev;
    prev->next = node;
    node->next = next;
    next->prev = node;
    return node;
  }

  template<typename Node, typename Tag>
  void IList<Node, Tag>::splice(IList& other)
  {
    auto const b = other.begin();
    auto const e = other.end();
    if(b != e) {
      base_node_type* const first = b.node;
      base_node_type* const last = e.node->prev;
      other.clear();
      base_node_type* const our_last = _internal_node.prev;
      first->prev = our_last;
      our_last->next = first;
      last->next = &_internal_node;
      _internal_node.prev = last;
    }
  }

  template<typename Node, typename Tag>
  void IList<Node, Tag>::splice(base_node_type* other)
  {
    ANTON_ASSERT(other != nullptr, "other must not be nullptr");
    auto const first = ilist_begin(other);
    auto const last = ilist_end(other);
    base_node_type* const our_last = _internal_node.prev;
    base_node_type* const our_first = &_internal_node;
    first->prev = our_last;
    our_last->next = first;
    last->next = our_first;
    our_first->prev = last;
  }

  template<typename Node, typename Tag>
  void IList<Node, Tag>::erase(const_iterator position)
  {
    base_node_type* const node = position.node;
    base_node_type* const next = node->next;
    base_node_type* const prev = node->prev;
    next->prev = prev;
    prev->next = next;
  }

  template<typename Node, typename Tag>
  void IList<Node, Tag>::erase(const_iterator first, const_iterator last)
  {
    base_node_type* const next = last.node->next;
    base_node_type* const prev = first.node->prev;
    next->prev = prev;
    prev->next = next;
  }

  template<typename Node, typename Tag>
  void IList<Node, Tag>::erase_front()
  {
    base_node_type* const prev = &_internal_node;
    base_node_type* const node = prev->next;
    base_node_type* const next = node->next;
    next->prev = prev;
    prev->next = next;
  }

  template<typename Node, typename Tag>
  void IList<Node, Tag>::erase_back()
  {
    base_node_type* const next = &_internal_node;
    base_node_type* const node = next->prev;
    base_node_type* const prev = node->prev;
    next->prev = prev;
    prev->next = next;
  }

  template<typename Node, typename Tag>
  typename IList<Node, Tag>::pointer IList<Node, Tag>::unlink()
  {
    base_node_type* const internal_node = &_internal_node;
    base_node_type* const next = internal_node->next;
    base_node_type* const prev = internal_node->prev;
    next->prev = nullptr;
    prev->next = nullptr;
    _internal_node.next = internal_node;
    _internal_node.prev = internal_node;
    if(next != internal_node) {
      return static_cast<pointer>(next);
    } else {
      return nullptr;
    }
  }

  template<typename Node, typename Tag>
  void IList<Node, Tag>::clear()
  {
    base_node_type* const internal_node = &_internal_node;
    base_node_type* const next = internal_node->next;
    base_node_type* const prev = internal_node->prev;
    next->prev = nullptr;
    prev->next = nullptr;
    _internal_node.next = internal_node;
    _internal_node.prev = internal_node;
  }
} // namespace anton
