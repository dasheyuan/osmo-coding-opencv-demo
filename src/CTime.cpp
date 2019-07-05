//
// Created by chenyuan on 19-3-26.
//

#include "CTime.h"

CTime::CTime() {}

CTime::~CTime() {}

void CTime::start() {
    start_ = std::chrono::high_resolution_clock::now();
}

long CTime::now() {
    std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp = std::chrono::time_point_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now());
    auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
    std::time_t timestamp = tmp.count();
    //std::time_t timestamp = std::chrono::system_clock::to_time_t(tp);
    return timestamp;
}

int CTime::restart() {
    end_ = std::chrono::high_resolution_clock::now();
    auto cost = (int) std::chrono::duration_cast<std::chrono::milliseconds>(
            end_ - start_).count();
    start_ = std::chrono::high_resolution_clock::now();
    return cost;
}