#include "stdint.h"
#include "gen.h"
typedef struct _node
{
    num *seed;
    int control_bit;
    struct _node *next;
    struct _node *prev;
}node;
//GGM树节点，可以理解为GGM双向链表

node* create_tree_node(num *seed, int control_bit, node* prev);
