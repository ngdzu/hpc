

#include <iostream>
#include <omp.h>
#include <chrono>

#include <random>

void generateRandom(char *arr, size_t size)
{
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, 2);
    for (size_t i = 0; i < size; ++i)
    {
        arr[i] = dist(rng);
    }
}

int main()
{
    const long N = 1000000;
    char a[N] = {};
    char b[N] = {};
    char c[N] = {};

    generateRandom(b, N);
    generateRandom(c, N);

    auto start = std::chrono::high_resolution_clock::now();
    #pragma omp simd
    for (int i = 0; i < N; ++i)
    {
        a[i] = b[i] + c[i];
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::micro> duration = end - start;
    std::cout << "duration: " << duration.count() << " micro s" << "\n";

    // This will remove optimization which the compiler knows a[i]
    // is never used so it is never calculated.
    for (int i = 0; i < 10; ++i)
    {
        std::cout << a[i] << "\n";
    }
}
