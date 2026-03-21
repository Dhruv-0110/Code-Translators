#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <iostream>

// ============================================================
// Watcher : Monitors a data source for new events
// ============================================================
struct Watcher {
    std::string type;       // "file", "json", "csv", "html"
    std::string source;     // filepath or URL

    Watcher(const std::string& t, const std::string& s)
        : type(t), source(s) {}

    // Read all lines from a file source
    std::vector<std::string> readLines() {
        std::vector<std::string> lines;
        std::ifstream file(source);
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                lines.push_back(line);
            }
        }
        return lines;
    }
};
