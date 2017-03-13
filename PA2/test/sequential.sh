#!/bin/bash

#SBATCH --ntasks=1
#SBATCH --mem=4096MB
#SBATCH --time=00:07:30
#SBATCH --mail-user=grille@unr.edu
#SBATCH --mail-type=NONE
#SBATCH --ntasks-per-core=1
#SBATCH --ntasks-per-node=1
#SBATCH --nodes=1
#SBATCH --output=sequential.csv
#SBATCH --error=/dev/null
#SBATCH --open-mode=append

srun --mpi=pmi2 ./build/main 32000 32000 1000 30 sequential.pgm