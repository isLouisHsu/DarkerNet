#ifndef PARSER_H
#define PARSER_H
#include "darknet.h"
#include "network.h"

LAYER_TYPE string_to_layer_type(char * type);
char* layer_type_to_string(LAYER_TYPE lt);

void save_network(network net, char *filename);
void save_weights_double(network net, char *filename);

#endif
