#!/bin/bash

#SBATCH --ntasks=24
#SBATCH --nodes=4
#SBATCH --ntasks-per-node=8
#SBATCH --output=results.csv
#SBATCH --error=/dev/null
#SBATCH --open-mode=append
#SBATCH --exclusive


srun --mpi=pmi2 ./build/main 10000000