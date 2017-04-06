#!/bin/bash

#SBATCH --ntasks=2
#SBATCH --nodes=1
#SBATCH --output=results.csv
#SBATCH --error=/dev/null
#SBATCH --open-mode=append
#SBATCH --exclusive


srun --mpi=pmi2 ./build/main 1000000000