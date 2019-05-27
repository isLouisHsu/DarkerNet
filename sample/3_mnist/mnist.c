#include "darknet.h"
#include "data.h"

#define H 28
#define W 28
#define PREFIX "./data/images"

void* load_mnist(void* ptr)
{
    printf("Loading batch... ");
    load_args args = *(load_args*)ptr;

    char** random_path = get_random_paths(args.paths, args.n, args.m);

    data d = {0};
    d.shallow = 0;
    d.w = args.w; d.h = args.h;
    d.X.rows = args.n; d.X.cols = args.h*args.w;
    d.X.vals = calloc(d.X.rows, sizeof(float*));
    d.y = make_matrix(args.n, 1);

    for (int i = 0; i < args.n; i++ ){

        char buffer[32] = {0};
        sprintf(buffer, "%s/%s", PREFIX, random_path[i]);
        printf("%s\n", buffer);

        image im = load_image(buffer, 0, 0, 1);
        // image im = make_image(W, H, 1);
        d.X.vals[i] = im.data;                  // 图像数据
        
        char* ret = strchr(buffer, '_');          // _%4d.jpg
        *ret = '\0';
        d.y.vals[i][0] = (float)atof(buffer);
    }

    *args.d = d;
    free(ptr);
    free(random_path);
    printf("OK!\n");
    return 0;
}

pthread_t load_mnist_thread(load_args args)
{
    pthread_t thread;
    load_args *ptr = calloc(1, sizeof(load_args)); *ptr = args;
    if(pthread_create(&thread, 0, load_mnist, ptr)) error("Thread creation failed");
    return thread;
}

int main(int argc, char** argv)
{
   // ======================== build network ======================== //
    network* net = load_network("mnist.cfg", 0, 0);
    printf("Learning Rate: %g, Momentum: %g, Decay: %g\n", net->learning_rate, net->momentum, net->decay);
    int imgs = net->batch*net->subdivisions;
    int i = *net->seen/imgs;
    data train, buffer;

    // ========================== load data ========================== //
    list* plist = get_paths("./data/list.txt");
    char** paths = (char**)list_to_array(plist);

    load_args args = {0};
    args.w = net->w;
    args.h = net->h;
    args.paths = paths; 
    args.n = imgs;
    args.m = plist->size;
    args.d = &buffer;

    pthread_t load_thread = load_mnist_thread(args);
    clock_t time;

    float avg_loss = -1;
    while (get_current_batch(net) < net->max_batches){
        i += 1;

        time = clock();
        pthread_join(load_thread, 0);
        train = buffer;
        load_thread = load_mnist_thread(args);
        printf("Loaded: %lf seconds\n", sec(clock()-time));

        time = clock();
        float loss = train_network(net, train);
        if (avg_loss < 0) avg_loss = loss;
        avg_loss = avg_loss*.9 + loss*.1;
        
        printf("%d: %f, %f avg, %f rate, %lf seconds, %d images\n", i, loss, avg_loss, get_current_rate(net), sec(clock()-time), i*imgs);
        if(i%1000==0 || (i < 1000 && i%100 == 0)){
            char buff[256];
            sprintf(buff, "mnist.weights");
            save_weights(net, buff);
        }
        free_data(train);
    }
    char buff[256];
    save_weights(net, "mnist_final.weights");

    free(paths);
    free_list(plist);
    return 0;
}

// int main(int argc, char** argv)
// {
//     // ========================== load data ========================== //
//     list* plist = get_paths("./data/list.txt");
//     char** paths = (char**)list_to_array(plist);
//     data buffer;

//     load_args args = {0};
//     args.w = 28;
//     args.h = 28;
//     args.paths = paths; 
//     args.n = 32*4;
//     args.m = plist->size;
//     args.d = &buffer;

//     for (int i = 0; i < 5; i++ ){
//         load_mnist(&args);
//         free_data(buffer);
//     }

//     free(paths);
//     free_list(plist);
//     return 0;
// }