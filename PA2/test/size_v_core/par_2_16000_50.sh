#!/bin/bash

#SBATCH --ntasks=2
#SBATCH --mem=4GB
#SBATCH --time=00:05:00
#SBATCH --mincpus=2
#SBATCH --nodes=1
#SBATCH --output=results.csv
#SBATCH --error=/dev/null
#SBATCH --open-mode=append
#SBATCH --exclusive

srun --mpi=pmi2 ./build/main 16000 16000 255 parallel.pgm