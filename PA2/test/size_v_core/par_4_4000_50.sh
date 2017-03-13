#!/bin/bash

#SBATCH --ntasks=4
#SBATCH --mem=16384MB
#SBATCH --time=00:07:30
#SBATCH --mail-user=grille@unr.edu
#SBATCH --mail-type=NONE
#SBATCH --ntasks-per-core=1
#SBATCH --ntasks-per-node=8
#SBATCH --nodes=1
#SBATCH --output=results.csv
#SBATCH --error=/dev/null
#SBATCH --open-mode=append

srun --mpi=pmi2 ./build/main 4000 4000 255 50 parallel.pgm