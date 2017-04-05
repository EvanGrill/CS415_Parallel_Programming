#!/bin/bash

rm -f ./results.csv
echo "num_nums,exec_time" >> ./results.csv
for file in ./test/*.txt
do
    mpirun -n 8 ./build/main ${file} >> ./results.csv
done