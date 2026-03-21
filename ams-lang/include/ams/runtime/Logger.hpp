#pragma once
#include <string>
#include <iostream>
#include <chrono>
#include <ctime>

// ============================================================
// Logger : Formatted logging utility for the AMS Engine
// ============================================================
namespace Logger {

    inline std::string timestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::string ts = std::ctime(&time);
        if (!ts.empty() && ts.back() == '\n') ts.pop_back();
        return ts;
    }

    inline void info(const std::string& msg) {
        std::cout << "[" << timestamp() << "] [INFO] " << msg << std::endl;
    }

    inline void warn(const std::string& msg) {
        std::cout << "[" << timestamp() << "] [WARN] " << msg << std::endl;
    }

    inline void error(const std::string& msg) {
        std::cerr << "[" << timestamp() << "] [ERROR] " << msg << std::endl;
    }

}
