#!/bin/bash

#SBATCH --ntasks=32
#SBATCH --mem=16384MB
#SBATCH --time=00:07:30
#SBATCH --ntasks-per-core=1
#SBATCH --nodes=4
#SBATCH --output=results.csv
#SBATCH --error=/dev/null
#SBATCH --open-mode=append

srun --mpi=pmi2 ./build/main 16000 16000 255 1 parallel.pgm