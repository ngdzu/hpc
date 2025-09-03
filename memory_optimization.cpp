#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>
#include <memory>
#include <cstring>
#include <spdlog/spdlog.h>

#ifdef _OPENMP
#include <omp.h>
#endif

class MemoryOptimizationDemo {
private:
    const size_t MATRIX_SIZE = 1024;
    const size_t ARRAY_SIZE = 16 * 1024 * 1024;  // 16MB of integers
    const size_t CACHE_LINE_SIZE = 64;  // Typical cache line size in bytes
    
public:
    // 1. Cache-Friendly vs Cache-Unfriendly Matrix Multiplication
    void matrixMultiplicationComparison() {
        spdlog::info("=== Matrix Multiplication: Cache Optimization ===");
        
        // Allocate matrices
        auto A = std::make_unique<float[]>(MATRIX_SIZE * MATRIX_SIZE);
        auto B = std::make_unique<float[]>(MATRIX_SIZE * MATRIX_SIZE);
        auto C1 = std::make_unique<float[]>(MATRIX_SIZE * MATRIX_SIZE);
        auto C2 = std::make_unique<float[]>(MATRIX_SIZE * MATRIX_SIZE);
        
        // Initialize with random data
        std::mt19937 rng(42);
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        
        for (size_t i = 0; i < MATRIX_SIZE * MATRIX_SIZE; ++i) {
            A[i] = dist(rng);
            B[i] = dist(rng);
        }
        
        // Naive implementation (poor cache locality)
        auto start = std::chrono::high_resolution_clock::now();
        naiveMatrixMultiply(A.get(), B.get(), C1.get(), MATRIX_SIZE);
        auto end = std::chrono::high_resolution_clock::now();
        auto naive_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        // Cache-optimized implementation
        start = std::chrono::high_resolution_clock::now();
        cacheOptimizedMatrixMultiply(A.get(), B.get(), C2.get(), MATRIX_SIZE);
        end = std::chrono::high_resolution_clock::now();
        auto optimized_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        spdlog::info("Naive matrix multiply: {} ms", naive_time.count());
        spdlog::info("Cache-optimized matrix multiply: {} ms", optimized_time.count());
        spdlog::info("Speedup: {:.2f}x", static_cast<double>(naive_time.count()) / optimized_time.count());
        
        // Verify results are similar
        float max_diff = 0.0f;
        for (size_t i = 0; i < MATRIX_SIZE * MATRIX_SIZE; ++i) {
            max_diff = std::max(max_diff, std::abs(C1[i] - C2[i]));
        }
        spdlog::info("Max difference between results: {:.6f}", max_diff);
    }
    
    // 2. Memory Access Patterns
    void memoryAccessPatterns() {
        spdlog::info("\n=== Memory Access Patterns ===");
        
        auto data = std::make_unique<int[]>(ARRAY_SIZE);
        
        // Initialize data
        for (size_t i = 0; i < ARRAY_SIZE; ++i) {
            data[i] = static_cast<int>(i);
        }
        
        // Sequential access (cache-friendly)
        auto start = std::chrono::high_resolution_clock::now();
        long long sum1 = sequentialSum(data.get(), ARRAY_SIZE);
        auto end = std::chrono::high_resolution_clock::now();
        auto seq_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        // Random access (cache-unfriendly)
        start = std::chrono::high_resolution_clock::now();
        long long sum2 = randomSum(data.get(), ARRAY_SIZE);
        end = std::chrono::high_resolution_clock::now();
        auto random_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        // Strided access (varying cache behavior)
        start = std::chrono::high_resolution_clock::now();
        long long sum3 = stridedSum(data.get(), ARRAY_SIZE, 16);
        end = std::chrono::high_resolution_clock::now();
        auto strided_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        spdlog::info("Sequential access: {} μs (sum: {})", seq_time.count(), sum1);
        spdlog::info("Random access: {} μs (sum: {})", random_time.count(), sum2);
        spdlog::info("Strided access (stride=16): {} μs (sum: {})", strided_time.count(), sum3);
        spdlog::info("Random vs Sequential slowdown: {:.2f}x", 
                    static_cast<double>(random_time.count()) / seq_time.count());
    }
    
    // 3. Data Structure of Arrays vs Array of Structures
    void soaVsAosComparison() {
        spdlog::info("\n=== SoA vs AoS Comparison ===");
        
        const size_t N = 1000000;
        
        // Array of Structures (AoS) - traditional approach
        struct Particle_AoS {
            float x, y, z;
            float vx, vy, vz;
            float mass;
        };
        
        auto particles_aos = std::make_unique<Particle_AoS[]>(N);
        
        // Structure of Arrays (SoA) - cache-friendly approach
        struct Particles_SoA {
            std::unique_ptr<float[]> x, y, z;
            std::unique_ptr<float[]> vx, vy, vz;
            std::unique_ptr<float[]> mass;
            
            Particles_SoA(size_t n) :
                x(std::make_unique<float[]>(n)),
                y(std::make_unique<float[]>(n)),
                z(std::make_unique<float[]>(n)),
                vx(std::make_unique<float[]>(n)),
                vy(std::make_unique<float[]>(n)),
                vz(std::make_unique<float[]>(n)),
                mass(std::make_unique<float[]>(n)) {}
        } particles_soa(N);
        
        // Initialize data
        std::mt19937 rng(42);
        std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
        
        for (size_t i = 0; i < N; ++i) {
            particles_aos[i] = {dist(rng), dist(rng), dist(rng), 
                               dist(rng), dist(rng), dist(rng), 1.0f};
            
            particles_soa.x[i] = particles_aos[i].x;
            particles_soa.y[i] = particles_aos[i].y;
            particles_soa.z[i] = particles_aos[i].z;
            particles_soa.vx[i] = particles_aos[i].vx;
            particles_soa.vy[i] = particles_aos[i].vy;
            particles_soa.vz[i] = particles_aos[i].vz;
            particles_soa.mass[i] = particles_aos[i].mass;
        }
        
        const float dt = 0.01f;
        
        // AoS update (poor cache locality for partial updates)
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < N; ++i) {
            particles_aos[i].x += particles_aos[i].vx * dt;
            particles_aos[i].y += particles_aos[i].vy * dt;
            particles_aos[i].z += particles_aos[i].vz * dt;
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto aos_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        // SoA update (excellent cache locality)
        start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < N; ++i) {
            particles_soa.x[i] += particles_soa.vx[i] * dt;
            particles_soa.y[i] += particles_soa.vy[i] * dt;
            particles_soa.z[i] += particles_soa.vz[i] * dt;
        }
        end = std::chrono::high_resolution_clock::now();
        auto soa_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        spdlog::info("AoS position update: {} μs", aos_time.count());
        spdlog::info("SoA position update: {} μs", soa_time.count());
        spdlog::info("SoA speedup: {:.2f}x", static_cast<double>(aos_time.count()) / soa_time.count());
    }
    
    // 4. Memory Alignment and Padding
    void memoryAlignmentDemo() {
        spdlog::info("\n=== Memory Alignment and Padding ===");
        
        // Unaligned structure
        struct UnalignedStruct {
            char a;
            int b;
            char c;
            double d;
        };
        
        // Aligned structure
        struct alignas(64) AlignedStruct {
            char a;
            char padding1[3];
            int b;
            char c;
            char padding2[7];
            double d;
        };
        
        spdlog::info("Unaligned struct size: {} bytes", sizeof(UnalignedStruct));
        spdlog::info("Aligned struct size: {} bytes", sizeof(AlignedStruct));
        spdlog::info("Cache line size: {} bytes", CACHE_LINE_SIZE);
        
        const size_t N = 1000000;
        auto unaligned_array = std::make_unique<UnalignedStruct[]>(N);
        auto aligned_array = std::make_unique<AlignedStruct[]>(N);
        
        // Initialize
        for (size_t i = 0; i < N; ++i) {
            unaligned_array[i] = {'x', static_cast<int>(i), 'y', static_cast<double>(i)};
            aligned_array[i] = {'x', {}, static_cast<int>(i), 'y', {}, static_cast<double>(i)};
        }
        
        // Access patterns
        auto start = std::chrono::high_resolution_clock::now();
        double sum1 = 0.0;
        for (size_t i = 0; i < N; ++i) {
            sum1 += unaligned_array[i].d;
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto unaligned_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        start = std::chrono::high_resolution_clock::now();
        double sum2 = 0.0;
        for (size_t i = 0; i < N; ++i) {
            sum2 += aligned_array[i].d;
        }
        end = std::chrono::high_resolution_clock::now();
        auto aligned_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        spdlog::info("Unaligned access time: {} μs (sum: {:.2f})", unaligned_time.count(), sum1);
        spdlog::info("Aligned access time: {} μs (sum: {:.2f})", aligned_time.count(), sum2);
    }
    
    // 5. Cache Blocking (Tiling) Demonstration
    void cacheBlockingDemo() {
        spdlog::info("\n=== Cache Blocking (Tiling) ===");
        
        const size_t N = 2048;
        auto A = std::make_unique<float[]>(N * N);
        auto B = std::make_unique<float[]>(N * N);
        
        // Initialize
        for (size_t i = 0; i < N * N; ++i) {
            A[i] = 1.0f;
            B[i] = 0.0f;
        }
        
        // Matrix transpose without blocking
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < N; ++j) {
                B[j * N + i] = A[i * N + j];
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto naive_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        // Reset B
        std::fill_n(B.get(), N * N, 0.0f);
        
        // Matrix transpose with cache blocking
        const size_t BLOCK_SIZE = 64;
        start = std::chrono::high_resolution_clock::now();
        for (size_t ii = 0; ii < N; ii += BLOCK_SIZE) {
            for (size_t jj = 0; jj < N; jj += BLOCK_SIZE) {
                for (size_t i = ii; i < std::min(ii + BLOCK_SIZE, N); ++i) {
                    for (size_t j = jj; j < std::min(jj + BLOCK_SIZE, N); ++j) {
                        B[j * N + i] = A[i * N + j];
                    }
                }
            }
        }
        end = std::chrono::high_resolution_clock::now();
        auto blocked_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        spdlog::info("Matrix transpose ({}x{}):", N, N);
        spdlog::info("  Naive approach: {} ms", naive_time.count());
        spdlog::info("  Cache-blocked approach: {} ms", blocked_time.count());
        spdlog::info("  Speedup: {:.2f}x", static_cast<double>(naive_time.count()) / blocked_time.count());
    }
    
    void runAllDemos() {
        spdlog::info("=== Memory Optimization and Cache Performance ===");
        spdlog::info("Demonstrating the impact of memory access patterns on performance\n");
        
        matrixMultiplicationComparison();
        memoryAccessPatterns();
        soaVsAosComparison();
        memoryAlignmentDemo();
        cacheBlockingDemo();
        
        spdlog::info("\n=== Memory Optimization Summary ===");
        spdlog::info("• Cache locality is crucial for performance");
        spdlog::info("• Sequential access patterns are fastest");
        spdlog::info("• SoA (Structure of Arrays) often outperforms AoS for bulk operations");
        spdlog::info("• Memory alignment can improve performance");
        spdlog::info("• Cache blocking/tiling helps with large data sets");
        spdlog::info("• Understanding your hardware's cache hierarchy is essential");
    }

private:
    // Naive matrix multiplication (i-j-k order)
    void naiveMatrixMultiply(const float* A, const float* B, float* C, size_t n) {
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                float sum = 0.0f;
                for (size_t k = 0; k < n; ++k) {
                    sum += A[i * n + k] * B[k * n + j];
                }
                C[i * n + j] = sum;
            }
        }
    }
    
    // Cache-optimized matrix multiplication (i-k-j order)
    void cacheOptimizedMatrixMultiply(const float* A, const float* B, float* C, size_t n) {
        // Initialize C to zero
        std::fill_n(C, n * n, 0.0f);
        
        for (size_t i = 0; i < n; ++i) {
            for (size_t k = 0; k < n; ++k) {
                float a_ik = A[i * n + k];
                for (size_t j = 0; j < n; ++j) {
                    C[i * n + j] += a_ik * B[k * n + j];
                }
            }
        }
    }
    
    long long sequentialSum(const int* data, size_t size) {
        long long sum = 0;
        for (size_t i = 0; i < size; ++i) {
            sum += data[i];
        }
        return sum;
    }
    
    long long randomSum(const int* data, size_t size) {
        std::vector<size_t> indices(size);
        std::iota(indices.begin(), indices.end(), 0);
        
        std::mt19937 rng(42);
        std::shuffle(indices.begin(), indices.end(), rng);
        
        long long sum = 0;
        for (size_t i = 0; i < size; ++i) {
            sum += data[indices[i]];
        }
        return sum;
    }
    
    long long stridedSum(const int* data, size_t size, size_t stride) {
        long long sum = 0;
        for (size_t i = 0; i < size; i += stride) {
            sum += data[i];
        }
        return sum;
    }
};

int main() {
    try {
        spdlog::info("Starting Memory Optimization Demo");
        
        MemoryOptimizationDemo demo;
        demo.runAllDemos();
        
        spdlog::info("\nMemory Optimization Demo completed successfully!");
        
    } catch (const std::exception& ex) {
        spdlog::error("Error: {}", ex.what());
        return 1;
    }
    
    return 0;
}
