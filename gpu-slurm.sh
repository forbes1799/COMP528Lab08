#!/bin/bash -l

#SBATCH -p gpu-l40s
#SBATCH --gres=gpu:1
#SBATCH -D ./
#SBATCH --export=ALL

./deviceQuery
