#include "cuda_runtime.h"
#include "curand.h"
#include "cublas_v2.h"

extern "C" {
#include "prelu_layer.h"
#include "blas.h"
#include "cuda.h"
}

# if 1
__global__ void forward_prelu_layer_kernel(int n, int w, int h, int c, int g, float *input, float *weights, float *output)
{
    int id = (blockIdx.x + blockIdx.y*gridDim.x) * blockDim.x + threadIdx.x;
    if (id >= n) return;    // 所有通道索引

    int k = id % c;         // 某张量通道索引
    float alpha = weights[k / g];

    for(int i = 0; i < h*w; ++i){
        int idx = i + h*w * id;
        float val = input[idx];
        output[idx] = val < 0? alpha*val: val;
    }
}

extern "C" void  forward_prelu_layer_gpu(const layer l, network net)
{
    size_t n = l.c*l.batch;

    forward_prelu_layer_kernel<<<cuda_gridsize(n), BLOCK>>>(n, l.w, l.h, l.c, l.groups, net.input_gpu, l.weights_gpu, l.output_gpu);
    check_error(cudaPeekAtLastError());
}
#else
__global__ void prelu_kernel(float *input, float *output, float *alpha, int c, int n, int size)
{
    int offset = blockIdx.x * blockDim.x + threadIdx.x; // 像素索引
    int filter = blockIdx.y;    // 通道索引
    int batch = blockIdx.z;     // 数据索引
    int idx_n = filter / n;
    int index = (batch*c+filter)*size + offset;

    if(offset < size)
        output[index] = input[index] < 0 ? input[index]*alpha[idx_n] : input[index];
}

extern "C" void prelu_gpu(float *input, float *output, float *alpha, int batch, int c, int n, int size)
{
    dim3 dimGrid((size-1)/BLOCK + 1, c, batch);
    dim3 dimBlock(BLOCK, 1, 1);

    prelu_kernel<<<dimGrid, dimBlock>>>(input, output, alpha, c, n, size);
    check_error(cudaPeekAtLastError());
}

extern "C" void forward_prelu_layer_gpu(layer l, network net){
    fill_gpu(l.outputs*l.batch, 0, l.output_gpu, 1);
    prelu_gpu(net.input_gpu, l.output_gpu, l.weights_gpu, l.batch, l.c, l.groups, l.w*l.h);
}
#endif

void  backward_prelu_layer_gpu(layer l, network net)
{
    // TODO
}

void  update_prelu_layer_gpu(layer l, update_args a)
{
    // TODO
}

void pull_prelu_layer(layer l)
{
    cuda_pull_array(l.weights_gpu, l.weights, l.nweights);
    cuda_pull_array(l.weight_updates_gpu, l.weight_updates, l.nweights);
}

void push_prelu_layer(layer l)
{
    cuda_push_array(l.weights_gpu, l.weights, l.nweights);
    cuda_push_array(l.weight_updates_gpu, l.weight_updates, l.nweights);
}