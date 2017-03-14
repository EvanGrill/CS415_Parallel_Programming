#!/bin/bash

#SBATCH --ntasks=16
#SBATCH --mem=4GB
#SBATCH --time=00:07:00
#SBATCH --mincpus=8
#SBATCH --nodes=2
#SBATCH --output=results.csv
#SBATCH --error=/dev/null
#SBATCH --open-mode=append
#SBATCH --exclusive

srun --mpi=pmi2 ./build/main 32000 32000 255 parallel.pgm