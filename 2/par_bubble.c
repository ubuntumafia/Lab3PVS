#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

void swap(int* a, int* b) {
    int t = *a;
    *a = *b;
    *b = t;
}

int partition(int arr[], int low, int high) {
    int pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (arr[j] <= pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return i + 1;
}

void quicksort(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quicksort(arr, low, pi - 1);
        quicksort(arr, pi + 1, high);
    }
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const char* env_size = getenv("ARRAY_SIZE");
    int N = env_size ? atoi(env_size) : 200000;

    // Проверка делимости размера массива на число процессов
    if (N % size != 0) {
        if (rank == 0) {
            fprintf(stderr, "ОШИБКА: Размер массива (%d) не делится на число процессов (%d)\n", N, size);
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int* arr = NULL;
    if (rank == 0) {
        arr = malloc(N * sizeof(int));
        if (!arr) {
            fprintf(stderr, "Ошибка выделения памяти\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        srand(time(NULL));
        for (int i = 0; i < N; i++) {
            arr[i] = rand();
        }
    }

    // Синхронизация перед замером времени
    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    int local_size = N / size;
    int* local_arr = malloc(local_size * sizeof(int));
    if (!local_arr) {
        fprintf(stderr, "Ошибка выделения памяти (процесс %d)\n", rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Распределение данных
    MPI_Scatter(arr, local_size, MPI_INT, local_arr, local_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Локальная сортировка
    quicksort(local_arr, 0, local_size - 1);

    // Сбор данных
    MPI_Gather(local_arr, local_size, MPI_INT, arr, local_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Синхронизация перед завершением замера
    MPI_Barrier(MPI_COMM_WORLD);
    double end = MPI_Wtime();

    if (rank == 0) {
        printf("ВРЕМЯ_ВЫПОЛНЕНИЯ: %.6f\n", end - start);
        printf("ПРОЦЕССОВ: %d\n", size);
    }

    free(local_arr);
    if (rank == 0) free(arr);
    MPI_Finalize();
    return 0;
}
