#ifndef GRAPH_MATRIX_H
#define GRAPH_MATRIX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GRAPH_MATRIX_SIZE 100

typedef struct {
    int vertexCount;
    int matrix[GRAPH_MATRIX_SIZE][GRAPH_MATRIX_SIZE];
}GraphMatrix;

void matrix_init(GraphMatrix* matrix,int vertexCount);

void matrix_print(GraphMatrix* matrix);

#endif