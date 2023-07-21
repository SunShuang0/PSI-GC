#include "stdlib.h"
#include "tree.h"


node* create_tree_node(num *seed, int control_bit, node* prev)
{

    node* tree_node = malloc(sizeof(node));
    if(prev != NULL)
    {
        prev->next = tree_node;
    }
    tree_node->prev = prev;
    tree_node->next = NULL;
    tree_node->control_bit = control_bit;
    tree_node->seed = seed;
    return tree_node;
}
