#include <iostream>
#include <vector>
#include <chrono>

int main() {
    const int N = 10000;
    std::vector<std::vector<int>> matrix(N, std::vector<int>(N, 1));
    long long sum = 0;

    auto start = std::chrono::high_resolution_clock::now();
    // Row-major order (cache-friendly)
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            sum += matrix[i][j];
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> duration = end - start;
    std::cout << "Sum: " << sum << ", Time: " << duration.count() << " s" << std::endl;
    return 0;
}