#ifndef LIST_C_H
#define LIST_C_H

#include "darknet.h"

typedef float (*nodeAttr)(node*);

list_c *make_list();

void *list_pop(list_c *l);
void list_insert(list_c *l, void *val);

// 2019.05.15
node* list_index(list_c* l, int index);
void list_delete(list_c* l, int index);
void list_bsort(list_c *l, nodeAttr attr);
void free_current_node(node *n);

// 2019.05.16
int list_check(list_c* l);
void list_attr(list_c* l, nodeAttr attr);

void free_node(node *n);
void free_list(list_c *l);
void free_list_contents(list_c *l);

void **list_to_array(list_c *l);


#endif
