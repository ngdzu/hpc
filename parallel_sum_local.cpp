#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <fstream>

void partial_sum(const std::vector<int>& data, int start, int end, long long& local_sum) {
    local_sum = 0;
    for (int i = start; i < end; ++i)
        local_sum += data[i];
}

int main() {
    const int N = 100000000;
    std::vector<int> data(N, 1);
    int num_threads = 4;
    std::vector<long long> local_sums(num_threads, 0);
    std::vector<std::thread> threads;
    int chunk = N / num_threads;

    auto start = std::chrono::high_resolution_clock::now();
    for (int t = 0; t < num_threads; ++t) {
        int begin = t * chunk;
        int end = (t == num_threads - 1) ? N : begin + chunk;
        threads.emplace_back(partial_sum, std::ref(data), begin, end, std::ref(local_sums[t]));
    }
    for (auto& th : threads) th.join();

    long long sum = 0;
    for (auto s : local_sums) sum += s;
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