#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>

void initialize_matrix(double *matrix, int rows, int cols) {
    for (int i = 0; i < rows * cols; i++) {
        matrix[i] = (double)rand() / RAND_MAX * 100.0;
    }
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int rows, cols;
    if (rank == 0) {
        if (argc != 3) {
            fprintf(stderr, "Usage: %s <rows> <cols>\n", argv[0]);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        rows = atoi(argv[1]);
        cols = atoi(argv[2]);
        if (rows <= 0 || cols <= 0) {
            fprintf(stderr, "Invalid matrix dimensions\n");
            MPI_Abort(MPI_COMM_WORLD, 2);
        }
    }

    // Broadcast matrix dimensions to all processes
    MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&cols, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int total_elements = rows * cols;
    int elements_per_proc = total_elements / size;
    int remainder = total_elements % size;

    // Calculate local workload
    int local_start, local_end;
    if (rank < remainder) {
        local_start = rank * (elements_per_proc + 1);
        local_end = local_start + elements_per_proc;
    } else {
        local_start = remainder * (elements_per_proc + 1) + (rank - remainder) * elements_per_proc;
        local_end = local_start + elements_per_proc - 1;
    }
    int local_count = local_end - local_start + 1;

    double *A = NULL, *B = NULL, *C = NULL;
    double *local_A = malloc(local_count * sizeof(double));
    double *local_B = malloc(local_count * sizeof(double));
    double *local_C = malloc(local_count * sizeof(double));

    if (rank == 0) {
        A = malloc(total_elements * sizeof(double));
        B = malloc(total_elements * sizeof(double));
        C = malloc(total_elements * sizeof(double));

        srand(42); // Fixed seed for reproducibility
        initialize_matrix(A, rows, cols);
        initialize_matrix(B, rows, cols);
    }

    // Prepare send counts and displacements for scatter
    int *sendcounts = malloc(size * sizeof(int));
    int *displs = malloc(size * sizeof(int));
    for (int i = 0; i < size; i++) {
        if (i < remainder) {
            sendcounts[i] = elements_per_proc + 1;
        } else {
            sendcounts[i] = elements_per_proc;
        }
        displs[i] = (i == 0) ? 0 : (displs[i-1] + sendcounts[i-1]);
    }

    // Scatter matrices
    MPI_Scatterv(A, sendcounts, displs, MPI_DOUBLE, local_A, local_count, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatterv(B, sendcounts, displs, MPI_DOUBLE, local_B, local_count, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    // Perform local operations
    for (int i = 0; i < local_count; i++) {
        local_C[i] = local_A[i] + local_B[i];      // Addition
        local_C[i] = local_A[i] - local_B[i];      // Subtraction
        local_C[i] = local_A[i] * local_B[i];      // Multiplication
        if (fabs(local_B[i]) > 1e-12) {           // Division with check
            local_C[i] = local_A[i] / local_B[i];
        } else {
            local_C[i] = 0.0;
        }
    }

    MPI_Gatherv(local_C, local_count, MPI_DOUBLE, C, sendcounts, displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    double end = MPI_Wtime();

    if (rank == 0) {
        printf("MATRIX_OPS_TIME: %.6f\n", end - start);
        printf("PROCESSES: %d\n", size);
    }

    free(local_A);
    free(local_B);
    free(local_C);
    free(sendcounts);
    free(displs);
    if (rank == 0) {
        free(A);
        free(B);
        free(C);
    }

    MPI_Finalize();
    return 0;
}