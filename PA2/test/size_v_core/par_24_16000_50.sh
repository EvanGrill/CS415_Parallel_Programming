#!/bin/bash

#SBATCH --ntasks=24
#SBATCH --mem=4GB
#SBATCH --time=00:04:00
#SBATCH --mincpus=8
#SBATCH --nodes=3
#SBATCH --output=results.csv
#SBATCH --error=/dev/null
#SBATCH --open-mode=append
#SBATCH --exclusive

srun --mpi=pmi2 ./build/main 16000 16000 255 50 parallel.pgm