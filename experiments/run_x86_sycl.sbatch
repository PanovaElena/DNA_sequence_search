#!/bin/bash
#SBATCH -N 1
#SBATCH -c 152
#SBATCH -t 300
#SBATCH -p p6cpuCascadeLake
#SBATCH --output=slurm_%j_run_x86_sycl.out

threads=("1" "2" "4" "8" "19" "38" "76" "152")
genome_file="./../genome_samples/s521893.txt"
algos=("naive" "rabin_karp_rolling_hash" "rabin_karp_SWAR" "rabin_karp_SWAR2" "hash3")
codes=("scalar")

# AOT
prog_path="../build/build_x86_sycl_aot/perf/"
test_path="../build/build_x86_sycl_aot/test/"
# JIT
#prog_path="../build/build_x86_sycl/perf/"
#test_path="../build/build_x86_sycl/test/"

srun $test_path/most_common_string_test

echo "########## run sycl x86 ##########"

for code in "${codes[@]}"; do
    for algo in "${algos[@]}"; do
    
        echo "============= ${algo}_${code}_perf ============="
    
        for thread_num in "${threads[@]}"; do
            
            echo "----- threads=${thread_num} -----"
            export DPCPP_CPU_NUM_CUS=$thread_num
            numactl --physcpubind=0-$(($thread_num - 1)) $prog_path/${algo}_${code}_perf $genome_file
            unset DPCPP_CPU_NUM_CUS

        done
    done
done
