#!/bin/bash
# run_seq.sh — запускает последовательную программу seq_quick 10 раз и усредняет время
#
# Использование: ./run_seq.sh <кол-во_элементов>
# Пример:   ./run_seq.sh 240000

set -euo pipefail

if [ "$#" -ne 1 ]; then
    echo "Использование: $0 <кол-во_элементов>" >&2
    exit 1
fi

ELEMENTS=$1
EXEC=./seq_quick
RUNS=10

# Проверяем, что исполняемый файл есть и он исполняемый
if [ ! -x "$EXEC" ]; then
    echo "Ошибка: $EXEC не найден или не исполняем" >&2
    exit 2
fi

echo "=== Запуск $EXEC, N=$ELEMENTS (запусков: $RUNS) ==="

# Передаём размер массива через переменную окружения
export ARRAY_SIZE="$ELEMENTS"

total_time=0

for ((i=1; i<=RUNS; i++)); do
    echo -n "Запуск $i/$RUNS… "
    # Запускаем программу (MPI or plain)
    # Если собрали с mpicc, можно через mpirun -np 1, иначе просто ./seq_quick
    output=$("$EXEC" 2>&1)
    # Если MPI-версия, заменить на:
    # output=$(mpirun -np 1 "$EXEC" 2>&1)

    # Извлекаем время из строки "ВРЕМЯ_ВЫПОЛНЕНИЯ: 0.123456"
    time_val=$(echo "$output" | awk '/ВРЕМЯ_ВЫПОЛНЕНИЯ:/ {print $2}')

    if ! [[ "$time_val" =~ ^[0-9]+(\.[0-9]+)?$ ]]; then
        echo -e "\nОшибка парсинга времени: '$time_val'" >&2
        echo "$output" >&2
        exit 3
    fi

    echo "$time_val секунд"
    total_time=$(awk "BEGIN {printf \"%.6f\", $total_time + $time_val}")
done

avg_time=$(awk "BEGIN {printf \"%.6f\", $total_time / $RUNS}")
echo "========================================"
echo "Среднее время за $RUNS запусков: $avg_time секунд"
