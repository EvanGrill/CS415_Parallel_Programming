#!/bin/bash

#SBATCH --ntasks=2
#SBATCH --mem=2048MB
#SBATCH --time=00:30:00
#SBATCH --mail-user=grille@unr.edu
#SBATCH --mail-type=FAIL
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=1

rm -f buffer_test.csv
srun ./build/main 2 > buffer_test.csv