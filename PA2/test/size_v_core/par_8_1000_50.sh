#!/bin/bash

#SBATCH --ntasks=8
#SBATCH --mem=4GB
#SBATCH --time=00:00:30
#SBATCH --mincpus=4
#SBATCH --nodes=1
#SBATCH --output=results.csv
#SBATCH --error=/dev/null
#SBATCH --open-mode=append
#SBATCH --exclusive

srun --mpi=pmi2 ./build/main 1000 1000 255 parallel.pgm