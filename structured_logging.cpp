#include <iostream>
#include <chrono>
#include <random>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class StructuredLogger {
private:
    std::shared_ptr<spdlog::logger> logger_;
    
public:
    StructuredLogger(const std::string& name, const std::string& filename) {
        logger_ = spdlog::basic_logger_mt(name, filename);
        logger_->set_pattern("%v"); // Only output the message (pure JSON)
    }
    
    void log(spdlog::level::level_enum level, const std::string& message, 
             const std::string& component = "", const json& fields = json::object()) {
        
        json log_entry = {
            {"timestamp", std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count()},
            {"level", spdlog::level::to_string_view(level)},
            {"message", message},
            {"component", component}
        };
        
        // Merge additional fields
        if (!fields.empty()) {
            log_entry.update(fields);
        }
        
        logger_->log(level, log_entry.dump());
    }
    
    void info(const std::string& message, const std::string& component = "", const json& fields = json::object()) {
        log(spdlog::level::info, message, component, fields);
    }
    
    void warn(const std::string& message, const std::string& component = "", const json& fields = json::object()) {
        log(spdlog::level::warn, message, component, fields);
    }
    
    void error(const std::string& message, const std::string& component = "", const json& fields = json::object()) {
        log(spdlog::level::err, message, component, fields);
    }
    
    void debug(const std::string& message, const std::string& component = "", const json& fields = json::object()) {
        log(spdlog::level::debug, message, component, fields);
    }
};

int main()
{
    try {
        // Create structured logger
        StructuredLogger struct_log("structured", "logs/structured.log");
        
        // 1. Basic structured log
        struct_log.info("Application started", "main");
        
        // 2. Log with structured data
        json user_context = {
            {"user_id", 12345},
            {"session_id", "sess_abc123"},
            {"ip_address", "192.168.1.100"},
            {"user_agent", "Mozilla/5.0 Chrome/91.0"}
        };
        struct_log.info("User authentication successful", "auth", user_context);
        
        // 3. Performance monitoring with structured data
        auto start = std::chrono::high_resolution_clock::now();
        
        std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<int> dist(1, 100);
        const int N = 100000;
        
        spdlog::info("Starting performance test with {} iterations", N);
        
        long long sum = 0;
        for (int i = 1; i <= N; ++i) {
            sum += dist(rng);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        // Log performance metrics
        json perf_metrics = {
            {"operation", "random_sum_calculation"},
            {"iterations", N},
            {"result", sum},
            {"duration_microseconds", duration.count()},
            {"throughput_ops_per_sec", static_cast<double>(N) / (duration.count() / 1000000.0)},
            {"avg_time_per_op_ns", duration.count() * 1000.0 / N}
        };
        struct_log.info("Performance test completed", "performance", perf_metrics);
        
        // 4. Business event logging
        json business_event = {
            {"event_type", "order_processed"},
            {"order_id", "ORD-2025-001234"},
            {"customer_id", 98765},
            {"amount", 199.99},
            {"currency", "USD"},
            {"payment_method", "credit_card"},
            {"processing_time_ms", duration.count() / 1000}
        };
        struct_log.info("Order processed successfully", "business", business_event);
        
        // 5. Error logging with context
        if (sum > 5000000) {  // Simulate an error condition
            json error_context = {
                {"error_code", "THRESHOLD_EXCEEDED"},
                {"threshold", 5000000},
                {"actual_value", sum},
                {"severity", "high"},
                {"requires_investigation", true}
            };
            struct_log.error("Sum exceeded safety threshold", "validation", error_context);
        }
        
        // 6. Request/Response logging
        json api_log = {
            {"request_id", "req_" + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count())},
            {"method", "POST"},
            {"endpoint", "/api/v1/calculate"},
            {"status_code", 200},
            {"response_time_ms", duration.count() / 1000},
            {"payload_size_bytes", 1024}
        };
        struct_log.info("API request processed", "api", api_log);
        
        // 7. System metrics
        json system_metrics = {
            {"cpu_usage_percent", 23.5},
            {"memory_usage_mb", 512},
            {"disk_free_gb", 45.2},
            {"network_bytes_sent", 1048576},
            {"network_bytes_received", 2097152}
        };
        struct_log.info("System metrics collected", "monitoring", system_metrics);
        
        // Regular console output for human readability
        spdlog::info("=== Structured Logging Demo Complete ===");
        spdlog::info("Processed {} iterations, Sum: {}, Duration: {} μs", N, sum, duration.count());
        spdlog::info("Check logs/proper_structured.log for JSON structured logs");
        
        struct_log.info("Application shutdown", "main");
        
    } catch (const std::exception& ex) {
        spdlog::error("Application error: {}", ex.what());
        return 1;
    }
    
    return 0;
}
