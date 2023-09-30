#pragma once

#include <anton/iterators.hpp>
#include <anton/swap.hpp>
#include <anton/types.hpp>

namespace anton {
    // Intrusive_List_Node
    // A POD base type of Intrusive_List's nodes.
    //
    struct Intrusive_List_Node {
        Intrusive_List_Node* prev;
        Intrusive_List_Node* next;
    };

    // insert_node
    // Insert a node into an intrusive list before position.
    // This is an external management function and does not require the list object.
    //
    // Parameters:
    // position - node before which to insert the new node.
    //     node - new node to be inserted.
    //
    void insert_node(Intrusive_List_Node* position, Intrusive_List_Node* node);

    // erase_node
    // Unlink node from its list. The links of the node are assumed to be valid. If the node is not
    // in a list or its links are invalid, the behaviour is undefined.
    // This is an external management function and does not require the list object.
    //
    // Parameters:
    // node - node to be removed.
    //
    void erase_node(Intrusive_List_Node* node);

    // Intrusive_List_Iterator
    // Default constructed iterator is invalid.
    // Since this is an intrusive iterator list, the value type is also the node type.
    //
    // Parameters:
    // Node_Type - node type of the list.
    //
    template<typename Node_Type>
    struct Intrusive_List_Iterator {
    public:
        using value_type = Node_Type;
        using node_type = Node_Type;
        using pointer = value_type*;
        using reference = value_type&;
        using difference_type = i64;
        using iterator_category = Bidirectional_Iterator_Tag;

        pointer node = nullptr;

        Intrusive_List_Iterator() = default;
        Intrusive_List_Iterator(pointer node): node(node) {}
        Intrusive_List_Iterator(Intrusive_List_Iterator const&) = default;
        Intrusive_List_Iterator(Intrusive_List_Iterator&&) = default;
        ~Intrusive_List_Iterator() = default;
        Intrusive_List_Iterator& operator=(Intrusive_List_Iterator const&) = default;
        Intrusive_List_Iterator& operator=(Intrusive_List_Iterator&&) = default;

        // Conversion operator to the const version of the iterator.
        [[nodiscard]] operator Intrusive_List_Iterator<Node_Type const>() const {
            return node;
        }

        [[nodiscard]] pointer operator->() const {
            return &node->data;
        }

        [[nodiscard]] reference operator*() const {
            return node->data;
        }

        Intrusive_List_Iterator& operator++() {
            node = node->next;
            return *this;
        }

        [[nodiscard]] Intrusive_List_Iterator operator++(int) {
            Intrusive_List_Iterator v{node};
            node = node->next;
            return v;
        }

        Intrusive_List_Iterator& operator--() {
            node = node->prev;
            return *this;
        }

        [[nodiscard]] Intrusive_List_Iterator operator--(int) {
            Intrusive_List_Iterator v{node};
            node = node->prev;
            return v;
        }

        [[nodiscard]] bool operator==(Intrusive_List_Iterator const& other) const {
            return node == other.node;
        }

        [[nodiscard]] bool operator!=(Intrusive_List_Iterator const& other) const {
            return node != other.node;
        }
    };

    // Intrusive_List
    // Similarly to List, the container provides address-stable node-based storage with
    // bidirectional iteration. However, there are differences:
    // - The node information is stored directly in the data structures.
    // - Allocation and deallocation of the nodes is managed by the user - the container does no
    //   memory allocations and does not deallocate anything as it works with user-supplied data.
    // - Does not support copy construction or assignment.
    //
    template<typename Node>
    struct Intrusive_List {
    public:
        using size_type = i64;
        using value_type = Node;
        using node_type = Node;
        using reference = Node&;
        using pointer = Node*;
        using iterator = Intrusive_List_Iterator<Node>;
        using const_iterator = Intrusive_List_Iterator<Node const>;

    private:
        alignas(alignof(Node)) Intrusive_List_Node _internal_node;

    public:
        Intrusive_List();
        Intrusive_List(Intrusive_List const& other) = delete;
        Intrusive_List(Intrusive_List&& other);
        Intrusive_List& operator=(Intrusive_List const& other) = delete;
        Intrusive_List& operator=(Intrusive_List&& other);
        ~Intrusive_List() = default;

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

        // insert
        // Insert a node before position.
        //
        // Parameters:
        // position - iterator to the node before which to insert the new nodes.
        //     node - the new node to be inserted.
        //
        // Returns:
        // Iterator to the inserted node.
        //
        iterator insert(const_iterator position, node_type const& node);

        // insert_front
        // Insert a node at the front of the list.
        //
        // Parameters:
        // node - the new node to be inserted.
        //
        // Returns:
        // Iterator to the inserted node.
        //
        iterator insert_front(node_type const& node);

        // insert_back
        // Insert a node at the back of the list.
        //
        // Parameters:
        // node - the new node to be inserted.
        //
        // Returns:
        // Iterator to the inserted node.
        //
        iterator insert_back(node_type const& node);

        // erase
        // Erase the node at position.
        //
        // Parameters:
        // position - iterator to the node to erase.
        //
        void erase(const_iterator position);

        // erase
        // Erase a range of nodes defined by [first, last[.
        //
        // Parameters:
        // first - the iterator to the beginning of the range.
        //  last - the iterator past the end of the range.
        //
        void erase(const_iterator first, const_iterator last);

        // erase_front
        // Erase the first node from the list.
        // If the list is empty, the behaviour is undefined.
        //
        void erase_front();

        // erase_back
        // Erase the last node from the list.
        // If the list is empty, the behaviour is undefined.
        //
        void erase_back();

        // clear
        // Unlink all nodes present in the list.
        //
        void clear();

        // swap
        // Swaps the contents of 2 lists.
        //
        // Compexity:
        // O(1)
        //
        friend void swap(Intrusive_List& list1, Intrusive_List& list2) {
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

    template<typename Node>
    Intrusive_List<Node>::Intrusive_List() {
        node_type* const internal_node = reinterpret_cast<Intrusive_List_Node*>(&_internal_node);
        _internal_node.next = internal_node;
        _internal_node.prev = internal_node;
    }

    template<typename Node>
    Intrusive_List<Node>::Intrusive_List(Intrusive_List&& other) {
        node_type* const internal_node = reinterpret_cast<Intrusive_List_Node*>(&_internal_node);
        _internal_node.next = internal_node;
        _internal_node.prev = internal_node;
        swap(*this, other);
    }

    template<typename Node>
    Intrusive_List<Node>& Intrusive_List<Node>::operator=(Intrusive_List&& other) {
        node_type* const internal_node = reinterpret_cast<Intrusive_List_Node*>(&_internal_node);
        _internal_node.next = internal_node;
        _internal_node.prev = internal_node;
        swap(*this, other);
    }

    template<typename Node>
    auto Intrusive_List<Node>::begin() -> iterator {
        return reinterpret_cast<node_type*>(_internal_node.next);
    }

    template<typename Node>
    auto Intrusive_List<Node>::end() -> iterator {
        return reinterpret_cast<node_type*>(_internal_node);
    }

    template<typename Node>
    auto Intrusive_List<Node>::begin() const -> const_iterator {
        return reinterpret_cast<node_type*>(_internal_node.next);
    }

    template<typename Node>
    auto Intrusive_List<Node>::end() const -> const_iterator {
        return reinterpret_cast<node_type*>(_internal_node);
    }

    template<typename Node>
    auto Intrusive_List<Node>::cbegin() const -> const_iterator {
        return reinterpret_cast<node_type*>(_internal_node.next);
    }

    template<typename Node>
    auto Intrusive_List<Node>::cend() const -> const_iterator {
        return reinterpret_cast<node_type*>(_internal_node);
    }

    template<typename Node>
    auto Intrusive_List<Node>::size() const -> size_type {
        size_type _size = 0;
        node_type* const internal_node = reinterpret_cast<node_type*>(&_internal_node);
        node_type* node = internal_node->next;
        while(node != internal_node) {
            node = node->next;
            ++_size;
        }
        return _size;
    }

    template<typename Node>
    auto Intrusive_List<Node>::insert(const_iterator position, node_type const& node) -> iterator {
        node_type* const next = position.node;
        node_type* const prev = next->prev;
        node.prev = prev;
        prev->next = &node;
        node.next = next;
        next->prev = &node;
        return &node;
    }

    template<typename Node>
    auto Intrusive_List<Node>::insert_front(node_type const& node) -> iterator {
        node_type* const next = reinterpret_cast<node_type*>(&_internal_node);
        node_type* const prev = next->prev;
        node.prev = prev;
        prev->next = &node;
        node.next = next;
        next->prev = &node;
        return &node;
    }

    template<typename Node>
    auto Intrusive_List<Node>::insert_back(node_type const& node) -> iterator {
        node_type* const next = reinterpret_cast<node_type*>(&_internal_node);
        node_type* const prev = next->prev;
        node.prev = prev;
        prev->next = &node;
        node.next = next;
        next->prev = &node;
        return &node;
    }

    template<typename Node>
    void Intrusive_List<Node>::erase(const_iterator position) {
        node_type* const node = position.node;
        node_type* const next = node->next;
        node_type* const prev = node->prev;
        next->prev = prev;
        prev->next = next;
    }

    template<typename Node>
    void Intrusive_List<Node>::erase(const_iterator first, const_iterator last) {
        node_type* const next = last.node->next;
        node_type* const prev = first.node->prev;
        next->prev = prev;
        prev->next = next;
    }

    template<typename Node>
    void Intrusive_List<Node>::erase_front() {
        node_type* const prev = reinterpret_cast<node_type*>(&_internal_node);
        node_type* const node = prev->next;
        node_type* const next = node->next;
        next->prev = prev;
        prev->next = next;
    }

    template<typename Node>
    void Intrusive_List<Node>::erase_back() {
        node_type* const next = reinterpret_cast<node_type*>(&_internal_node);
        node_type* const node = next->prev;
        node_type* const prev = node->prev;
        next->prev = prev;
        prev->next = next;
    }

    template<typename Node>
    void Intrusive_List<Node>::clear() {
        node_type* const internal_node = reinterpret_cast<Intrusive_List_Node*>(&_internal_node);
        _internal_node.next = internal_node;
        _internal_node.prev = internal_node;
    }
} // namespace anton
