#!/bin/bash
# run_mpi_bubble.sh — запускает MPI-программу par_bubble несколько раз и усредняет время
#
# Использование: ./run_mpi_bubble.sh <число_рангов> <кол-во_элементов>
# Пример:   ./run_mpi_bubble.sh 6 600000

set -euo pipefail

if [ "$#" -ne 2 ]; then
    echo "Использование: $0 <число_рангов> <кол-во_элементов>" >&2
    exit 1
fi

RANKS=$1
ELEMENTS=$2
EXEC=./par_bubble
RUNS=10   # сколько запусков для усреднения

# Проверяем, что наш исполняемый файл есть и он исполняем
if [ ! -x "$EXEC" ]; then
    echo "Ошибка: $EXEC не найден или не исполняем" >&2
    exit 2
fi

# Проверка делимости, чтобы par_bubble не кидал MPI_Abort
if (( ELEMENTS % RANKS != 0 )); then
    echo "Ошибка: ELEMENTS ($ELEMENTS) не делится на RANKS ($RANKS)" >&2
    exit 3
fi

echo "=== Запуск $EXEC на $RANKS рангах, N=$ELEMENTS (запусков: $RUNS) ==="

# Настройки OpenMPI (при необходимости)
export OMPI_MCA_btl="self,vader,tcp"
export OMPI_MCA_mtl="^psm,psm2,psm3"
export OMPI_MCA_pml="ob1"

# Передаём размер массива через переменную окружения
export ARRAY_SIZE="$ELEMENTS"

total_time=0
count=0

for ((i=1; i<=RUNS; i++)); do
    echo -n "Запуск $i/$RUNS… "

    # Запускаем MPI-программу
    output=$(mpirun -np "$RANKS" "$EXEC" 2>&1)
    rc=$?
    if [ $rc -ne 0 ]; then
        echo -e "\nОшибка: par_bubble завершился с кодом $rc" >&2
        echo "$output" >&2
        exit 4
    fi

    # Ищем строку "ВРЕМЯ_ВЫПОЛНЕНИЯ: 0.123456"
    time_val=$(echo "$output" | awk '/ВРЕМЯ_ВЫПОЛНЕНИЯ:/ {print $2}')

    if ! [[ "$time_val" =~ ^[0-9]+(\.[0-9]+)?$ ]]; then
        echo -e "\nОшибка парсинга времени: '$time_val'" >&2
        echo "$output" >&2
        exit 5
    fi

    echo "$time_val секунд"
    total_time=$(awk "BEGIN {print $total_time + $time_val}")
    count=$((count + 1))
done

avg_time=$(awk "BEGIN {printf \"%.6f\", $total_time / $count}")
echo "========================================"
echo "Среднее время за $count запусков: $avg_time секунд"
