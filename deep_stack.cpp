#include <iostream>
#include <chrono>

// Deep stack functions
int leaf(int x) {
    // Simulate work
    int sum = 0;
    for (int i = 0; i < 1000; ++i)
        sum += (x * i) % 7;
    return sum;
}

int level3(int x) {
    int result = 0;
    for (int i = 0; i < 10; ++i)
        result += leaf(x + i);
    return result;
}

int level2(int x) {
    int result = 0;
    for (int i = 0; i < 10; ++i)
        result += level3(x + i);
    return result;
}

int level1(int x) {
    int result = 0;
    for (int i = 0; i < 10; ++i)
        result += level2(x + i);
    return result;
}

int main() {
    const int N = 10000;
    long long total = 0;

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < N; ++i) {
        total += level1(i);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "Total: " << total << "\n";
    std::cout << "Elapsed time: " << duration.count() << " seconds\n";
    return 0;
}