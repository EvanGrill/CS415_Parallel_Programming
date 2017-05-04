#!/bin/bash
# While this is "techincally" sbatch compatible, it's not
# designed to use sbatch.  Instead, it just makes calls to
# srun successively to ensure that only one job is queued
# at a time.
srun -n 4 -N 1 --mpi=pmi2 ./build/main 120 >> output.csv
srun -n 4 -N 1 --mpi=pmi2 ./build/main 240 >> output.csv
srun -n 4 -N 1 --mpi=pmi2 ./build/main 360 >> output.csv
srun -n 4 -N 1 --mpi=pmi2 ./build/main 480 >> output.csv
srun -n 4 -N 1 --mpi=pmi2 ./build/main 600 >> output.csv
srun -n 4 -N 1 --mpi=pmi2 ./build/main 720 >> output.csv
srun -n 4 -N 1 --mpi=pmi2 ./build/main 840 >> output.csv
srun -n 4 -N 1 --mpi=pmi2 ./build/main 960 >> output.csv
srun -n 4 -N 1 --mpi=pmi2 ./build/main 1080 >> output.csv
srun -n 4 -N 1 --mpi=pmi2 ./build/main 1200 >> output.csv
srun -n 4 -N 1 --mpi=pmi2 ./build/main 1320 >> output.csv
srun -n 4 -N 1 --mpi=pmi2 ./build/main 1440 >> output.csv
srun -n 4 -N 1 --mpi=pmi2 ./build/main 1560 >> output.csv
srun -n 4 -N 1 --mpi=pmi2 ./build/main 1680 >> output.csv
srun -n 4 -N 1 --mpi=pmi2 ./build/main 1800 >> output.csv
srun -n 4 -N 1 --mpi=pmi2 ./build/main 1920 >> output.csv
srun -n 4 -N 1 --mpi=pmi2 ./build/main 2040 >> output.csv
srun -n 4 -N 1 --mpi=pmi2 ./build/main 2160 >> output.csv
srun -n 4 -N 1 --mpi=pmi2 ./build/main 2280 >> output.csv
srun -n 4 -N 1 --mpi=pmi2 ./build/main 2400 >> output.csv
srun -n 4 -N 1 --mpi=pmi2 ./build/main 2520 >> output.csv
srun -n 4 -N 1 --mpi=pmi2 ./build/main 2640 >> output.csv
srun -n 4 -N 1 --mpi=pmi2 ./build/main 2760 >> output.csv
srun -n 4 -N 1 --mpi=pmi2 ./build/main 2880 >> output.csv
srun -n 4 -N 1 --mpi=pmi2 ./build/main 3000 >> output.csv
srun -n 4 -N 1 --mpi=pmi2 ./build/main 3120 >> output.csv
srun -n 4 -N 1 --mpi=pmi2 ./build/main 3240 >> output.csv
srun -n 4 -N 1 --mpi=pmi2 ./build/main 3360 >> output.csv
srun -n 4 -N 1 --mpi=pmi2 ./build/main 3480 >> output.csv
srun -n 4 -N 1 --mpi=pmi2 ./build/main 3600 >> output.csv
srun -n 4 -N 1 --mpi=pmi2 ./build/main 3720 >> output.csv
srun -n 4 -N 1 --mpi=pmi2 ./build/main 3840 >> output.csv
srun -n 4 -N 1 --mpi=pmi2 ./build/main 3960 >> output.csv
srun -n 4 -N 1 --mpi=pmi2 ./build/main 4080 >> output.csv
srun -n 4 -N 1 --mpi=pmi2 ./build/main 4200 >> output.csv
srun -n 4 -N 1 --mpi=pmi2 ./build/main 4320 >> output.csv
srun -n 4 -N 1 --mpi=pmi2 ./build/main 4440 >> output.csv
srun -n 4 -N 1 --mpi=pmi2 ./build/main 4560 >> output.csv