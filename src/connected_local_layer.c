/* Added on 2019.5.22 */
/* forward only */
#include "blas.h"
#include "assert.h"
#include "cuda.h"
#include "gemm.h"

#include "connected_local_layer.h"
#include "convolutional_layer.h"

layer make_connected_local_layer(int batch, int inputs, int outputs, int groups, ACTIVATION activation)
{
    assert(inputs % groups == 0 && outputs % groups == 0);

    int i;
    layer l = {0};
    l.learning_rate_scale = 1;
    l.type = CONNECTED_LOCAL;

    l.inputs = inputs;
    l.outputs = outputs;
    l.batch = batch;

    l.h = 1;
    l.w = 1;
    l.c = inputs;
    l.out_h = 1;
    l.out_w = 1;
    l.out_c = outputs;
    l.groups = groups;

    l.nweights = inputs*outputs/groups;
    l.nbiases = outputs;

    l.output = calloc(batch*outputs, sizeof(float));        // (batch, outputs)
    l.delta = calloc(batch*outputs, sizeof(float));

    l.weights = calloc(l.nweights, sizeof(float));          // (groups, outputs/groups, inputs/groups)
    l.weight_updates = calloc(l.nweights, sizeof(float));
    l.biases = calloc(l.nbiases, sizeof(float));            // (outputs,)
    l.bias_updates = calloc(l.nbiases, sizeof(float));

    l.forward = forward_connected_local_layer;
    l.backward = backward_connected_local_layer;
    l.update = update_connected_local_layer;

    float scale = sqrt(2./inputs);
    for(i = 0; i < l.nweights; i++ ){
        l.weights[i] = scale*rand_uniform(-1, 1);
    }

    for (i = 0; i < l.nbiases; i++ ){
        l.biases[i] = 0;
    }

#ifdef GPU
    l.forward_gpu = forward_connected_local_layer_gpu;
    l.backward_gpu = backward_connected_local_layer_gpu;
    l.update_gpu = update_connected_local_layer_gpu;

    l.output_gpu = cuda_make_array(l.output, batch*outputs);
    l.delta_gpu = cuda_make_array(l.delta, batch*outputs);

    l.weights_gpu = cuda_make_array(l.weights, l.nweights);
    l.weight_updates_gpu = cuda_make_array(l.weight_updates, l.nweights);
    l.biases_gpu = cuda_make_array(l.biases, l.nbiases);
    l.bias_updates_gpu = cuda_make_array(l.bias_updates, l.nbiases);
#endif

    l.activation = activation;
    fprintf(stderr, "locally %3d x%3d /%2d %4d x%4d x%4d   ->  %4d x%4d x%4d\n", l.inputs, l.outputs, l.groups, l.h, l.w, l.c, l.out_h, l.out_w, l.out_c);

    return l;
}

void forward_connected_local_layer(layer l, network net)
{
    fill_cpu(l.batch*l.outputs, 0, l.output, 1);

    int m = 1;
    int k = l.inputs / l.groups;
    int n = l.outputs / l.groups;
    int w = k * n;      // w = l.nweights / l.groups
    for (int i = 0; i < l.batch; i++ ){
        for (int g = 0; g < l.groups; g++ ){
            float* a = net.input + i*l.inputs + g*k;    // 1 x k
            float* b = l.weights + g*w;                 // n x k
            float* c = l.output + i*l.outputs + g*n;    // 1 x n
            gemm(0,1,m,n,k,1,a,k,b,k,1,c,n);
        }
    }

    add_bias(l.output, l.biases, l.batch, l.outputs, 1);
    activate_array(l.output, l.batch*l.outputs, l.activation);
}

void backward_connected_local_layer(layer l, network net)
{
    // TODO
}

void update_connected_local_layer(layer l, update_args a)
{
    // TODO
}

void resize_connected_local_layer(layer *l)
{
    l->output = realloc(l->output, l->batch*l->outputs*sizeof(float));
    l->delta  = realloc(l->delta,  l->batch*l->outputs*sizeof(float));
#ifdef GPU
    cuda_free(l->output_gpu);
    cuda_free(l->delta_gpu);
    l->output_gpu = cuda_make_array(l->output, l->batch*l->outputs);
    l->delta_gpu = cuda_make_array(l->delta, l->batch*l->outputs);
#endif
}

#ifdef GPU
void forward_connected_local_layer_gpu(layer l, network net)
{
    fill_gpu(l.batch*l.outputs, 0, l.output_gpu, 1);

    int m = 1;
    int k = l.inputs / l.groups;
    int n = l.outputs / l.groups;
    int w = k * n;      // w = l.nweights / l.groups
    for (int i = 0; i < l.batch; i++ ){
        for (int g = 0; g < l.groups; g++ ){
            float* a = net.input_gpu + i*l.inputs + g*k;    // 1 x k
            float* b = l.weights_gpu + g*w;                 // n x k
            float* c = l.output_gpu + i*l.outputs + g*n;    // 1 x n
            gemm_gpu(0,1,m,n,k,1,a,k,b,k,1,c,n);
        }
    }

    add_bias_gpu(l.output_gpu, l.biases_gpu, l.batch, l.outputs, 1);
    activate_array_gpu(l.output_gpu, l.batch*l.outputs, l.activation);
}

void backward_connected_local_layer_gpu(layer l, network net)
{
    // TODO
}

void update_connected_local_layer_gpu(layer l, update_args a)
{
    // TODO
}

void push_connected_local_layer(layer l)
{
    cuda_pull_array(l.weights_gpu, l.weights, l.nweights);
    cuda_pull_array(l.weight_updates_gpu, l.weight_updates, l.nweights);
    cuda_pull_array(l.biases_gpu, l.biases, l.nbiases);
    cuda_pull_array(l.bias_updates_gpu, l.bias_updates, l.nbiases);
}

void pull_connected_local_layer(layer l)
{
    cuda_push_array(l.weights_gpu, l.weights, l.nweights);
    cuda_push_array(l.weight_updates_gpu, l.weight_updates, l.nweights);
    cuda_push_array(l.biases_gpu, l.biases, l.nbiases);
    cuda_push_array(l.bias_updates_gpu, l.bias_updates, l.nbiases);
}

#endif