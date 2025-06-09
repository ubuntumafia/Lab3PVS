#!/bin/bash
if [ $# -ne 2 ]; then
  echo "Использование: $0 ARRAY_SIZE NUM_RUNS"
  exit 1
fi

ARRAY_SIZE=$1
NUM_RUNS=$2
total_time=0

for ((i=1; i<=NUM_RUNS; i++)); do
  # получаем вывод вида "Последовательная программа завершена\nВремя выполнения: ... секунд"
  line=$(./seq_array_ops "$ARRAY_SIZE" | grep "Время выполнения")
  # извлекаем последнее поле (время)
  t=$(awk '{print $3}' <<< "$line")
  total_time=$(awk -v acc="$total_time" -v dt="$t" 'BEGIN{printf "%.9f", acc + dt}')
done

# считаем среднее
avg_time=$(awk -v acc="$total_time" -v n="$NUM_RUNS" 'BEGIN{printf "%.9f", acc / n}')
echo "Размер массива: $ARRAY_SIZE"
echo "Число запусков: $NUM_RUNS"
echo "Среднее время: $avg_time секунд"
