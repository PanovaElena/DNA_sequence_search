#include "test_common.hpp"

void host_naive(std::vector<uint32_t>& freq, const std::string& input_file, const uint32_t len_);

class Algo : public ::testing::TestWithParam<uint32_t>
{
public:
    uint32_t lenght = GetParam();
    std::string path = TEST_GENOME_FILE;
    std::vector<uint32_t> freq1;
    std::vector<uint32_t> freq2;   
};

INSTANTIATE_TEST_SUITE_P(AlgoTestParams,
                         Algo,
                         testing::Values(13, 65, 129));

// ---------- scalar tests or CAF tests ----------

#if (defined(__OPENMP__) || defined(__KOKKOS__) || defined(__SYCL__) || defined(__CUDA__) || defined(__CAF__))

TEST_P(Algo, test_naive_host_device)
{
    host_naive(freq1, path, lenght);
    naive_scalar(freq2, path, lenght);
    EXPECT_EQ(freq1, freq2);
}

TEST_P(Algo, test_rabin_karp_rolling_hash_scalar)
{
    rabin_karp_rolling_hash_scalar(freq1, path, lenght);
    naive_scalar(freq2, path, lenght);
    EXPECT_EQ(freq1, freq2);
}

TEST_P(Algo, test_rabin_karp_SWAR_scalar)
{
    rabin_karp_SWAR_scalar(freq1, path, lenght);
    naive_scalar(freq2, path, lenght);
    EXPECT_EQ(freq1, freq2);
}

TEST_P(Algo, test_rabin_karp_SWAR2_scalar)
{
    rabin_karp_SWAR2_scalar(freq1, path, lenght);
    naive_scalar(freq2, path, lenght);
    EXPECT_EQ(freq1, freq2);
}

TEST_P(Algo, test_hash3_scalar)
{
    hash3_scalar(freq1, path, lenght);
    naive_scalar(freq2, path, lenght);
    EXPECT_EQ(freq1, freq2);
}

#endif

// ---------- vector tests ----------

#if (defined(__OPENMP__) || defined(__KOKKOS__) || defined(__CUDA__) || (defined(__SYCL__) && defined(__ESIMD__)))

TEST_P(Algo, test_naive_vector)
{
    naive_vector(freq1, path, lenght);
    naive_scalar(freq2, path, lenght);
    EXPECT_EQ(freq1, freq2);
}

TEST_P(Algo, test_rabin_karp_rolling_hash_vector)
{
    rabin_karp_rolling_hash_vector(freq1, path, lenght);
    naive_vector(freq2, path, lenght);
    EXPECT_EQ(freq1, freq2);
}

#if (!defined(__KOKKOS__) && !defined(__SYCL__) && !defined(__CUDA__))
TEST_P(Algo, test_rabin_karp_SWAR_vector)
{
    rabin_karp_SWAR_vector(freq1, path, lenght);
    naive_vector(freq2, path, lenght);
    EXPECT_EQ(freq1, freq2);
}
#endif

TEST_P(Algo, test_rabin_karp_SWAR2_vector)
{
    rabin_karp_SWAR2_vector(freq1, path, lenght);
    naive_vector(freq2, path, lenght);
    EXPECT_EQ(freq1, freq2);
}

TEST_P(Algo, test_hash3_vector)
{
    hash3_vector(freq1, path, lenght);
    naive_vector(freq2, path, lenght);
    EXPECT_EQ(freq1, freq2);
}

#endif

#if (defined(__OPENMP__) && (defined(__X86__) || defined(__RISCV__)))

TEST_P(Algo, test_naive_block_vector)
{
    naive_block_vector(freq1, path, lenght);
    naive_vector(freq2, path, lenght);
    EXPECT_EQ(freq1, freq2);
}
    
TEST_P(Algo, test_rabin_karp_rolling_hash_block_vector)
{
    rabin_karp_rolling_hash_block_vector(freq1, path, lenght);
    naive_vector(freq2, path, lenght);
    EXPECT_EQ(freq1, freq2);
}

#endif
