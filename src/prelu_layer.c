/* Added on 2019.5.7 */
/* forward only */

#include "prelu_layer.h"
#include "blas.h"
#include "assert.h"

layer make_prelu_layer(int batch, int h, int w, int c, int n)
{
    layer l = {0};
    l.type = PRELU;

    assert(c % n == 0);

    l.n = n;
    l.groups = c / n;   // the number of feature maps in a group

    l.batch = batch;
    l.h = h;
    l.w = w;
    l.c = c;
    l.inputs = h*w*c;

    l.out_h = h;    
    l.out_w = w;
    l.out_c = c;
    l.outputs = h*w*c;

    l.output = calloc(batch*l.outputs, sizeof(float));
    l.delta  = calloc(batch*l.outputs, sizeof(float));

    l.weights = calloc(n, sizeof(float));
    l.weight_updates = calloc(n, sizeof(float));

    l.nweights = n;

    l.forward = forward_prelu_layer;
    l.backward = backward_prelu_layer;
    l.update = update_prelu_layer;

#ifdef GPU
    l.forward_gpu = forward_prelu_layer_gpu;
    l.backward_gpu = backward_prelu_layer_gpu;
    l.update_gpu = update_prelu_layer_gpu;

    if (gpu_index >= 0){
        l.output_gpu = cuda_make_array(l.output, l.batch*l.outputs);
        l.delta_gpu  = cuda_make_array(l.delta, l.batch*l.outputs);
        l.weights_gpu = cuda_make_array(l.weights, l.nweights);
        l.weight_updates_gpu = cuda_make_array(l.weight_updates, l.nweights);
    }
#endif

    fprintf(stderr, "prelu   %3d             %4d x%4d x%4d   ->  %4d x%4d x%4d\n", n, l.w, l.h, l.c, l.out_w, l.out_h, l.out_c);

    return l;
}

void  forward_prelu_layer(const layer l, network net)
{
    // >= 0
    clip_cpu(l.batch*l.outputs, 0, net.input, 1, l.output,  1);
    axpy_cpu(l.batch*l.outputs, -1, l.output, 1, net.input, 1);
    
    // < 0
    int size = l.w*l.h*l.groups;
    float alpha = 0;
    for (int i = 0; i < l.batch; i++ ){
        for (int j = 0; j < l.n; j++ ){
            alpha = l.weights[j];
            float *a = net.input + i*l.outputs + j*size;
            float *b = l.output  + i*l.outputs + j*size;
            axpy_cpu(size, alpha, a, 1, b, 1);
        }
    } 
}

void  backward_prelu_layer(layer l, network net)
{
    // TODO
}

void  update_prelu_layer(layer l, update_args a)
{
    // TODO
}

void  resize_prelu_layer(layer *l, int w, int h)
{
    l->w = w;
    l->h = h;
    l->inputs = w*h*l->c;

    l->out_w = w;
    l->out_h = h;
    l->outputs = w*h*l->out_c;

    l->output = realloc(l->output, l->batch*l->outputs*sizeof(float));
    l->delta  = realloc(l->delta, l->batch*l->outputs*sizeof(float));

#ifdef GPU
    cuda_free(l->output_gpu);
    cuda_free(l->delta_gpu);
    l->output_gpu = cuda_make_array(l->output, l->batch*l->outputs);
    l->delta_gpu  = cuda_make_array(l->delta,  l->batch*l->outputs);
#endif

}
