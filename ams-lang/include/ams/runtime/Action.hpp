#pragma once
#include <string>
#include <iostream>
#include <cstdlib>

// ============================================================
// Action : Executes automated responses when rules trigger
// ============================================================
namespace ActionEngine {

    inline void sendEmail(const std::string& recipient) {
        std::cout << "[ACTION] SEND_EMAIL to: " << recipient << std::endl;
    }

    inline void alert(const std::string& message) {
        std::cout << "[ACTION] ALERT: " << message << std::endl;
    }

    inline void logToFile(const std::string& filename, const std::string& message) {
        std::cout << "[ACTION] LOG to: " << filename << std::endl;
    }

    inline void callApi(const std::string& endpoint) {
        std::cout << "[ACTION] CALL_API: " << endpoint << std::endl;
    }

    inline void executeScript(const std::string& script) {
        std::cout << "[ACTION] EXECUTE_SCRIPT: " << script << std::endl;
        std::system(script.c_str());
    }

    inline void console(const std::string& text) {
        std::cout << text << std::endl;
    }

}
