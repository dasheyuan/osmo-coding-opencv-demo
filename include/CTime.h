//
// Created by chenyuan on 19-3-26.
//

#ifndef CTIME_H
#define CTIME_H

#include <chrono>

class CTime {
public:
    CTime();

    ~CTime();

    void start();

    long now();

    int restart();

private:
    std::chrono::high_resolution_clock::time_point start_;
    std::chrono::high_resolution_clock::time_point end_;
};

#endif