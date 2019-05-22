/* Added on 2019.5.7 */
#ifndef __PRELU_H
#define __PRELU_H

#include "layer.h"
#include "network.h"

layer make_prelu_layer(int batch, int h, int w, int c, int n);
void  forward_prelu_layer(const layer l, network net);
void  backward_prelu_layer(layer l, network net);
void  update_prelu_layer(layer l, update_args a);
void  resize_prelu_layer(layer *l, int w, int h);

#ifdef GPU
void pull_prelu_layer(layer l);
void push_prelu_layer(layer l);

void  forward_prelu_layer_gpu(const layer l, network net);
void  backward_prelu_layer_gpu(layer l, network net);
void  update_prelu_layer_gpu(layer l, update_args a);
#endif


#endif