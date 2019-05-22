#include "darknet.h"
#include "prelu_layer.h"

void print_feature_map(float *x, int w, int h, int c)
{
    for (int k = 0; k < c; k++ ){
        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                printf("%f ", x[k*h*w + i*w + j]);
            }
            printf("\n");
        }
        printf("\n\n");
    }
}

int main(int argc, char **argv)
{
    image X = make_image(3, 3, 10);
    fill_cpu(3*3*10, 10, X.data, 1);
    for (int c = 0; c < 10; c++ ) {
        for (int i = 2; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                X.data[c*3*3 + i*3 + j] *= -1;
            }
        }
    }
    print_feature_map(X.data, 3, 3, 10);

    network* net = load_network("prelu.cfg", "prelu.weights", 0);
    for (int i = 0; i<5; i++ )
        printf("%f ", net->layers[0].weights[i]);
    printf("\n\n");
    
    network_predict(net, X.data);
    print_feature_map(net->layers[0].output, 3, 3, 10);

    return 0;
}