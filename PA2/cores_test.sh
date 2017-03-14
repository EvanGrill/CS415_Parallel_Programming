#!/bin/bash

rm -f ./results.csv
echo "num_tasks, pixel_size, exec_time" > ./results.csv
for file in ./test/size_v_core/par_*.sh
do
    echo "Starting ${file}"
    sbatch ${file}
    sleep 3m
done