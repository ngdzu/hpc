
#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>
#include <cmath>
#include <spdlog/spdlog.h>

#ifdef _OPENMP
#include <omp.h>
#else
// Fallback for when OpenMP is not available
int omp_get_max_threads() { return 1; }
#endif

class SIMDDemo {
private:
    const size_t N = 1000000;
    std::vector<float> a, b, c;
    std::vector<int> int_a, int_b, int_c;
    
public:
    SIMDDemo() : a(N), b(N), c(N), int_a(N), int_b(N), int_c(N) {
        generateRandomFloat();
        generateRandomInt();
    }
    
    void generateRandomFloat() {
        std::mt19937 rng(std::random_device{}());
        std::uniform_real_distribution<float> dist(0.0f, 100.0f);
        
        for (size_t i = 0; i < N; ++i) {
            b[i] = dist(rng);
            c[i] = dist(rng);
        }
    }
    
    void generateRandomInt() {
        std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<int> dist(1, 1000);
        
        for (size_t i = 0; i < N; ++i) {
            int_b[i] = dist(rng);
            int_c[i] = dist(rng);
        }
    }
    
    // 1. Basic scalar version (no vectorization)
    double scalarAddition() {
        auto start = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < N; ++i) {
            a[i] = b[i] + c[i];
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        spdlog::info("Scalar Addition: {} μs", duration.count());
        return duration.count();
    }
    
    // 2. OpenMP SIMD version (compiler auto-vectorization)
    double autoVectorizedAddition() {
        auto start = std::chrono::high_resolution_clock::now();
        
        // Compiler should auto-vectorize this with -O3 -march=native
        #pragma GCC ivdep  // Tell compiler loop has no dependencies
        for (size_t i = 0; i < N; ++i) {
            a[i] = b[i] + c[i];
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        spdlog::info("Auto-Vectorized Addition: {} μs", duration.count());
        return duration.count();
    }
    
    // 3. OpenMP SIMD version
    double ompSIMDAddition() {
        auto start = std::chrono::high_resolution_clock::now();
        
        #ifdef _OPENMP
        #pragma omp simd
        #endif
        for (size_t i = 0; i < N; ++i) {
            a[i] = b[i] + c[i];
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        spdlog::info("OpenMP SIMD Addition: {} μs", duration.count());
        return duration.count();
    }
    
    // 4. Complex operation: Dot product (scalar)
    double scalarDotProduct() {
        auto start = std::chrono::high_resolution_clock::now();
        
        double sum = 0.0;
        for (size_t i = 0; i < N; ++i) {
            sum += b[i] * c[i];
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        spdlog::info("Scalar Dot Product: {} μs, Result: {:.2f}", duration.count(), sum);
        return duration.count();
    }
    
    // 5. SIMD Dot product with reduction
    double simdDotProduct() {
        auto start = std::chrono::high_resolution_clock::now();
        
        double sum = 0.0;
        #ifdef _OPENMP
        #pragma omp simd reduction(+:sum)
        #endif
        for (size_t i = 0; i < N; ++i) {
            sum += b[i] * c[i];
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        spdlog::info("SIMD Dot Product: {} μs, Result: {:.2f}", duration.count(), sum);
        return duration.count();
    }
    
    // 6. Parallel + SIMD combination
    double parallelSIMDAddition() {
        auto start = std::chrono::high_resolution_clock::now();
        
        #ifdef _OPENMP
        #pragma omp parallel for simd
        #endif
        for (size_t i = 0; i < N; ++i) {
            a[i] = b[i] + c[i];
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        spdlog::info("Parallel + SIMD Addition: {} μs", duration.count());
        return duration.count();
    }
    
    // 7. SIMD with different data types (int operations)
    double simdIntegerOperations() {
        auto start = std::chrono::high_resolution_clock::now();
        
        #ifdef _OPENMP
        #pragma omp simd
        #endif
        for (size_t i = 0; i < N; ++i) {
            int_a[i] = int_b[i] * int_c[i] + (int_b[i] >> 2);  // Multiply + bit shift
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        spdlog::info("SIMD Integer Operations: {} μs", duration.count());
        return duration.count();
    }
    
    // 8. Math-intensive SIMD operations
    double mathIntensiveOperations() {
        auto start = std::chrono::high_resolution_clock::now();
        
        #ifdef _OPENMP
        #pragma omp simd
        #endif
        for (size_t i = 0; i < N; ++i) {
            a[i] = std::sqrt(b[i] * b[i] + c[i] * c[i]);  // Vector magnitude
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        spdlog::info("Math-Intensive SIMD: {} μs", duration.count());
        return duration.count();
    }
    
    // 9. Memory bandwidth test
    double memoryBandwidthTest() {
        auto start = std::chrono::high_resolution_clock::now();
        
        // Simple memory copy operation
        #ifdef _OPENMP
        #pragma omp simd
        #endif
        for (size_t i = 0; i < N; ++i) {
            a[i] = b[i];  // Memory bandwidth limited
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        spdlog::info("Memory Bandwidth Test: {} μs", duration.count());
        
        // Calculate bandwidth
        double bytes_transferred = N * sizeof(float) * 2; // Read b, write a
        double bandwidth_gb_s = bytes_transferred / (duration.count() * 1e-6) / 1e9;
        spdlog::info("Memory Bandwidth: {:.2f} GB/s", bandwidth_gb_s);
        
        return duration.count();
    }
    
    // 10. Loop unrolling demonstration
    double unrolledLoopAddition() {
        auto start = std::chrono::high_resolution_clock::now();
        
        size_t unroll_factor = 4;
        size_t unrolled_size = N - (N % unroll_factor);
        
        // Manually unrolled loop
        for (size_t i = 0; i < unrolled_size; i += unroll_factor) {
            a[i] = b[i] + c[i];
            a[i+1] = b[i+1] + c[i+1];
            a[i+2] = b[i+2] + c[i+2];
            a[i+3] = b[i+3] + c[i+3];
        }
        
        // Handle remaining elements
        for (size_t i = unrolled_size; i < N; ++i) {
            a[i] = b[i] + c[i];
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        spdlog::info("Unrolled Loop Addition: {} μs", duration.count());
        return duration.count();
    }
    
    void runAllBenchmarks() {
        spdlog::info("=== SIMD Performance Benchmarks ===");
        spdlog::info("Array size: {} elements", N);
        spdlog::info("Data size: {:.2f} MB", (N * sizeof(float) * 3) / 1e6);
        
        #ifdef _OPENMP
        spdlog::info("OpenMP support: Available");
        spdlog::info("OpenMP threads: {}", omp_get_max_threads());
        #else
        spdlog::info("OpenMP support: Not available (using fallbacks)");
        #endif
        
        #ifdef __ARM_NEON
        spdlog::info("ARM NEON support: Available");
        #endif
        
        #ifdef __AVX2__
        spdlog::info("AVX2 support: Available");
        #elif defined(__AVX__)
        spdlog::info("AVX support: Available");
        #elif defined(__SSE4_2__)
        spdlog::info("SSE4.2 support: Available");
        #else
        spdlog::info("SIMD support: Using compiler auto-vectorization");
        #endif
        
        std::vector<double> results;
        
        results.push_back(scalarAddition());
        results.push_back(autoVectorizedAddition());
        results.push_back(ompSIMDAddition());
        results.push_back(scalarDotProduct());
        results.push_back(simdDotProduct());
        results.push_back(parallelSIMDAddition());
        results.push_back(simdIntegerOperations());
        results.push_back(mathIntensiveOperations());
        results.push_back(memoryBandwidthTest());
        results.push_back(unrolledLoopAddition());
        
        // Calculate speedups
        spdlog::info("\n=== Performance Analysis ===");
        if (results[0] > 0) {
            spdlog::info("Auto-vectorization speedup: {:.2f}x", results[0] / results[1]);
            spdlog::info("OpenMP SIMD speedup: {:.2f}x", results[0] / results[2]);
            spdlog::info("Parallel+SIMD speedup: {:.2f}x", results[0] / results[5]);
            spdlog::info("Unrolled loop speedup: {:.2f}x", results[0] / results[9]);
            if (results[3] > 0) {
                spdlog::info("Dot product SIMD speedup: {:.2f}x", results[3] / results[4]);
            }
        }
        
        // Verify results to prevent dead code elimination
        spdlog::info("\n=== Verification (first 5 elements) ===");
        for (int i = 0; i < 5; ++i) {
            spdlog::info("a[{}] = {:.2f}", i, a[i]);
        }
        
        // Performance insights
        spdlog::info("\n=== Performance Insights ===");
        spdlog::info("• Scalar version provides baseline performance");
        spdlog::info("• Auto-vectorization relies on compiler optimization (-O3 -march=native)");
        spdlog::info("• OpenMP SIMD gives explicit vectorization hints");
        spdlog::info("• Parallel+SIMD combines multiple threads with vectorization");
        spdlog::info("• Memory bandwidth often limits performance more than computation");
        spdlog::info("• Integer operations typically vectorize better than floating-point");
    }
};

int main() {
    try {
        spdlog::info("Starting SIMD/Vectorization Demo");
        
        SIMDDemo demo;
        demo.runAllBenchmarks();
        
        spdlog::info("\nSIMD Demo completed successfully!");
        
        // Educational summary
        spdlog::info("\n=== SIMD Learning Summary ===");
        spdlog::info("1. SIMD (Single Instruction, Multiple Data) processes multiple data elements simultaneously");
        spdlog::info("2. Modern compilers can auto-vectorize simple loops with -O3 optimization");
        spdlog::info("3. OpenMP SIMD pragmas provide explicit vectorization hints");
        spdlog::info("4. Memory bandwidth often becomes the bottleneck, not computation");
        spdlog::info("5. Different data types (int vs float) have different vectorization characteristics");
        spdlog::info("6. Loop unrolling can improve performance by reducing branch overhead");
        spdlog::info("7. Combining parallelization (#pragma omp parallel) with vectorization maximizes throughput");
        
    } catch (const std::exception& ex) {
        spdlog::error("Error: {}", ex.what());
        return 1;
    }
    
    return 0;
}
