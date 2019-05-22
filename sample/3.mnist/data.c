#include "data.h"

// [offset] [type]          [value]          [description]
// 0000     32 bit integer  0x00000803(2051) magic number
// 0004     32 bit integer  60000            number of images
// 0008     32 bit integer  28               number of rows
// 0012     32 bit integer  28               number of columns
// 0016     unsigned byte   ??               pixel
// 0017     unsigned byte   ??               pixel
// ........
// xxxx     unsigned byte   ??               pixel
// Pixels are organized row-wise. Pixel values are 0 to 255. 0 means background (white), 255 means foreground (black). 
Idx3_ubyte load_mnist_images(char* idx3_ubyte_file)
{
    Idx3_ubyte d = {0};
    FILE* fp = fopen(idx3_ubyte_file, "rb+");

    // header
    fread(&d.magic_number, sizeof(int), 1, fp);
    if (d.magic_number != 2051){
        printf("Error: Wrong magic number!\n");
        fclose(fp);
        return d;
    }
    fread(&d.n, sizeof(int), 1, fp);
    fread(&d.rows, sizeof(int), 1, fp);
    fread(&d.cols, sizeof(int), 1, fp);

    // data
    int size = d.rows*d.cols;
    d.X = make_matrix(d.n, size);   // n x (28 x 28)
    unsigned char* x = calloc(size, sizeof(unsigned char)); // (28 x 28) x 1 (B)
    for (int i = 0; i < d.n; i++ ){
        fread(x, sizeof(unsigned char), size, fp);
        for (int j = 0; j < size; j++ ){
            *(d.X.vals[i] + j) = (float)x[j];
        }
    }

    free(x);
    fclose(fp);
    return d;
}

// [offset] [type]          [value]          [description]
// 0000     32 bit integer  0x00000801(2049) magic number (MSB first)
// 0004     32 bit integer  60000            number of items
// 0008     unsigned byte   ??               label
// 0009     unsigned byte   ??               label
// ........
// xxxx     unsigned byte   ??               label
// The labels values are 0 to 9.
Idx1_ubyte load_mnist_labels(char* idx1_ubyte_file)
{
    Idx1_ubyte d = {0};
    FILE* fp = fopen(idx1_ubyte_file, "rb+");

    // header
    fread(&d.magic_number, sizeof(int), 1, fp);
    if (d.magic_number != 2049){
        printf("Error: Wrong magic number!\n");
        fclose(fp);
        return d;
    }
    fread(&d.n, sizeof(int), 1, fp);

    d.X = make_matrix(d.n, 1);
    unsigned char* x = calloc(1, sizeof(unsigned char));
    for (int i = 0; i < d.n; i++ ){
        fread(x, sizeof(unsigned char), 1, fp);
        *(d.X.vals[i]) = (float)(*x);
    }

    free(x);
    fclose(fp);
    return d;
}
