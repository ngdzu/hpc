#include <iostream>
#include <vector>
#include <omp.h>
#include <chrono>

int main()
{
    const int N = 8;

    typedef struct alignas(64) 
    {
        int value;
    } PaddedInt;
    std::vector<PaddedInt>
        arr(N, {0});

    auto start = std::chrono::high_resolution_clock::now();
#pragma omp parallel for
    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < 1000000; ++j)
        {
            arr[i].value++;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float, std::milli> duration = end - start;

    for (int i = 0; i < N; ++i)
    {
        std::cout << arr[i].value << " ";
    }
    std::cout << std::endl;
    std::cout << "Duration: " << duration.count() << " ms\n";
}