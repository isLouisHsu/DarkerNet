#include <stdlib.h>
#include <string.h>
#include "list.h"

list *make_list()
{
	list *l = malloc(sizeof(list));
	l->size = 0;
	l->front = 0;
	l->back = 0;
	return l;
}

/*
void transfer_node(list *s, list *d, node *n)
{
    node *prev, *next;
    prev = n->prev;
    next = n->next;
    if(prev) prev->next = next;
    if(next) next->prev = prev;
    --s->size;
    if(s->front == n) s->front = next;
    if(s->back == n) s->back = prev;
}
*/

void *list_pop(list *l){
    if(!l->back) return 0;
    node *b = l->back;
    void *val = b->val;
    l->back = b->prev;
    if(l->back) l->back->next = 0;
    free(b);
    --l->size;
    
    return val;
}

void list_insert(list *l, void *val)
{
	node *new = malloc(sizeof(node));
	new->val = val;
	new->next = 0;

	if(!l->back){
		l->front = new;
		new->prev = 0;
	}else{
		l->back->next = new;
		new->prev = l->back;
	}
	l->back = new;
	++l->size;
}

// 2019.05.15
node* list_index(list* l, int index)
{
	node* n = l->front;
	while(index > 0){
		n = n->next;
		--index;
	}
	return n;
}

void list_delete(list* l, int index)
{
	node* n = list_index(l, index);
    if(n->next)n->next->prev = n->prev;
    if(n->prev)n->prev->next = n->next;
	free_current_node(n);
	--l->size;
}

// sorted in ascending order
void list_bsort(list *l, nodeAttr attr)
{
    for (int i = 0; i < l->size - 1; i++ ){
		int j = l->size - 1 - i;
		node* n = l->front;
		while (j > 0){
			if (attr(n) > attr(n->next)){
				void* val = n->val;
				n->val = n->next->val;
				n->next->val = val;
			}
			n = n->next;
			--j;
		}
    }
}

void free_node(node *n)
{
	node *next;
	while(n) {
		next = n->next;
		free(n);
		n = next;
	}
}

void free_current_node(node *n)
{
	free(n->val);
	free(n);
}

// 2019.05.16
int list_check(list* l)
{
	if (l->size == 0) return -1;// good list

	int index = 0;
	node* n = l->front;
	while(n){
		++index;
		node* next = n->next;

		if (next){
			if (next->prev != n) 
				return index;	// bad list
		}
		n = next;
	}
	return -1;					// good list
}

void list_attr(list* l, nodeAttr attr)
{
	node* n = l->front;
	while(n){
		printf("%f ", attr(n));
		n = n->next;
	}
	printf("\n");
}

void free_list(list *l)
{
	free_node(l->front);
	free(l);
}

void free_list_contents(list *l)
{
	node *n = l->front;
	while(n){
		free(n->val);
		n = n->next;
	}
}

void **list_to_array(list *l)
{
    void **a = calloc(l->size, sizeof(void*));
    int count = 0;
    node *n = l->front;
    while(n){
        a[count++] = n->val;
        n = n->next;
    }
    return a;
}
