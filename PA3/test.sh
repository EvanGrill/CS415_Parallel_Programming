#!/bin/bash

BATCHFILES="./test/*.sh"

rm -rf results.csv
echo "num_tasks,data_size,exec_time" > results.csv

for batchfile in $BATCHFILES
do

    TEST=$(squeue -o"%.18i %.9P %.20j %.20u %.8T %.10M %.9l %.6D %R")

    while [[ "$TEST" =~ "grille-" ]]
    do
        sleep 1s
        TEST=$(squeue -o"%.18i %.9P %.20j %.20u %.8T %.10M %.9l %.6D %R")
    done
    echo "Batching $batchfile"
    sbatch $batchfile
    sleep 1s
done