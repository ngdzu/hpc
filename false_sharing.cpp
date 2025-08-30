#include <iostream>
#include <vector>
#include <omp.h>
#include <chrono>

int main()
{
    const int N = 8;
    std::vector<int> arr(N, 0);

    auto start = std::chrono::high_resolution_clock::now();
#pragma omp parallel for
    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < 1000000; ++j)
        {
            arr[i]++;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float, std::milli> duration = end - start;

    for (int i = 0; i < N; ++i)
    {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;
    std::cout << "Duration: " << duration.count() << " ms\n";
}