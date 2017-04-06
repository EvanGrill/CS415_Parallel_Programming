#!/bin/bash

BATCHFILES="test/"


for batchfile in $BATCHFILES
do

    TEST=$(squeue -o"%.18i %.9P %.20j %.20u %.8T %.10M %.9l %.6D %R")

    while [[ "$TEST" =~ "grille-" ]]
    do
        sleep 1s
        TEST=$(squeue -o"%.18i %.9P %.20j %.20u %.8T %.10M %.9l %.6D %R")
    done

    sbatch $batchfile
    sleep 1s
done