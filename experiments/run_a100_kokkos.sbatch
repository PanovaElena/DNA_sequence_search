#!/bin/bash
#SBATCH -N 1
#SBATCH -t 100
#SBATCH -p p5gpuA100
#SBATCH --gres gpu:1
#SBATCH --output=slurm_%j_run_a100_kokkos.out

codes=("scalar" "vector")
algos=("naive" "rabin_karp_rolling_hash" "rabin_karp_SWAR" "rabin_karp_SWAR2" "hash3")

genome_file="./../genome_samples/s521893.txt"
prog_path="../build/build_nvidia_gpu_kokkos/perf/"
test_path="../build/build_nvidia_gpu_kokkos/test/"

srun $test_path/most_common_string_test

echo "########## run kokkos A100 ##########"

for code in "${codes[@]}"; do
    for algo in "${algos[@]}"; do
    
        echo "============= ${algo}_${code}_perf ============="
        srun $prog_path/${algo}_${code}_perf $genome_file
        
    done
done
