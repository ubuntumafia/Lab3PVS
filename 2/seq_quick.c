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

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    const char* env_size = getenv("ARRAY_SIZE");
    int N = env_size ? atoi(env_size) : 200000;

    int* arr = malloc(N * sizeof(int));
    if (!arr) {
        fprintf(stderr, "Ошибка выделения памяти\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    if (rank == 0) {
        srand(time(NULL));
        for (int i = 0; i < N; i++) {
            arr[i] = rand();
        }
    }

    double start = MPI_Wtime();
    if (rank == 0) {
        quicksort(arr, 0, N - 1);
    }
    double end = MPI_Wtime();

    if (rank == 0) {
        printf("ВРЕМЯ_ВЫПОЛНЕНИЯ: %.6f\n", end - start);
        printf("ПРОЦЕССОВ: 1\n");
    }

    free(arr);
    MPI_Finalize();
    return 0;
}
