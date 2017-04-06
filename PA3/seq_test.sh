#!/bin/bash

rm -f ./results.csv
echo "num_tasks,data_size,exec_time" >> ./results.csv
for file in ./test/*.sh
do
    sbatch ${file}
done