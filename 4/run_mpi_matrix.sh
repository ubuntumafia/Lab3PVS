#!/bin/bash
set -euo pipefail

if [ "$#" -ne 4 ]; then
    echo "Usage: $0 <ranks> <rows> <cols> <num_runs>"
    exit 1
fi

RANKS=$1
ROWS=$2
COLS=$3
RUNS=$4
EXEC=./par_matrix_ops

if [ ! -x "$EXEC" ]; then
    echo "Error: $EXEC not found or not executable"
    exit 2
fi

echo "=== Running $EXEC with $RANKS ranks on ${ROWS}x${COLS} matrix (runs: $RUNS) ==="

total_time=0

for ((i=1; i<=RUNS; i++)); do
    echo -n "Run $i/$RUNS... "
    output=$(mpirun -np "$RANKS" "$EXEC" "$ROWS" "$COLS" 2>&1)
    time_val=$(echo "$output" | awk '/MATRIX_OPS_TIME:/ {print $2}')
    proc_count=$(echo "$output" | awk '/PROCESSES:/ {print $2}')
    
    if ! [[ "$time_val" =~ ^[0-9.]+$ ]]; then
        echo -e "\nError parsing time: '$time_val'"
        echo "$output"
        exit 3
    fi
    
    echo "$time_val seconds (processes: $proc_count)"
    total_time=$(awk "BEGIN {print $total_time + $time_val}")
done

avg_time=$(awk "BEGIN {printf \"%.6f\", $total_time / $RUNS}")
echo "========================================"
echo "Average time over $RUNS runs: $avg_time seconds"