#include <iostream>
#include <vector>
#include <chrono>

int main()
{
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

    const int jmax = 20; // rows
    const int imax = 30; // columns
    double **x = (double **)malloc(jmax * sizeof(double *));
    x[0] = (double *)malloc(jmax * imax * sizeof(double));
    for (int j = 1; j < jmax ; ++j)
    {
        x[j]= x[j-1] + imax;
    }

    return 0;
}