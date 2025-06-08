#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>

void initialize_matrix(double *matrix, int rows, int cols) {
    for (int i = 0; i < rows * cols; i++) {
        matrix[i] = (double)rand() / RAND_MAX * 100.0;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <rows> <cols>\n", argv[0]);
        return 1;
    }

    int rows = atoi(argv[1]);
    int cols = atoi(argv[2]);
    int total_elements = rows * cols;

    if (rows <= 0 || cols <= 0) {
        fprintf(stderr, "Invalid matrix dimensions\n");
        return 2;
    }

    double *A = malloc(total_elements * sizeof(double));
    double *B = malloc(total_elements * sizeof(double));
    double *C = malloc(total_elements * sizeof(double));

    if (!A || !B || !C) {
        fprintf(stderr, "Memory allocation failed\n");
        return 3;
    }

    srand(42); // Fixed seed for reproducibility
    initialize_matrix(A, rows, cols);
    initialize_matrix(B, rows, cols);

    struct timeval t0, t1;
    gettimeofday(&t0, NULL);

    // Perform all operations
    for (int i = 0; i < total_elements; i++) {
        C[i] = A[i] + B[i];      // Addition
        C[i] = A[i] - B[i];      // Subtraction
        C[i] = A[i] * B[i];      // Multiplication
        if (fabs(B[i]) > 1e-12) { // Division with check
            C[i] = A[i] / B[i];
        } else {
            C[i] = 0.0;
        }
    }

    gettimeofday(&t1, NULL);
    double elapsed = (t1.tv_sec - t0.tv_sec) + (t1.tv_usec - t0.tv_usec) / 1e6;

    printf("Matrix operations completed. Time: %.6f seconds\n", elapsed);

    free(A);
    free(B);
    free(C);
    return 0;
}