#pragma once
#include <functional>
#include <vector>
#include <string>
#include <iostream>
#include <thread>
#include <chrono>

// ============================================================
// EventLoop : Monitoring cycle controller
// ============================================================
class EventLoop {
public:
    using Task = std::function<void()>;

    void addTask(Task task) {
        tasks.push_back(task);
    }

    // Run all tasks once (single-pass mode for compiled output)
    void runOnce() {
        std::cout << "[EventLoop] Running single monitoring pass..." << std::endl;
        for (auto& task : tasks) {
            task();
        }
        std::cout << "[EventLoop] Pass complete." << std::endl;
    }

    // Run tasks in a loop with a delay (continuous monitoring mode)
    void runLoop(int intervalSeconds, int maxIterations = 1) {
        for (int i = 0; i < maxIterations; i++) {
            std::cout << "[EventLoop] Cycle " << (i + 1) << "/" << maxIterations << std::endl;
            for (auto& task : tasks) {
                task();
            }
            if (i < maxIterations - 1) {
                std::this_thread::sleep_for(std::chrono::seconds(intervalSeconds));
            }
        }
    }

private:
    std::vector<Task> tasks;
};
