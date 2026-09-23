#!/bin/bash
#SBATCH -N 1
#SBATCH -c 8
#SBATCH -t 300
#SBATCH -p k3
#SBATCH --output=slurm_%j_run_riscv_kokkos.out

threads=("1" "2" "4" "8")
genome_file="./../genome_samples/s103258.txt"
algos=("naive" "rabin_karp_rolling_hash" "rabin_karp_SWAR" "rabin_karp_SWAR2" "hash3")
codes=("scalar" "vector")

prog_path="../build/build_riscv_kokkos/perf/"
test_path="../build/build_riscv_kokkos/test/"

srun $test_path/most_common_string_test

echo "########## run kokkos riscv ##########"

for code in "${codes[@]}"; do
    for algo in "${algos[@]}"; do
    
        echo "============= ${algo}_${code}_perf ============="
    
        for thread_num in "${threads[@]}"; do
            
            echo "----- threads=${thread_num} -----"
            export OMP_NUM_THREADS=$thread_num
            numactl --physcpubind=0-$(($thread_num - 1)) $prog_path/${algo}_${code}_perf $genome_file
            unset OMP_NUM_THREADS

        done
    done
done
