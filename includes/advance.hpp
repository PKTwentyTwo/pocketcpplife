// Header responsible for advancing patterns.
#pragma once
#include <algorithm>
#include <cstdint>
#include <utility>
#include <vector>
#include "params.hpp"
#include "hensel.hpp"
inline int64_t tokey(const int32_t x, const int32_t y) {
    // Converts an x and y coordinate to a 64-bit key.
    return ((int64_t)x << 32) | (y & 0xFFFFFFFF);
}
inline int32_t getx(const int64_t key) {
    // Extracts the x coordinate from a 64-bit key.
    return key >> 32;
}
inline int32_t gety(const int64_t key) {
    // Extracts the y coordinate from a 64-bit key.
    return ((key << 32) >> 32);
}
#define MAXINC 9
void advanceone(std::vector<std::pair<int32_t, int32_t> >& lifevector, const bool* conditionset) {
    // Advances a std::vector of coordinates in place by one generation.
    uint32_t i;
    int64_t key;
    //An unordered map is the best container for the job here.
    umap<int64_t, uint16_t> neighbours = {};
    neighbours.reserve(MAXINC * lifevector.size());
    int32_t x, y;
    uint8_t dx, dy;
    // Calculating neighbours:
    const size_t vsize = lifevector.size();
    for (i = 0; i < (vsize); i++) {
        auto [x, y] = lifevector[i];
        for (dx = 0; dx < 3; dx++) {
            for (dy = 0; dy < 3; dy++) {
                neighbours[tokey(x + dx - 1, y + dy - 1)] += 1 << (3*dy + dx);
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
void cppadvance(std::vector<std::pair<int32_t, int32_t> >& lifevector, const uint32_t generations, const std::string rule) {
    // Internal function used for advancing patterns (for other C++ functions).
    size_t i;
    const bool* conditions = getmap(rule);
    for (i = 0; i < generations; i++) {
        advanceone(lifevector, conditions);
    }
}
