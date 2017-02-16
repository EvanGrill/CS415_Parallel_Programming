#!/bin/bash

#SBATCH --ntasks=2
#SBATCH --mem=2048MB
#SBATCH --time=00:05:00
#SBATCH --mail-user=grille@unr.edu
#SBATCH --mail-type=FAIL
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=2

srun --mpi=pmi2 ./main