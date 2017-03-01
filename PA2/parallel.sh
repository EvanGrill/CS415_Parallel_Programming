#!/bin/bash

#SBATCH --ntasks=64
#SBATCH --mem=32768MB
#SBATCH --time=00:30:00
#SBATCH --mail-user=grille@unr.edu
#SBATCH --mail-type=FAIL

srun ./build/main 1000 1000 1000 sequential.pgm