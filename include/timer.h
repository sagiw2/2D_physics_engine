#pragma once

#include <chrono>
#include <iostream>

#define PROFILE_SCOPE(name) ScopedTimer timer##__LINE__(name)

class ScopedTimer {
public:
    ScopedTimer(const std::string& name)
        : name(name), start(std::chrono::high_resolution_clock::now()) {}

    ~ScopedTimer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << name << " took " << duration.count() << " µs\n";
    }

private:
    std::string name;
    std::chrono::high_resolution_clock::time_point start;
};
