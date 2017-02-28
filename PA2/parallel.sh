#!/bin/bash

#SBATCH --ntasks=16
#SBATCH --mem=16384MB
#SBATCH --time=00:30:00
#SBATCH --mail-user=grille@unr.edu
#SBATCH --mail-type=FAIL

srun ./build/main 25000 25000 256 sequential.pgm