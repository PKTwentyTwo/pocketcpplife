// Header storing the main Pattern class.
#pragma once
#include <cstdint>
#include <string>
#include <utility>
#include <vector>
#include "advance.hpp"
#include "gridops.hpp"
class Pattern {
    ptvec lifevector;
    public:
    Pattern(ptvec ptvector) {
        lifevector = ptvector;
    }
    Pattern(const std::string rle) {
        if (isapgcode(rle)) {
            lifevector = apgcodetogrid(rle);
        }
        else {
            lifevector = rle_to_vector(rle);
        }
    }
    Pattern(Pattern& pt) {
        lifevector = pt.coords();
    }
    Pattern advance(int32_t generations) {
        ptvec lifevector2(lifevector);
        cppadvance(lifevector2, generations);
        Pattern* newptr = new Pattern(lifevector2);
        return *newptr;
    }
    Pattern operator[](int32_t numgens) {
        return advance(numgens);
    }
    ptvec coords() {
        return lifevector;
    }
    int64_t population() {
        return lifevector.size();
    }
    int32_t* getrect() {
        return getgridrect(lifevector);
    }
    int64_t digest() {
        return digestvector(lifevector);
    }
    bool empty() {
        return (lifevector.size() == 0);
    }
    bool nonempty() {
        return (lifevector.size() != 0);
    }
    std::string getrle() {
        return vector_to_RLE(lifevector);
    }
    std::string rle_string() {
        return vector_to_RLE(lifevector);
    }
    int32_t period() {
        int32_t i;
        int64_t initdigest = this->digest();
        Pattern pt2 = Pattern(lifevector);
        for (i = 1; i <= MAX_PERIOD; i++) {
            pt2 = pt2.advance(1);
            int64_t newdigest = pt2.digest();
            if (newdigest == initdigest) {
                return i;
            }
        }
        return -1;
    }
    std::pair<int32_t, int32_t> displacement() {
        int32_t ptperiod = this->period();
        if (ptperiod == -1) {
            return std::make_pair(0, 0);
        }
        int32_t* bbox1 = this->getrect();
        Pattern pt2 = this->advance(ptperiod);
        int32_t* bbox2 = pt2.getrect();
        return std::make_pair(bbox2[0] - bbox1[0], bbox2[1] - bbox1[1]);
    }
    std::pair<int32_t, int32_t> displacement(int32_t ptperiod) {
        if (ptperiod == -1) {
            return std::make_pair(0, 0);
        }
        int32_t* bbox1 = this->getrect();
        Pattern pt2 = this->advance(ptperiod);
        int32_t* bbox2 = pt2.getrect();
        return std::make_pair(bbox2[0] - bbox1[0], bbox2[1] - bbox1[1]);
    }
    Pattern translate(const int32_t dx, const int32_t dy) {
        ptvec lifevector2;
        lifevector2.reserve(lifevector.size());
        lifevector2 = translategrid(lifevector, dx, dy);
        return Pattern(lifevector2);
    }
    Pattern operator()(const int32_t dx, const int32_t dy) {
        return this->translate(dx, dy);
    }
    Pattern transform(std::string transformation) {
        ptvec lifevector2;
        lifevector2.reserve(lifevector.size());
        lifevector2 = transformgrid(lifevector, transformation);
        return Pattern(lifevector2);
    }
    Pattern operator()(std::string transformation) {
        return this->transform(transformation);
    }
    Pattern addpt(Pattern& other) {
        ptvec vector2 = other.coords();
        ptvec newvector = applyADD(lifevector, vector2);
        return Pattern(newvector);
    }
    Pattern operator+(Pattern& other) {
        return this->addpt(other);
    }
    Pattern operator+=(Pattern& other) {
        Pattern newpt = this->addpt(other);
        lifevector = newpt.coords();
        return *this;
    }
    Pattern subpt(Pattern& other) {
        ptvec vector2 = other.coords();
        ptvec newvector = applySUB(lifevector, vector2);
        return Pattern(newvector);
    }
    Pattern operator-(Pattern& other) {
        return this->subpt(other);
    }
    Pattern operator-=(Pattern& other) {
        Pattern newpt = this->subpt(other);
        lifevector = newpt.coords();
        return *this;
    }
    Pattern andpt(Pattern& other) {
        ptvec vector2 = other.coords();
        ptvec newvector = applyAND(lifevector, vector2);
        return Pattern(newvector);
    }
    Pattern operator&(Pattern& other) {
        return this->andpt(other);
    }
    std::string apgcode() {
        int32_t ptperiod = this->period();
        int32_t i, j;
        if (ptperiod == -1) {
            return "aperiodic";
        }
        std::string suffix = getapgcodesuffix(lifevector, ptperiod);
        std::pair<int32_t, int32_t> disp = this->displacement(ptperiod);
        if ((disp.first) || (disp.second)) {
            return "xq" + std::to_string(ptperiod) + "_" + suffix;
        }
        else if (ptperiod == 1) {
            return "xs" + std::to_string(this->population()) + "_" + suffix;
        }
        else {
            return "xp" + std::to_string(ptperiod) + "_" + suffix;
        }
    }
    std::string wechsler() {
        return getgridapgcode(lifevector);
    }
    bool operator==(Pattern& other) {
        return (this->digest() == other.digest());
    }
    bool operator!=(Pattern& other) {
        return (!(*this == other));
    }
};
