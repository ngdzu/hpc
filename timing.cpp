#include <iostream>
#include <chrono>

int main()
{
    long long sum = 0;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 1; i <= 100; ++i)
    {
        sum += i;
    }
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration<double, std::milli>(end - start);

    std::cout << "Sum: " << sum << std::endl;
    std::cout << "Elapsed time: " << duration.count() << " ms" << std::endl;

    return 0;
}