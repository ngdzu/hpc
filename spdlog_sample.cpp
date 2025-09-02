#include <iostream>
#include <omp.h>
#include <chrono>
#include <random>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>

int main()
{
    // 1. Basic logging to console
    spdlog::info("Starting the application");
    spdlog::warn("This is a warning message");
    spdlog::error("This is an error message");
    
    // 2. Set global log level
    spdlog::set_level(spdlog::level::debug);
    spdlog::debug("This debug message will now be visible");
    
    // 3. Format messages with variables
    int age = 25;
    std::string name = "Alice";
    spdlog::info("User {} is {} years old", name, age);
    
    // 4. Create a file logger
    try {
        auto file_logger = spdlog::basic_logger_mt("file_logger", "logs/app.log");
        file_logger->info("This message goes to the file");
        file_logger->warn("File logging is working!");
        
        // 5. Create a rotating file logger (auto-creates new files when size limit is reached)
        auto rotating_logger = spdlog::rotating_logger_mt("rotating_logger", "logs/rotating.log", 1048576 * 5, 3);
        rotating_logger->info("This goes to a rotating log file");
        
        // 6. Create a console logger with color
        auto console_logger = spdlog::stdout_color_mt("console");
        console_logger->info("Colorful console output!");
        console_logger->error("Red error message");
        
        // 7. Performance timing example with spdlog
        std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<int> dist(1, 100);
        const int N = 10000;
        
        auto start = std::chrono::high_resolution_clock::now();
        spdlog::info("Starting calculation with N = {}", N);
        
        long long sum = 0;
        for (int i = 1; i <= N; ++i) {
            sum += dist(rng);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        spdlog::info("Calculation completed:");
        spdlog::info("  Sum: {}", sum);
        spdlog::info("  Duration: {} microseconds", duration.count());
        
        // 8. Conditional logging
        if (sum > 500000) {
            spdlog::warn("Sum is quite large: {}", sum);
        } else {
            spdlog::info("Sum is reasonable: {}", sum);
        }
        
        // 9. Log with custom pattern
        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
        spdlog::info("Custom formatted message");
        
    } catch (const spdlog::spdlog_ex& ex) {
        std::cout << "Log initialization failed: " << ex.what() << std::endl;
    }
    
    spdlog::info("Application finished successfully");
    return 0;
}