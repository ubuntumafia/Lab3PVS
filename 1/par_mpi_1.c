#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, nprocs;
    long long total_size;
    long long local_start, local_end;
    long long base, rem;
    long long local_count;
    long long local_sum = 0, global_sum = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    if (rank == 0) {
        const char *env = getenv("ARRAY_SIZE");
        if (!env) {
            fprintf(stderr, "rank 0: не задана переменная окружения ARRAY_SIZE\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        total_size = atoll(env);
        if (total_size <= 0) {
            fprintf(stderr, "rank 0: неверное значение ARRAY_SIZE=%s\n", env);
            MPI_Abort(MPI_COMM_WORLD, 2);
        }
    }
    MPI_Bcast(&total_size, 1, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);

    base = total_size / nprocs;
    rem  = total_size % nprocs;

    if (rank < rem) {
        local_count = base + 1;
        local_start = rank * (base + 1) + 1;
    } else {
        local_count = base;
        local_start = rem * (base + 1) + (rank - rem) * base + 1;
    }
    local_end = local_start + local_count - 1;

    MPI_Barrier(MPI_COMM_WORLD);
    double t_start = MPI_Wtime();

    for (long long x = local_start; x <= local_end; x++) {
        local_sum += x;
    }

    MPI_Reduce(&local_sum, &global_sum, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    double t_end = MPI_Wtime();

    if (rank == 0) {
        printf("Сумма массива (параллельное MPI): %lld\n", global_sum);
        printf("Время выполнения MPI-программы (рангов = %d): %.6f секунд\n", nprocs, t_end - t_start);
    }

    MPI_Finalize();
    return 0;
}
