// Automatically generated C++ header for simulating b3s23
#pragma once
#include <algorithm>
#include <cstdint>
#include <unordered_map>
#include <utility>
#include <vector>
#include "params.hpp"

inline int64_t tokey(const int32_t x, const int32_t y) {
    // Converts an x and y coordinate to a 64-bit key.
    int64_t key;
    key = 2147483648 * (x + 1073741824) + y + 1073741824;
    return key;
}
inline int32_t getx(const int64_t key) {
    // Extracts the x coordinate from a 64-bit key.
    int32_t x;
    x = key >> 31;
    x -= 1073741824;
    return x;
}
inline int32_t gety(const int64_t key) {
    // Extracts the y coordinate from a 64-bit key.
    int32_t y;
    y = key % 2147483648;
    y -= 1073741824;
    return y;
}
// Saves time calculating exponents later:
const int16_t neighbournum[9] = {1, 2, 4, 8, 16, 32, 64, 128, 256};
#define MAXINC 9
void advanceone(std::vector<std::pair<int32_t, int32_t> >& lifevector) {
    // Advances a std::vector of coordinates in place by one generation.
    uint32_t i;
    int64_t key;
    //An unordered map is the best container for the job here.
    umap<int64_t, uint16_t> neighbours = {};
    neighbours.reserve(MAXINC * lifevector.size());
    int32_t x, y;
    uint8_t dx, dy;
    std::pair<int32_t, int32_t> cpair;
    // Calculating neighbours:
    for (i = 0; i < (lifevector.size()); i++) {
        auto [x, y] = lifevector[i];
        for (dx = 0; dx < 3; dx++) {
            for (dy = 0; dy < 3; dy++) {
                neighbours[tokey(x + dx - 1, y + dy - 1)] += neighbournum[3*dy + dx];
            }
        }
    }
    lifevector.clear();
    lifevector.reserve(neighbours.size());
    for (const auto& kv : neighbours) {
        if (conditionset[kv.second]) {
            key = kv.first;
            lifevector.push_back(std::make_pair(getx(key), gety(key)));
        }
    }
}
void cppadvance(std::vector<std::pair<int32_t, int32_t> >& lifevector, const int32_t generations) {
    // Internal function used for advancing patterns (for other C++ functions).
    uint32_t i;
    for (i = 0; i < generations; i++) {
        advanceone(lifevector);
    }
}
