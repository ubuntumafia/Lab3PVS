#!/bin/bash
set -euo pipefail

if [ "$#" -ne 3 ]; then
    echo "Usage: $0 <rows> <cols> <num_runs>"
    exit 1
fi

ROWS=$1
COLS=$2
RUNS=$3
EXEC=./seq_matrix_ops

if [ ! -x "$EXEC" ]; then
    echo "Error: $EXEC not found or not executable"
    exit 2
fi

echo "=== Running $EXEC with ${ROWS}x${COLS} matrix (runs: $RUNS) ==="

total_time=0

for ((i=1; i<=RUNS; i++)); do
    echo -n "Run $i/$RUNS... "
    output=$("$EXEC" "$ROWS" "$COLS" 2>&1)
    time_val=$(echo "$output" | awk '/Time:/ {print $(NF-1)}')
    
    if ! [[ "$time_val" =~ ^[0-9.]+$ ]]; then
        echo -e "\nError parsing time: '$time_val'"
        echo "$output"
        exit 3
    fi
    
    echo "$time_val seconds"
    total_time=$(awk "BEGIN {print $total_time + $time_val}")
done

avg_time=$(awk "BEGIN {printf \"%.6f\", $total_time / $RUNS}")
echo "========================================"
echo "Average time over $RUNS runs: $avg_time seconds"