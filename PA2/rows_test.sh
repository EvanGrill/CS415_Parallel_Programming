#!/bin/bash

echo "num_tasks,pixel_size,num_rows,exec_time\r\n" > ./results.csv
for file in ./test/rows_v_core/par_*.sh
do
    echo "Starting ${file}\n"
    sbatch ${file}
    sleep 7.5m
done