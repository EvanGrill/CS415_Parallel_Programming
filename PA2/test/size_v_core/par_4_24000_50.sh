#!/bin/bash

#SBATCH --ntasks=4
#SBATCH --mem=4GB
#SBATCH --time=00:06:00
#SBATCH --mincpus=4
#SBATCH --nodes=1
#SBATCH --output=results.csv
#SBATCH --error=/dev/null
#SBATCH --open-mode=append
#SBATCH --exclusive

srun --mpi=pmi2 ./build/main 24000 24000 255 50 parallel.pgm