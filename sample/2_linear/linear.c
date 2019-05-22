#include "darknet.h"
#include "utils.h"
#include "math.h"

typedef struct {
    int n;
    float Xmin;
    float Xmax;
    data* d;
} data_args;

void *load_thread(void *ptr)
{
    data_args a = *(data_args*)ptr;
    
    data d = {0};
    d.shallow = 0;
    d.X.rows = a.n;
    d.X.cols = 1;
    d.X.vals = calloc(d.X.rows, sizeof(float*));
    d.y.rows = a.n;
    d.y.cols = 1;
    d.y.vals = calloc(d.y.rows, sizeof(float*));
    
    for (int i = 0; i < a.n; i++ ){
        d.X.vals[i] = calloc(1, sizeof(float));
        d.y.vals[i] = calloc(1, sizeof(float));
        float x = rand_uniform(a.Xmin, a.Xmax);
        float y = sin(x) + 0.02*rand_normal();
        *(d.X.vals[i]) = x;
        *(d.y.vals[i]) = y;
    }

    *a.d = d;
    free(ptr);
    return 0;
}

pthread_t load_data_thread(data_args args)
{
    pthread_t thread;
    data_args *ptr = calloc(1, sizeof(data_args));
    *ptr = args;
    if(pthread_create(&thread, 0, load_thread, ptr)) error("Thread creation failed");
    return thread;
}

void train_linear(char* cfg, char* weights)
{
    srand(time(0));

    // build network
    float avg_loss = -1.;
    network* net = load_network(cfg, 0, 0);
    printf("Learning Rate: %g, Momentum: %g, Decay: %g\n", net->learning_rate, net->momentum, net->decay);
    
    // load data
    data buffer;
    data_args args = {0};
    args.n = net->batch;
    args.Xmin = 0;
    args.Xmax = TWO_PI/2;
    args.d = &buffer;
    pthread_t load_data = load_data_thread(args);

    // start training
    int i = 0;
    while(get_current_batch(net) < net->max_batches){
        i += 1;

        // load batch data
        pthread_join(load_data, 0);
        data train = buffer;
        load_data = load_data_thread(args);

        // train batch
        float loss = train_network(net, train);
        if (avg_loss < 0) avg_loss = loss;
        avg_loss = avg_loss*0.9 + loss*0.1;
        free_data(train);
        
        // save weights
        if (i%1000 == 0){
            char buff[256];
            sprintf(buff, weights);
            save_weights(net, buff);

            printf("\033[2J");
            printf("\033[1;1H");
            printf("%d: %f, %f avg, %f rate\n", 
                    i, loss, avg_loss, get_current_rate(net));
        }
    }
    char buff[256];
    sprintf(buff, weights, i);
    save_weights(net, buff);
}

int main(int argc, char** argv)
{
    char* cfg = "linear.cfg";
    char* weights = "linear.weights";

    train_linear(cfg, weights);
    return 0;
}