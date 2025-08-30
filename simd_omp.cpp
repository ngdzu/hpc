

#include <iostream>
#include <omp.h>
#include <chrono>

#include <random>

void generateRandom(int *arr, size_t size)
{
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, 99);
    for (size_t i = 0; i < size; ++i)
    {
        arr[i] = dist(rng);
    }
}

int main()
{
    const int N = 1000;
    int a[N] = {};
    int b[N] = {};
    int c[N] = {};

    generateRandom(a, N);
    generateRandom(b, N);
    generateRandom(c, N);

    auto start = std::chrono::high_resolution_clock::now();
    // #pragma omp simd
    for (int i = 0; i < N; ++i)
    {
        a[i] = b[i] + c[i];
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::nano> duration = end - start;
    std::cout << "duration: " << duration.count() << " ns" << "\n";

    for (int i = 0; i < 10; ++i)
    {
        a[i] = b[i] + c[i];
        std::cout << a[i] << "\n";
    }
}
