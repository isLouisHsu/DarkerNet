#ifndef __DATA_H
#define __DATA_H

#include "darknet.h"

typedef struct {
    int magic_number;
    int n;
    int rows;
    int cols;
    matrix X;
}Idx3_ubyte;

typedef struct {
    int magic_number;
    int n;
    matrix X;
}Idx1_ubyte;

Idx3_ubyte load_mnist_images(char* idx3_ubyte_file);
Idx1_ubyte load_mnist_labels(char* idx1_ubyte_file);

#endif