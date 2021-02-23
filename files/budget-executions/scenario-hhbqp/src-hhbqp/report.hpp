#pragma once
#include <iostream>

struct Report {
    double bestSoFar;

    Report() {
        bestSoFar = std::numeric_limits<int>::max();
    }

    void newBestKnownValue(std::chrono::system_clock::time_point timePoint, double value) {
        if(value < bestSoFar) {
            bestSoFar = value;
            std::cout << bestSoFar << std::endl;
        }
    }
};