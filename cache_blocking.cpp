#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <iomanip>
#include <cstring>

class CacheBlockingDemo {
private:
    static constexpr size_t CACHE_LINE_SIZE = 64;
    static constexpr size_t L1_CACHE_SIZE = 32 * 1024;      // 32 KB typical L1
    static constexpr size_t L2_CACHE_SIZE = 256 * 1024;     // 256 KB typical L2
    static constexpr size_t L3_CACHE_SIZE = 8 * 1024 * 1024; // 8 MB typical L3

public:
    // Matrix Multiplication: Classic Cache Blocking Example
    void matrixMultiplicationBlocking() {
        std::cout << "\n=== Matrix Multiplication Cache Blocking ===\n";
        
        const int N = 1024;
        std::vector<std::vector<float>> A(N, std::vector<float>(N));
        std::vector<std::vector<float>> B(N, std::vector<float>(N));
        std::vector<std::vector<float>> C1(N, std::vector<float>(N, 0));
        std::vector<std::vector<float>> C2(N, std::vector<float>(N, 0));
        
        // Initialize matrices
        initializeMatrix(A, N);
        initializeMatrix(B, N);
        
        // Naive implementation (i-j-k order)
        auto start = std::chrono::high_resolution_clock::now();
        naiveMatrixMultiply(A, B, C1, N);
        auto end = std::chrono::high_resolution_clock::now();
        auto naive_time = std::chrono::duration<double>(end - start).count();
        
        // Blocked implementation
        start = std::chrono::high_resolution_clock::now();
        blockedMatrixMultiply(A, B, C2, N, 64); // 64x64 blocks
        end = std::chrono::high_resolution_clock::now();
        auto blocked_time = std::chrono::duration<double>(end - start).count();
        
        std::cout << std::fixed << std::setprecision(4);
        std::cout << "Naive time:   " << naive_time << "s\n";
        std::cout << "Blocked time: " << blocked_time << "s\n";
        std::cout << "Speedup:      " << naive_time / blocked_time << "x\n";
        
        // Verify correctness
        if (verifyResults(C1, C2, N)) {
            std::cout << "✓ Results match!\n";
        } else {
            std::cout << "✗ Results differ!\n";
        }
    }
    
    // Matrix Transpose: Demonstrates Spatial Locality
    void matrixTransposeBlocking() {
        std::cout << "\n=== Matrix Transpose Cache Blocking ===\n";
        
        const int N = 4096;
        std::vector<std::vector<float>> A(N, std::vector<float>(N));
        std::vector<std::vector<float>> B1(N, std::vector<float>(N));
        std::vector<std::vector<float>> B2(N, std::vector<float>(N));
        
        initializeMatrix(A, N);
        
        // Naive transpose
        auto start = std::chrono::high_resolution_clock::now();
        naiveTranspose(A, B1, N);
        auto end = std::chrono::high_resolution_clock::now();
        auto naive_time = std::chrono::duration<double>(end - start).count();
        
        // Blocked transpose
        start = std::chrono::high_resolution_clock::now();
        blockedTranspose(A, B2, N, 64);
        end = std::chrono::high_resolution_clock::now();
        auto blocked_time = std::chrono::duration<double>(end - start).count();
        
        std::cout << std::fixed << std::setprecision(4);
        std::cout << "Naive transpose time:   " << naive_time << "s\n";
        std::cout << "Blocked transpose time: " << blocked_time << "s\n";
        std::cout << "Speedup:                " << naive_time / blocked_time << "x\n";
    }
    
    // Cache-Oblivious vs Cache-Aware Algorithms
    void cacheObliviousDemo() {
        std::cout << "\n=== Cache-Oblivious Matrix Multiplication ===\n";
        
        const int N = 512;
        std::vector<std::vector<float>> A(N, std::vector<float>(N));
        std::vector<std::vector<float>> B(N, std::vector<float>(N));
        std::vector<std::vector<float>> C1(N, std::vector<float>(N, 0));
        std::vector<std::vector<float>> C2(N, std::vector<float>(N, 0));
        
        initializeMatrix(A, N);
        initializeMatrix(B, N);
        
        // Standard blocked approach
        auto start = std::chrono::high_resolution_clock::now();
        blockedMatrixMultiply(A, B, C1, N, 64);
        auto end = std::chrono::high_resolution_clock::now();
        auto blocked_time = std::chrono::duration<double>(end - start).count();
        
        // Cache-oblivious recursive approach
        start = std::chrono::high_resolution_clock::now();
        cacheObliviousMatrixMultiply(A, B, C2, 0, 0, 0, 0, 0, 0, N);
        end = std::chrono::high_resolution_clock::now();
        auto recursive_time = std::chrono::duration<double>(end - start).count();
        
        std::cout << std::fixed << std::setprecision(4);
        std::cout << "Blocked time:           " << blocked_time << "s\n";
        std::cout << "Cache-oblivious time:   " << recursive_time << "s\n";
        std::cout << "Ratio:                  " << blocked_time / recursive_time << "\n";
    }
    
    // Memory Access Pattern Analysis
    void memoryAccessPatterns() {
        std::cout << "\n=== Memory Access Pattern Analysis ===\n";
        
        const size_t SIZE = 64 * 1024 * 1024; // 64M floats = 256MB
        std::vector<float> data(SIZE);
        
        // Initialize with random data
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(0.0f, 1.0f);
        for (auto& val : data) {
            val = dis(gen);
        }
        
        std::cout << "Array size: " << SIZE * sizeof(float) / (1024 * 1024) << " MB\n";
        
        // Test different access patterns
        testAccessPattern(data, SIZE, 1, "Sequential");
        testAccessPattern(data, SIZE, 2, "Stride-2");
        testAccessPattern(data, SIZE, 4, "Stride-4");
        testAccessPattern(data, SIZE, 8, "Stride-8");
        testAccessPattern(data, SIZE, 16, "Stride-16");
        testAccessPattern(data, SIZE, 64, "Stride-64 (cache line)");
        testAccessPattern(data, SIZE, 1024, "Stride-1024");
    }

private:
    void initializeMatrix(std::vector<std::vector<float>>& matrix, int N) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(0.0f, 1.0f);
        
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                matrix[i][j] = dis(gen);
            }
        }
    }
    
    // Naive O(n³) matrix multiplication
    void naiveMatrixMultiply(const std::vector<std::vector<float>>& A,
                           const std::vector<std::vector<float>>& B,
                           std::vector<std::vector<float>>& C, int N) {
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                for (int k = 0; k < N; ++k) {
                    C[i][j] += A[i][k] * B[k][j];
                }
            }
        }
    }
    
    // Cache-blocked matrix multiplication
    void blockedMatrixMultiply(const std::vector<std::vector<float>>& A,
                             const std::vector<std::vector<float>>& B,
                             std::vector<std::vector<float>>& C, 
                             int N, int block_size) {
        for (int ii = 0; ii < N; ii += block_size) {
            for (int jj = 0; jj < N; jj += block_size) {
                for (int kk = 0; kk < N; kk += block_size) {
                    // Multiply blocks
                    int i_max = std::min(ii + block_size, N);
                    int j_max = std::min(jj + block_size, N);
                    int k_max = std::min(kk + block_size, N);
                    
                    for (int i = ii; i < i_max; ++i) {
                        for (int j = jj; j < j_max; ++j) {
                            for (int k = kk; k < k_max; ++k) {
                                C[i][j] += A[i][k] * B[k][j];
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Naive matrix transpose
    void naiveTranspose(const std::vector<std::vector<float>>& A,
                       std::vector<std::vector<float>>& B, int N) {
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                B[j][i] = A[i][j];
            }
        }
    }
    
    // Cache-blocked matrix transpose
    void blockedTranspose(const std::vector<std::vector<float>>& A,
                         std::vector<std::vector<float>>& B, 
                         int N, int block_size) {
        for (int ii = 0; ii < N; ii += block_size) {
            for (int jj = 0; jj < N; jj += block_size) {
                int i_max = std::min(ii + block_size, N);
                int j_max = std::min(jj + block_size, N);
                
                for (int i = ii; i < i_max; ++i) {
                    for (int j = jj; j < j_max; ++j) {
                        B[j][i] = A[i][j];
                    }
                }
            }
        }
    }
    
    // Cache-oblivious recursive matrix multiplication
    void cacheObliviousMatrixMultiply(const std::vector<std::vector<float>>& A,
                                    const std::vector<std::vector<float>>& B,
                                    std::vector<std::vector<float>>& C,
                                    int row_a, int col_a, int row_b, int col_b,
                                    int row_c, int col_c, int size) {
        if (size <= 64) {  // Base case: small enough for naive multiplication
            for (int i = 0; i < size; ++i) {
                for (int j = 0; j < size; ++j) {
                    for (int k = 0; k < size; ++k) {
                        C[row_c + i][col_c + j] += 
                            A[row_a + i][col_a + k] * B[row_b + k][col_b + j];
                    }
                }
            }
            return;
        }
        
        // Divide and conquer
        int half = size / 2;
        
        // C11 = A11 * B11 + A12 * B21
        cacheObliviousMatrixMultiply(A, B, C, row_a, col_a, row_b, col_b, 
                                   row_c, col_c, half);
        cacheObliviousMatrixMultiply(A, B, C, row_a, col_a + half, 
                                   row_b + half, col_b, row_c, col_c, half);
        
        // C12 = A11 * B12 + A12 * B22
        cacheObliviousMatrixMultiply(A, B, C, row_a, col_a, row_b, col_b + half,
                                   row_c, col_c + half, half);
        cacheObliviousMatrixMultiply(A, B, C, row_a, col_a + half, 
                                   row_b + half, col_b + half, 
                                   row_c, col_c + half, half);
        
        // C21 = A21 * B11 + A22 * B21
        cacheObliviousMatrixMultiply(A, B, C, row_a + half, col_a, row_b, col_b,
                                   row_c + half, col_c, half);
        cacheObliviousMatrixMultiply(A, B, C, row_a + half, col_a + half,
                                   row_b + half, col_b, row_c + half, col_c, half);
        
        // C22 = A21 * B12 + A22 * B22
        cacheObliviousMatrixMultiply(A, B, C, row_a + half, col_a, 
                                   row_b, col_b + half, 
                                   row_c + half, col_c + half, half);
        cacheObliviousMatrixMultiply(A, B, C, row_a + half, col_a + half,
                                   row_b + half, col_b + half,
                                   row_c + half, col_c + half, half);
    }
    
    bool verifyResults(const std::vector<std::vector<float>>& C1,
                      const std::vector<std::vector<float>>& C2, int N) {
        const float EPSILON = 1e-5f;
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                if (std::abs(C1[i][j] - C2[i][j]) > EPSILON) {
                    return false;
                }
            }
        }
        return true;
    }
    
    void testAccessPattern(const std::vector<float>& data, size_t size, 
                          size_t stride, const std::string& name) {
        volatile float sum = 0.0f;  // volatile to prevent optimization
        size_t iterations = size / stride;
        
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < iterations; ++i) {
            sum += data[i * stride];
        }
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration<double>(end - start).count();
        double bandwidth = (iterations * sizeof(float)) / (duration * 1024 * 1024); // MB/s
        
        std::cout << std::setw(20) << name << ": " 
                  << std::setw(8) << std::fixed << std::setprecision(4) << duration << "s, "
                  << std::setw(8) << std::fixed << std::setprecision(1) << bandwidth << " MB/s\n";
    }
};

int main() {
    std::cout << "=== Cache Blocking and Memory Optimization Demonstration ===\n";
    
    CacheBlockingDemo demo;
    
    demo.matrixMultiplicationBlocking();
    demo.matrixTransposeBlocking();
    demo.cacheObliviousDemo();
    demo.memoryAccessPatterns();
    
    std::cout << "\n=== Key Takeaways ===\n";
    std::cout << "1. Cache blocking can provide 2-10x speedups\n";
    std::cout << "2. Optimal block size ≈ √(cache_size/3) for matrix multiplication\n";
    std::cout << "3. Sequential access >> strided access >> random access\n";
    std::cout << "4. Cache-oblivious algorithms adapt to any cache hierarchy\n";
    std::cout << "5. Understanding memory patterns is crucial for HPC\n";
    
    return 0;
}