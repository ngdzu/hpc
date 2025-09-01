#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <fstream>


std::mutex mtx;
// std::atomic<long long> sum;
long long sum = 0;;

void partial_sum(const std::vector<int> &data, int start, int end)
{
    long long local_sum = 0;
    for (int i = start; i < end; ++i)
        local_sum += data[i];
    std::lock_guard<std::mutex> lock(mtx);
    sum += local_sum;
}

int main()
{
    const int N = 100000000;
    std::vector<int> data(N, 1);
    int num_threads = 4;
    sum = 0;
    std::vector<std::thread> threads;
    int chunk = N / num_threads;

    auto start = std::chrono::high_resolution_clock::now();
    for (int t = 0; t < num_threads; ++t)
    {
        int start = t * chunk;
        int end = (t == num_threads - 1) ? N : start + chunk;
        threads.emplace_back(partial_sum, std::ref(data), start, end);
    }
    for (auto &th : threads)
        th.join();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;

    std::cout << "Sum: " << sum << std::endl;
    std::cout << "duration: " << duration.count() << " milli secs\n";

    std::ofstream log("log.txt", std::ios::app);
#ifdef PROJECT_NAME
    log << "Project name: " << PROJECT_NAME << std::endl;
#else
    log << "Project name: (undefined)" << std::endl;
#endif
    log << "Sum: " << sum << std::endl;
    log << "Duration: " << duration.count() << " milli secs" << "\n\n";
}