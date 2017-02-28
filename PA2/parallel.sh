#!/bin/bash

#SBATCH --ntasks=10
#SBATCH --mem=2048MB
#SBATCH --time=00:30:00
#SBATCH --mail-user=grille@unr.edu
#SBATCH --mail-type=FAIL
#SBATCH --nodes=10

srun ./build/main 25000 25000 1000 sequential.ppm