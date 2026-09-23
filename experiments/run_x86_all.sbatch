#!/bin/bash
#SBATCH -N 1
#SBATCH -c 152
#SBATCH -t 400
#SBATCH -p p6cpuCascadeLake
#SBATCH --output=slurm_%j_run_x86_all.out

threads=("1" "2" "4" "8" "19" "38" "76" "152")
genome_file="./../genome_samples/s521893.txt"
algos=("naive" "rabin_karp_rolling_hash" "rabin_karp_SWAR" "rabin_karp_SWAR2" "hash3")
algos_block=("naive_block" "rabin_karp_rolling_hash_block")

# -------------------------------------------------------------------
echo "===================== openmp ====================="
prog_path="../build/build_x86_openmp/perf/"
test_path="../build/build_x86_openmp/test/"
codes=("scalar" "vector")

srun $test_path/most_common_string_test

echo "########## run openmp x86 ##########"

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

for algo in "${algos_block[@]}"; do
    
    echo "============= ${algo}_vector_perf ============="
    
    for thread_num in "${threads[@]}"; do
        
        echo "----- threads=${thread_num} -----"
        export OMP_NUM_THREADS=$thread_num
        numactl --physcpubind=0-$(($thread_num - 1)) $prog_path/${algo}_vector_perf $genome_file
        unset OMP_NUM_THREADS
    
    done
done

# -------------------------------------------------------------------
echo "===================== kokkos ====================="
prog_path="../build/build_x86_kokkos/perf/"
test_path="../build/build_x86_kokkos/test/"
codes=("scalar" "vector")

srun $test_path/most_common_string_test

echo "########## run kokkos x86 ##########"

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

# -------------------------------------------------------------------
echo "===================== sycl ====================="
# AOT
prog_path="../build/build_x86_sycl_aot/perf/"
test_path="../build/build_x86_sycl_aot/test/"
# JIT
#prog_path="../build/build_x86_sycl/perf/"
#test_path="../build/build_x86_sycl/test/"
codes=("scalar")

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

## -------------------------------------------------------------------
#echo "===================== caf ====================="
#prog_path="../build/build_x86_caf/perf/"
#test_path="../build/build_x86_caf/test/"
#codes=("scalar")
#
#export I_MPI_OFI_PROVIDER=Verbs
#export I_MPI_FABRICS=shm
#
#srun $test_path/most_common_string_test
#
#echo "########## run caf x86 ##########"
#
#for code in "${codes[@]}"; do
#    for algo in "${algos[@]}"; do
#    
#        echo "============= ${algo}_${code}_perf ============="
#    
#        for thread_num in "${threads[@]}"; do
#            
#            echo "----- threads=${thread_num} -----"
#            export OMP_NUM_THREADS=$thread_num
#            numactl --physcpubind=0-$(($thread_num - 1)) $prog_path/${algo}_${code}_perf $genome_file
#            unset OMP_NUM_THREADS
#
#        done
#    done
#done
