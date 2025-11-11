#!/bin/bash -l

#SBATCH -p nodes
#SBATCH -D ./
#SBATCH -t 15
#SBATCH --export=ALL

procs=${SLURM_NTASKS:-1} #Gets the requested number of processes when sbatch -n is used

cores=${SLURM_CPUS_PER_TASK:-1} #Gets the requested number of cores when sbatch -c is used

# 1. Add code here to compile a program with mpiicc and also with omp

# 2. Add code here to run the program with mpirun -np $procs program_name

# 3. Then submit this script to the compute nodes. 
#    Instructions for how to do this are in the lab
