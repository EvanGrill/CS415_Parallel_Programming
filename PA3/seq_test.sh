#!/bin/bash

rm -f ./results.csv
echo "num_nums,exec_time" >> ./results.csv
for file in ./test/*.txt
do
    srun --ntasks=1 --nodes=1 --mpi=pmi2 --exclusive --mem=16384 ./build/main ${file} >> ./results.csv
done