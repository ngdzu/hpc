#include <fstream>
#include <iostream>
#include <vector>
#include <omp.h>
#include <chrono>
#include <random>
#include <spdlog/spdlog.h>

int main()
{

    // std::mt19937 rng(std::random_device{}());
    // std::uniform_int_distribution<int> dist(1, 3);
    // int random_number = dist(rng);
    // const int N = 10000;

    // // std::vector<int> a(N, 0);
    // long long sum = 0;
    // auto start = std::chrono::high_resolution_clock::now();
    // // #pragma omp parallel for reduction(+ : sum)
    // for (int i = 1; i <= N; ++i)
    // {
    //     sum += random_number;
    // }

    // auto end = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<float, std::micro> duration = end - start;
    // std::cout << "Sum: " << sum << "\n";
    // std::cout << "duration: " << duration.count() << " micro secs\n";

    std::ofstream log("mylog.txt", std::ios::app);
    log << "hello\n" ;
    std::cout << "hello\n";

    return 0;
}