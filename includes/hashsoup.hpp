/* Function for generating soups procedurally based on an SHA256 hash.
The SHA-256 implementation used, sha256.h, is written by me and not third-party. */
#pragma once
#include <cstdint>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include "gridops.hpp"
#include "sha256.h"
// Filter out duplicate coordinates:
ptvec filtersoup(const ptvec thesoup) {
    std::set<std::pair<int32_t, int32_t> > coordset;
    ptvec newsoup;
    for (size_t i = 0; i < thesoup.size(); i++) {
        std::pair<int32_t, int32_t> cpair = thesoup[i];
        if (coordset.find(cpair) == coordset.end()) {
            newsoup.push_back(cpair);
            coordset.insert(cpair);
        }
    }
    return newsoup;
}
/* This function is a C++ translation of a Python 3 translation of a Python 2 function.
   There may be a few problems. */
ptvec _hashsoup(const std::string instring, std::string sym) {
    const std::string lsym = tolower(sym);
    const bool is_stdin = (lsym.find("stdin") != std::string::npos);
    if (sym.length() < 2) {
        ptvec empty;
        return empty;
    }
    if (!is_stdin) {
        if ((sym[0] == 'G') || (sym[0] == 'H')) {
            sym[0] -= 4;
        }
    }
    if (!is_stdin) {
        size_t souplength;
        // Hash the instring:
        uint8_t* s = sha256_str(instring.c_str());
        ptvec thesoup;
        int d;
        if ((sym == "D2_x") || (sym == "D8_1") || (sym == "D8_4")) {
            d = 1;
        }
        else if ((sym == "D4_x1") || (sym == "D4_x4")) {
            d = 2;
        }
        else {
            d = 0;
        }
        size_t i;
        int j, k;
        int32_t x, y;
        uint8_t t;
        for (j = 0; j < 32; j++) {
            t = s[j];
            for (k = 0; k < 8; k++) {
                if (sym == "8x32") {
                    x = k + 8 * (j % 4);
                    y = j / 4;
                }
                else if (sym == "4x64") {
                    x = k + 8 * (j % 8);
                    y = j / 8;
                }
                else if (sym == "2x128") {
                    x = k + 8 * (j % 16);
                    y = j / 16;
                }
                else if (sym == "1x256") {
                    x = k + 8 * (j % 32);
                    y = j / 32;
                }
                else {
                    x = k + 8 * (j % 2);
                    y = j / 2;
                }
                if (t & (1 << (7 - k))) {
                    if ((d == 0) || (x >= y)) {
                        thesoup.push_back(std::make_pair(x, y));
                    }
                    else if (sym == "D4_x1") {
                        thesoup.push_back(std::make_pair(y, -x));
                    }
                    else if (sym == "D4_x4") {
                        thesoup.push_back(std::make_pair(y, -x-1));
                    }
                    if ((sym == "D4_x1") && (x == y)) {
                        thesoup.push_back(std::make_pair(y, -x));
                    }
                    if ((sym == "D4_x4") && (x == y)) {
                        thesoup.push_back(std::make_pair(y, -x-1));
                    }
                }
            }
        }
        free(s);
        if (d >= 1) {
            souplength = thesoup.size();
            for (i = 0; i < souplength; i++) {
                std::pair<int32_t, int32_t> cpair = thesoup[i];
                thesoup.push_back(std::make_pair(cpair.second, cpair.first));
            }
            if (d == 2) {
                if (sym == "D4_x1") {
                    
                    souplength = thesoup.size();
            for (i = 0; i < souplength; i++) {
                        std::pair<int32_t, int32_t> cpair = thesoup[i];
                        thesoup.push_back(std::make_pair(-cpair.second, -cpair.first));
                    }
                }
                else {
                    souplength = thesoup.size();
            for (i = 0; i < souplength; i++) {
                        std::pair<int32_t, int32_t> cpair = thesoup[i];
                        thesoup.push_back(std::make_pair(-cpair.second - 1, -cpair.first - 1));
                    }
                }
            }
        }
        // Odd orthogonal symmetry:
        if ((sym == "D2_+1") || (sym == "D4_+1") || (sym == "D4_+2")) {
            souplength = thesoup.size();
            for (i = 0; i < souplength; i++) {
                std::pair<int32_t, int32_t> cpair = thesoup[i];
                thesoup.push_back(std::make_pair(cpair.first, -cpair.second));
            }
        }
        else if ((sym == "D2_+2") || (sym == "D4_+4")) {
            souplength = thesoup.size();
            for (i = 0; i < souplength; i++) {
                std::pair<int32_t, int32_t> cpair = thesoup[i];
                thesoup.push_back(std::make_pair(cpair.first, -cpair.second-1));
            }
        }
        if (sym == "D4_+1") {
            souplength = thesoup.size();
            for (i = 0; i < souplength; i++) {
                std::pair<int32_t, int32_t> cpair = thesoup[i];
                thesoup.push_back(std::make_pair(-cpair.first, cpair.second));
            }
        }
        else if ((sym == "D4_+2") || (sym == "D4_+4")) {
            souplength = thesoup.size();
            for (i = 0; i < souplength; i++) {
                std::pair<int32_t, int32_t> cpair = thesoup[i];
                thesoup.push_back(std::make_pair(-cpair.first-1, cpair.second));
            }
        }
        if ((sym == "C2_1") || (sym == "C4_1") || (sym == "D8_1")) {
            souplength = thesoup.size();
            for (i = 0; i < souplength; i++) {
                std::pair<int32_t, int32_t> cpair = thesoup[i];
                thesoup.push_back(std::make_pair(-cpair.first, -cpair.second));
            }
        }
        else if ((sym == "C2_2")) {
            souplength = thesoup.size();
            for (i = 0; i < souplength; i++) {
                std::pair<int32_t, int32_t> cpair = thesoup[i];
                thesoup.push_back(std::make_pair(-cpair.first, -cpair.second-1));
            }
        }
        else if ((sym == "C2_2") || (sym == "C4_4") || (sym == "D8_4")) {
            souplength = thesoup.size();
            for (i = 0; i < souplength; i++) {
                std::pair<int32_t, int32_t> cpair = thesoup[i];
                thesoup.push_back(std::make_pair(-cpair.first-1, -cpair.second-1));
            }
        }
        if ((sym == "C4_1") || (sym == "D8_1")) {
            souplength = thesoup.size();
            for (i = 0; i < souplength; i++) {
                std::pair<int32_t, int32_t> cpair = thesoup[i];
                thesoup.push_back(std::make_pair(cpair.second, -cpair.first));
            }
        }
        else if ((sym == "C4_4") || (sym == "D8_4")) {
            souplength = thesoup.size();
            for (i = 0; i < souplength; i++) {
                std::pair<int32_t, int32_t> cpair = thesoup[i];
                thesoup.push_back(std::make_pair(cpair.second, -cpair.first-1));
            }
        }
        return filtersoup(thesoup);
    }
    const size_t first_dash = instring.find("-");
    if (first_dash == std::string::npos) {
        ptvec empty;
        return empty;
    }
    std::string rle = instring.substr(first_dash + 1, instring.length() - first_dash - 1);
    rle = replace(rle, "-", "\n");
    return rle_to_vector(rle);
}
