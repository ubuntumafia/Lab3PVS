#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Использование: $0 <число_рангов> <кол-во_элементов>"
    echo "Пример: $0 8 250000"
    exit 1
fi

RANKS=$1
ARRAY_SIZE=$2
export ARRAY_SIZE

EXEC="./par_mpi_1"

if [ ! -x "$EXEC" ]; then
    echo "Ошибка: $EXEC не найден или не исполняемый"
    exit 2
fi

echo "=== Запуск $EXEC с $RANKS рангов и $ARRAY_SIZE элементов (100 раз) ==="

total_time=0.0

for i in $(seq 1 100); do
    time_result=$(mpirun -np "$RANKS" "$EXEC" 2>/dev/null | \
        awk '/Время выполнения MPI-программы/ { print $(NF-1) }')

    if [ -z "$time_result" ]; then
        echo "Ошибка на запуске $i: время не получено"
        exit 3
    fi

    printf "Run #%3d: %s сек\n" "$i" "$time_result"

    total_time=$(awk -v a="$total_time" -v b="$time_result" 'BEGIN { printf("%.6f", a + b) }')
done

avg_time=$(awk -v t="$total_time" 'BEGIN { printf("%.6f", t / 100.0) }')

echo "-----------------------------------------------"
echo "Среднее время для $RANKS рангов и $ARRAY_SIZE элементов: $avg_time сек"
