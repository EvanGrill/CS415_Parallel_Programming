#!/bin/bash

#SBATCH --ntasks=32
#SBATCH --mem=16384MB
#SBATCH --time=00:07:30
#SBATCH --mail-user=grille@unr.edu
#SBATCH --mail-type=NONE
#SBATCH --ntasks-per-core=1
#SBATCH --ntasks-per-node=8
#SBATCH --nodes=4
#SBATCH --output=results.csv
#SBATCH --error=/dev/null
#SBATCH --open-mode=append

srun --mpi=pmi2 ./build/main 32000 32000 255 1 parallel.pgm