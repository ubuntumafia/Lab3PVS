#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s ARRAY_SIZE\n", argv[0]);
        return 1;
    }

    long long size = atoll(argv[1]);
    if (size <= 0) {
        fprintf(stderr, "Неверный размер массива\n");
        return 2;
    }

    double *A = malloc(size * sizeof(double));
    double *B = malloc(size * sizeof(double));
    double *sum = malloc(size * sizeof(double));
    double *diff = malloc(size * sizeof(double));
    double *prod = malloc(size * sizeof(double));
    double *quot = malloc(size * sizeof(double));

    if (!A || !B || !sum || !diff || !prod || !quot) {
        fprintf(stderr, "Ошибка выделения памяти\n");
        return 3;
    }

    // Инициализация
    for (long long i = 0; i < size; i++) {
        A[i] = i + 1;
        B[i] = (i % 10) + 1;  // Исключаем деление на 0
    }

    struct timeval t0, t1;
    gettimeofday(&t0, NULL);

    for (long long i = 0; i < size; i++) {
        sum[i]  = A[i] + B[i];
        diff[i] = A[i] - B[i];
        prod[i] = A[i] * B[i];
        quot[i] = A[i] / B[i];
    }

    gettimeofday(&t1, NULL);
    double elapsed = (t1.tv_sec - t0.tv_sec) + (t1.tv_usec - t0.tv_usec) / 1e6;

    printf("Последовательная программа завершена\n");
    printf("Время выполнения: %.6f секунд\n", elapsed);

    free(A); free(B); free(sum); free(diff); free(prod); free(quot);
    return 0;
}
