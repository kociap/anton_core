#pragma once

#include <anton/allocator.hpp>
#include <anton/assert.hpp>
#include <anton/iterators.hpp>
#include <anton/memory.hpp>
#include <anton/swap.hpp>
#include <anton/tags.hpp>
#include <anton/type_traits/utility.hpp>

namespace anton {
    template<typename T>
    struct List_Node {
        List_Node* prev = nullptr;
        List_Node* next = nullptr;
        T data;

        template<typename... Args>
        List_Node(Args&&... args): data(ANTON_FWD(args)...) {}
    };

    // List_Iterator
    // Default constructed iterator is invalid.
    //
    // Parameters:
    // Value_Type - qualified type returned by operator* and operator->.
    //  Data_Type - unqualified type stored by List_Node.
    //
    template<typename Value_Type, typename Data_Type>
    struct List_Iterator {
    public:
        using value_type = Value_Type;
        using node_type = List_Node<Data_Type>;
        using iterator_category = Bidirectional_Iterator_Tag;

        node_type* node = nullptr;

        List_Iterator() = default;
        List_Iterator(node_type* node): node(node) {}
        List_Iterator(List_Iterator const&) = default;
        List_Iterator(List_Iterator&&) = default;
        ~List_Iterator() = default;
        List_Iterator& operator=(List_Iterator const&) = default;
        List_Iterator& operator=(List_Iterator&&) = default;

        // Conversion operator to the const version of the iterator
        [[nodiscard]] operator List_Iterator<Value_Type const, Data_Type>() const {
            return node;
        }

        [[nodiscard]] value_type* operator->() const {
            return &node->data;
        }

        [[nodiscard]] value_type& operator*() const {
            return node->data;
        }

        List_Iterator& operator++() {
            node = node->next;
            return *this;
        }

        [[nodiscard]] List_Iterator operator++(int) {
            List_Iterator v{node};
            node = node->next;
            return v;
        }

        List_Iterator& operator--() {
            node = node->prev;
            return *this;
        }

        [[nodiscard]] List_Iterator operator--(int) {
            List_Iterator v{node};
            node = node->prev;
            return v;
        }

        [[nodiscard]] bool operator==(List_Iterator const& other) const {
            return node == other.node;
        }

        [[nodiscard]] bool operator!=(List_Iterator const& other) const {
            return node != other.node;
        }
    };

    // List
    // List is a node-based container that guarantees address stability of the elements it owns.
    // It supports constant time insertion and removal from anywhere inside the container,
    // but does not allow random access. Provides bidirectional iteration over the elements.
    // An iterator is only invalidated when its corresponding element is deallocated.
    // Operations such as adding, relocating or removing performed on elements within the list
    // do not invalidate the iterators or references to unaffected elements.
    //
    // Allocators are copied/moved/swapped during construction/assignment/swap operations.
    // When the allocators do not compare equal, all elements are copied and constructed
    // using the destination allocator.
    //
    template<typename T, typename Allocator = Allocator>
    struct List {
    public:
        using value_type = T;
        using allocator_type = Allocator;
        using size_type = i64;
        using node_type = List_Node<T>;
        using iterator = List_Iterator<T, T>;
        using const_iterator = List_Iterator<T const, T>;

    private:
        struct alignas(alignof(node_type)) Internal_Node {
            node_type* prev = nullptr;
            node_type* next = nullptr;
        };

        static_assert(alignof(node_type) == alignof(Internal_Node), "List_Node<T> and Internal_Node alignments differ");

        Internal_Node _internal_node;
        Allocator _allocator;

    public:
        List();
        explicit List(allocator_type const& allocator);
        List(List const& other);
        List(List const& other, allocator_type const& allocator);
        List(List&& other);
        List(List&& other, allocator_type const& allocator);
        template<typename... Args>
        List(Variadic_Construct_Tag, Args&&... args);
        template<typename Input_Iterator>
        List(Range_Construct_Tag, Input_Iterator first, Input_Iterator last);
        template<typename Input_Iterator>
        List(Range_Construct_Tag, Input_Iterator first, Input_Iterator last, allocator_type const& allocator);
        ~List();

        List& operator=(List const& other);
        List& operator=(List&& other);

        // get_allocator
        // Obtain the currently used allocator.
        //
        // Returns:
        // Reference to the allocator.
        //
        [[nodiscard]] allocator_type& get_allocator();
        [[nodiscard]] allocator_type const& get_allocator() const;

        // set_allocator
        // Change the currently used allocator.
        // Be careful as replacing allocators might have
        // unwanted or undefined consequences.
        //
        // Parameters:
        // allocator - the new allocator to be used.
        //
        void set_allocator(allocator_type const& allocator);
        void set_allocator(allocator_type&& allocator);

        [[nodiscard]] iterator begin();
        [[nodiscard]] iterator end();
        [[nodiscard]] const_iterator begin() const;
        [[nodiscard]] const_iterator end() const;
        [[nodiscard]] const_iterator cbegin() const;
        [[nodiscard]] const_iterator cend() const;

        // size
        // Calculates the size of the list by traversing the nodes.
        //
        // Returns:
        // The number of nodes in the list.
        //
        // Complexity:
        // O(n) where n is the number of nodes in the list.
        //
        [[nodiscard]] size_type size() const;

        // assign
        // Replaces the contents of the list with the elements from [first, last[.
        // All iterators besides the end iterator are invalidated.
        //
        // Parameters:
        // first - the iterator to the beginning of the range from which
        //         the elements will be constructed.
        //  last - the iterator past the end of the range.
        //
        // Complexity:
        // Linear in the distance between first and last.
        //
        template<typename Input_Iterator>
        void assign(Input_Iterator first, Input_Iterator last);

        // emplace
        // Construct an element in place and insert before position.
        // The arguments are forwarded to the constructor of the element.
        //
        // Parameters:
        // position - iterator before which to construct the new element.
        //     args - arguments to forward to the constructor of the element.
        //
        // Returns:
        // Iterator to the emplaced element.
        //
        template<typename... Args>
        iterator emplace(const_iterator position, Args&&... args);

        // emplace_front
        // Construct an element in place and insert at the beginning of the list.
        // The arguments are forwarded to the constructor of the element.
        //
        // Parameters:
        // args - arguments to forward to the constructor of the element.
        //
        // Returns:
        // Iterator to the emplaced element.
        //
        template<typename... Args>
        iterator emplace_front(Args&&... args);

        // emplace_back
        // Construct an element in place and insert at the end of the list.
        // The arguments are forwarded to the constructor of the element.
        //
        // Parameters:
        // args - arguments to forward to the constructor of the element.
        //
        // Returns:
        // Iterator to the emplaced element.
        //
        template<typename... Args>
        iterator emplace_back(Args&&... args);

        // insert
        // Construct elements from [first, last[ and insert before position.
        //
        // Parameters:
        // position - iterator to the node before which to insert the new nodes.
        //    first - the iterator to the beginning of the range from which
        //            the elements will be constructed.
        //     last - the iterator past the end of the range.
        //
        // Returns:
        // Iterator to the first of the inserted nodes.
        //
        template<typename Input_Iterator>
        iterator insert(const_iterator position, Input_Iterator first, Input_Iterator last);

        // erase
        // Erase the node at position.
        // Invalidates all iterators to the node.
        //
        // Parameters:
        // position - iterator to the node to erase.
        //
        void erase(const_iterator position);

        // erase
        // Erase a range of nodes defined by [first, last[.
        // Invalidates all the iterators to the nodes in the range.
        //
        // Parameters:
        // first - the iterator to the beginning of the range.
        //  last - the iterator past the end of the range.
        //
        void erase(const_iterator first, const_iterator last);

        // erase_front
        // Erase the first node from the list.
        // Invalidates all iterators to the first node.
        // If the list is empty, the behaviour is undefined.
        //
        void erase_front();

        // erase_back
        // Erase the last node from the list.
        // Invalidates all iterators to the last node. Does not invalidate the end.
        // If the list is empty, the behaviour is undefined.
        //
        void erase_back();

        // clear
        // Destroys all the nodes present in the container restoring it to
        // a default constructed state.
        //
        void clear();

        // swap
        // Swaps the contents of 2 lists.
        // If the allocators compare equal, this function swaps
        // the allocators and repoints the nodes.
        // Otherwise, it performs a full copy of the nodes.
        //
        // Compexity:
        // O(1) if the allocators compare equal.
        // O(n) if the allocators compare not equal.
        //
        friend void swap(List& list1, List& list2) {
            using anton::swap;
            if(list1._allocator == list2._allocator) {
                swap(list1._allocator, list2._allocator);
                // We swap the internal nodes and then fix up the addresses
                swap(list1._internal_node, list2._internal_node);
                if(list1._internal_node.next == (node_type*)&list2._internal_node) {
                    list1._internal_node.next = (node_type*)&list1._internal_node;
                    list1._internal_node.prev = (node_type*)&list1._internal_node;
                } else {
                    list1._internal_node.next->prev = (node_type*)&list1._internal_node;
                    list1._internal_node.prev->next = (node_type*)&list1._internal_node;
                }

                if(list2._internal_node.next == (node_type*)&list1._internal_node) {
                    list2._internal_node.next = (node_type*)&list2._internal_node;
                    list2._internal_node.prev = (node_type*)&list2._internal_node;
                } else {
                    list2._internal_node.next->prev = (node_type*)&list2._internal_node;
                    list2._internal_node.prev->next = (node_type*)&list2._internal_node;
                }
            } else {
                // Move operations call swap, which could lead to an endless loop
                List<T, Allocator> temp{list1};
                list1 = list2;
                list2 = temp;
            }
        }
    };

    template<typename T, typename Allocator>
    List<T, Allocator>::List(): List(allocator_type()) {}

    template<typename T, typename Allocator>
    List<T, Allocator>::List(allocator_type const& allocator): _allocator(allocator) {
        node_type* const internal_node = (node_type*)&_internal_node;
        internal_node->next = internal_node;
        internal_node->prev = internal_node;
    }

    template<typename T, typename Allocator>
    List<T, Allocator>::List(List const& other): List(other, other._allocator) {}

    template<typename T, typename Allocator>
    List<T, Allocator>::List(List const& other, allocator_type const& allocator): _allocator(allocator) {
        node_type* const internal_node = (node_type*)&_internal_node;
        internal_node->next = internal_node;
        internal_node->prev = internal_node;
        const_iterator p{internal_node};
        insert(p, other.begin(), other.end());
    }

    template<typename T, typename Allocator>
    List<T, Allocator>::List(List&& other): _allocator(ANTON_MOV(other._allocator)) {
        node_type* const internal_node = (node_type*)&_internal_node;
        internal_node->next = internal_node;
        internal_node->prev = internal_node;
        // swap takes care of the allocator business
        swap(*this, other);
    }

    template<typename T, typename Allocator>
    List<T, Allocator>::List(List&& other, allocator_type const& allocator): _allocator(allocator) {
        node_type* const internal_node = (node_type*)&_internal_node;
        internal_node->next = internal_node;
        internal_node->prev = internal_node;
        // swap takes care of the allocator business
        swap(*this, other);
    }

    template<typename T, typename Allocator>
    template<typename... Args>
    List<T, Allocator>::List(Variadic_Construct_Tag, Args&&... args) {
        node_type* const internal_node = (node_type*)&_internal_node;
        internal_node->next = internal_node;
        internal_node->prev = internal_node;

        node_type* node = internal_node;
        node_type* new_node = nullptr;
        ((new_node = (node_type*)_allocator.allocate(sizeof(node_type), alignof(node_type)), construct(new_node, ANTON_FWD(args)), // Construct a new node
          new_node->prev = node, node->next = new_node, // Insert the new node into the list
          node = new_node),
         ...);
        // Point the last node to end
        internal_node->prev = node;
        node->next = internal_node;
    }

    template<typename T, typename Allocator>
    template<typename Input_Iterator>
    List<T, Allocator>::List(Range_Construct_Tag, Input_Iterator first, Input_Iterator last): List(range_construct, first, last, allocator_type()) {}

    template<typename T, typename Allocator>
    template<typename Input_Iterator>
    List<T, Allocator>::List(Range_Construct_Tag, Input_Iterator first, Input_Iterator last, allocator_type const& allocator): _allocator(allocator) {
        node_type* const internal_node = (node_type*)&_internal_node;
        internal_node->next = internal_node;
        internal_node->prev = internal_node;

        const_iterator p{internal_node};
        insert(p, first, last);
    }

    template<typename T, typename Allocator>
    List<T, Allocator>::~List() {
        clear();
    }

    template<typename T, typename Allocator>
    List<T, Allocator>& List<T, Allocator>::operator=(List const& other) {
        ANTON_ASSERT(this != &other, "self-assignment");
        assign(other.begin(), other.end());
        return *this;
    }

    template<typename T, typename Allocator>
    List<T, Allocator>& List<T, Allocator>::operator=(List&& other) {
        ANTON_ASSERT(this != &other, "self-assignment");
        // swap takes care of the allocator business
        swap(*this, other);
        return *this;
    }

    template<typename T, typename Allocator>
    auto List<T, Allocator>::get_allocator() -> allocator_type& {
        return _allocator;
    }

    template<typename T, typename Allocator>
    auto List<T, Allocator>::get_allocator() const -> allocator_type const& {
        return _allocator;
    }

    template<typename T, typename Allocator>
    void List<T, Allocator>::set_allocator(allocator_type const& allocator) {
        _allocator = allocator;
    }

    template<typename T, typename Allocator>
    void List<T, Allocator>::set_allocator(allocator_type&& allocator) {
        _allocator = ANTON_MOV(allocator);
    }

    template<typename T, typename Allocator>
    auto List<T, Allocator>::begin() -> iterator {
        return ((node_type*)&_internal_node)->next;
    }

    template<typename T, typename Allocator>
    auto List<T, Allocator>::end() -> iterator {
        return (node_type*)&_internal_node;
    }

    template<typename T, typename Allocator>
    auto List<T, Allocator>::begin() const -> const_iterator {
        return ((node_type*)&_internal_node)->next;
    }

    template<typename T, typename Allocator>
    auto List<T, Allocator>::end() const -> const_iterator {
        return (node_type*)&_internal_node;
    }

    template<typename T, typename Allocator>
    auto List<T, Allocator>::cbegin() const -> const_iterator {
        return ((node_type*)&_internal_node)->next;
    }

    template<typename T, typename Allocator>
    auto List<T, Allocator>::cend() const -> const_iterator {
        return (node_type*)&_internal_node;
    }

    template<typename T, typename Allocator>
    auto List<T, Allocator>::size() const -> size_type {
        size_type _size = 0;
        node_type* const internal_node = (node_type*)&_internal_node;
        node_type* node = internal_node->next;
        while(node != internal_node) {
            node = node->next;
            ++_size;
        }
        return _size;
    }

    template<typename T, typename Allocator>
    template<typename Input_Iterator>
    void List<T, Allocator>::assign(Input_Iterator first, Input_Iterator last) {
        node_type* const internal_node = (node_type*)&_internal_node;
        node_type* node = internal_node->next;

        for(; node != internal_node && first != last; node = node->next, ++first) {
            node->data = *first;
        }

        if(first == last) {
            const_iterator b{node};
            const_iterator e{internal_node};
            erase(b, e);
        } else {
            const_iterator e{internal_node};
            insert(e, first, last);
        }
    }

    template<typename T, typename Allocator>
    template<typename... Args>
    auto List<T, Allocator>::emplace(const_iterator position, Args&&... args) -> iterator {
        node_type* const node = (node_type*)_allocator.allocate(sizeof(node_type), alignof(node_type));
        construct(node, ANTON_FWD(args)...);
        node->prev = position.node->prev;
        node->prev->next = node;
        node->next = position.node;
        node->next->prev = node;
        return node;
    }

    template<typename T, typename Allocator>
    template<typename... Args>
    auto List<T, Allocator>::emplace_front(Args&&... args) -> iterator {
        node_type* const internal_node = (node_type*)&_internal_node;
        node_type* const node = (node_type*)_allocator.allocate(sizeof(node_type), alignof(node_type));
        construct(node, ANTON_FWD(args)...);
        node->prev = internal_node;
        node->next = internal_node->next;
        node->prev->next = node;
        node->next->prev = node;
        return node;
    }

    template<typename T, typename Allocator>
    template<typename... Args>
    auto List<T, Allocator>::emplace_back(Args&&... args) -> iterator {
        node_type* const internal_node = (node_type*)&_internal_node;
        node_type* const node = (node_type*)_allocator.allocate(sizeof(node_type), alignof(node_type));
        construct(node, ANTON_FWD(args)...);
        node->prev = internal_node->prev;
        node->next = internal_node;
        node->prev->next = node;
        node->next->prev = node;
        return node;
    }

    template<typename T, typename Allocator>
    template<typename Input_Iterator>
    auto List<T, Allocator>::insert(const_iterator position, Input_Iterator first, Input_Iterator last) -> iterator {
        if(first == last) {
            return position.node;
        }

        node_type* const first_node = (node_type*)_allocator.allocate(sizeof(node_type), alignof(node_type));
        construct(first_node, *first);
        ++first;

        node_type* current = first_node;
        while(first != last) {
            node_type* const node = (node_type*)_allocator.allocate(sizeof(node_type), alignof(node_type));
            construct(node, *first);
            current->next = node;
            node->prev = current;
            current = node;
            ++first;
        }

        first_node->prev = position.node->prev;
        first_node->prev->next = first_node;
        current->next = position.node;
        current->next->prev = current;
        return first_node;
    }

    template<typename T, typename Allocator>
    void List<T, Allocator>::erase(const_iterator position) {
        if constexpr(ANTON_ITERATOR_DEBUG) {
            ANTON_FAIL(position.node != (node_type*)&_internal_node, "cannot erase end node");
        }

        node_type* const node = position.node;
        node->prev->next = node->next;
        node->next->prev = node->prev;
        destruct(node);
        _allocator.deallocate(node, sizeof(node_type), alignof(node_type));
    }

    template<typename T, typename Allocator>
    void List<T, Allocator>::erase(const_iterator first, const_iterator last) {
        node_type* first_node = first.node;
        node_type* last_node = last.node;

        last_node->prev = first_node->prev;
        last_node->prev->next = last_node;

        while(first_node != last_node) {
            node_type* current = first_node;
            first_node = first_node->next;
            destruct(current);
            _allocator.deallocate(current, sizeof(node_type), alignof(node_type));
        }
    }

    template<typename T, typename Allocator>
    void List<T, Allocator>::erase_front() {
        node_type* const internal_node = (node_type*)&_internal_node;
        ANTON_ASSERT(internal_node->next != internal_node, "cannot erase from an empty list");
        node_type* node = internal_node->next;
        internal_node->next = node->next;
        node->next->prev = internal_node;
        destruct(node);
        _allocator.deallocate(node, sizeof(node_type), alignof(node_type));
    }

    template<typename T, typename Allocator>
    void List<T, Allocator>::erase_back() {
        node_type* const internal_node = (node_type*)&_internal_node;
        ANTON_ASSERT(internal_node->next != internal_node, "cannot erase from an empty list");
        node_type* node = internal_node->prev;
        internal_node->prev = node->prev;
        node->prev->next = internal_node;
        destruct(node);
        _allocator.deallocate(node, sizeof(node_type), alignof(node_type));
    }

    template<typename T, typename Allocator>
    void List<T, Allocator>::clear() {
        node_type* const internal_node = (node_type*)&_internal_node;
        node_type* node = internal_node->next;
        while(node != internal_node) {
            node_type* current = node;
            node = node->next;
            destruct(current);
            _allocator.deallocate(current, sizeof(node_type), alignof(node_type));
        }

        internal_node->next = internal_node;
        internal_node->prev = internal_node;
    }
} // namespace anton
