#!/bin/bash

#SBATCH --ntasks=16
#SBATCH --nodes=2
#SBATCH --output=results.csv
#SBATCH --error=/dev/null
#SBATCH --open-mode=append
#SBATCH --exclusive


srun --mpi=pmi2 ./build/main 1000