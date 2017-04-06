#!/bin/bash

#!/bin/bash

rm -f ./results.csv
echo "num_tasks,data_size,exec_time" > ./results.csv
for file in ./test/*.sh
do
    echo "Starting ${file}"
    sbatch ${file}
    sleep 1m
done