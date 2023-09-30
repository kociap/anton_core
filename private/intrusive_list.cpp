#include <anton/intrusive_list.hpp>

namespace anton {
    void insert_node(Intrusive_List_Node* const position, Intrusive_List_Node* const node) {
        Intrusive_List_Node* const prev = position->prev;
        node->next = position;
        position->prev = node;
        node->prev = prev;
        prev->next = node;
    }

    void erase_node(Intrusive_List_Node* const node) {
        Intrusive_List_Node* const next = node->next;
        Intrusive_List_Node* const prev = node->prev;
        prev->next = next;
        next->prev = prev;
    }
} // namespace anton
