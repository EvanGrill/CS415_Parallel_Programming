#!/bin/bash

rm -f ./results.csv
echo "num_tasks,pixel_size,num_rows,exec_time" > ./results.csv
for file in ./test/rows_v_core/par_*.sh
do
    echo "Starting ${file}"
    sbatch ${file}
    sleep 7.5m
done