#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, size;
    long long total_size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const char *env = getenv("ARRAY_SIZE");
    if (!env) {
        if (rank == 0) fprintf(stderr, "Не установлена переменная окружения ARRAY_SIZE\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    total_size = atoll(env);
    if (total_size <= 0) {
        if (rank == 0) fprintf(stderr, "Неверное значение ARRAY_SIZE=%s\n", env);
        MPI_Abort(MPI_COMM_WORLD, 2);
    }

    long long base = total_size / size;
    long long rem  = total_size % size;
    long long local_size = (rank < rem) ? base + 1 : base;
    long long offset = (rank < rem) ? rank * (base + 1) : rem * (base + 1) + (rank - rem) * base;

    double *local_A = malloc(local_size * sizeof(double));
    double *local_B = malloc(local_size * sizeof(double));
    double *local_sum = malloc(local_size * sizeof(double));
    double *local_diff = malloc(local_size * sizeof(double));
    double *local_prod = malloc(local_size * sizeof(double));
    double *local_quot = malloc(local_size * sizeof(double));

    for (long long i = 0; i < local_size; i++) {
        local_A[i] = offset + i + 1;
        local_B[i] = ((offset + i) % 10) + 1;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double t_start = MPI_Wtime();

    for (long long i = 0; i < local_size; i++) {
        local_sum[i]  = local_A[i] + local_B[i];
        local_diff[i] = local_A[i] - local_B[i];
        local_prod[i] = local_A[i] * local_B[i];
        local_quot[i] = local_A[i] / local_B[i];
    }

    double t_end = MPI_Wtime();

    if (rank == 0) {
        printf("MPI-программа завершена\n");
        printf("Время выполнения MPI-программы (рангов = %d): %.6f секунд\n", size, t_end - t_start);
    }

    free(local_A); free(local_B); free(local_sum); free(local_diff); free(local_prod); free(local_quot);
    MPI_Finalize();
    return 0;
}
