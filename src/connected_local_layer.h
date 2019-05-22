#ifndef __CONNECTED_LOCAL_LAYER_H
#define __CONNECTED_LOCAL_LAYER_H

#include "activations.h"
#include "layer.h"
#include "network.h"

layer make_connected_local_layer(int batch, int inputs, int outputs, int groups, ACTIVATION activation);

void forward_connected_local_layer(layer l, network net);
void backward_connected_local_layer(layer l, network net);
void update_connected_local_layer(layer l, update_args a);

void resize_connected_local_layer(layer *l);

#ifdef GPU
void forward_connected_local_layer_gpu(layer l, network net);
void backward_connected_local_layer_gpu(layer l, network net);
void update_connected_local_layer_gpu(layer l, update_args a);
void push_connected_local_layer(layer l);
void pull_connected_local_layer(layer l);
#endif

#endif