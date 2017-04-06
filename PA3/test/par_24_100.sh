#!/bin/bash

#SBATCH --ntasks=24
#SBATCH --mem=16GB
#SBATCH --mincpus=8
#SBATCH --nodes=3
#SBATCH --output=results.csv
#SBATCH --error=/dev/null
#SBATCH --open-mode=append
#SBATCH --exclusive


srun --mpi=pmi2 ./build/main 100