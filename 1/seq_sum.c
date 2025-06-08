#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int main(int argc, char *argv[]) {
    long long total_size;
    long long sum = 0;

    if (argc != 2) {
        fprintf(stderr, "Использование: %s ARRAY_SIZE\n", argv[0]);
        return 1;
    }

    total_size = atoll(argv[1]);
    if (total_size <= 0) {
        fprintf(stderr, "Неверный размер: %s\n", argv[1]);
        return 2;
    }

    struct timeval t0, t1;
    gettimeofday(&t0, NULL);

    for (long long i = 1; i <= total_size; i++) {
        sum += i;
    }

    gettimeofday(&t1, NULL);
    double elapsed = (t1.tv_sec - t0.tv_sec)
                   + (t1.tv_usec - t0.tv_usec) / 1e6;

    printf("%lld %.9f\n", sum, elapsed);
    return 0;
}
