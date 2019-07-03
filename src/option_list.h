#ifndef OPTION_LIST_H
#define OPTION_LIST_H
#include "list_c.h"

typedef struct{
    char *key;
    char *val;
    int used;
} kvp;


int read_option(char *s, list_c *options);
void option_insert(list_c *l, char *key, char *val);
char *option_find(list_c *l, char *key);
float option_find_float(list_c *l, char *key, float def);
float option_find_float_quiet(list_c *l, char *key, float def);
void option_unused(list_c *l);

#endif
