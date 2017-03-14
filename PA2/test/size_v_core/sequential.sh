#!/bin/bash

#SBATCH --ntasks=1
#SBATCH --mem=4GB
#SBATCH --time=00:07:00
#SBATCH --mincpus=8
#SBATCH --nodes=1
#SBATCH --error=/dev/null
#SBATCH --exclusive

srun --mpi=pmi2 ./build/main 25000 25000 255 sequential.pgm
srun --mpi=pmi2 ./build/main 50000 50000 255 sequential.pgm
srun --mpi=pmi2 ./build/main 75000 75000 255 sequential.pgm