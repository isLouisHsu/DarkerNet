#ifndef LIST_H
#define LIST_H

#include "darknet.h"

typedef float (*nodeAttr)(node*);

list *make_list();

void *list_pop(list *l);
void list_insert(list *l, void *val);

// 2019.05.15
node* list_index(list* l, int index);
void list_delete(list* l, int index);
void list_bsort(list *l, nodeAttr attr);
void free_current_node(node *n);

// 2019.05.16
int list_check(list* l);
void list_attr(list* l, nodeAttr attr);

void free_node(node *n);
void free_list(list *l);
void free_list_contents(list *l);

void **list_to_array(list *l);


#endif
