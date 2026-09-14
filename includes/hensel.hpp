// C++ functions handling hensel notation.
#pragma once
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include "params.hpp"
const std::vector<std::vector<std::string> > conditionvector = {{""}, {"c", "e"}, {"a","c","e","i","k","n"}, {"a","c","e","i","j","k","n","q","r","y"}, {"a","c","e","i","j","k","n","q","r","t","w","y","z"}, {"a","c","e","i","j","k","n","q","r","y"}, {"a","c","e","i","k","n"}, {"c", "e"}, {""}};
const std::string uppercase = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const std::string lowercase = "abcdefghijklmnopqrstuvwxyz";
umap<std::string, bool*> mainrulemap;
std::string replace(const std::string string1, const std::string target, const std::string replacement) {
    std::string newstring(string1);
    size_t pos = newstring.find(target);
    while (pos != newstring.npos) {
        newstring.replace(pos, target.size(), replacement);
        pos = newstring.find(target, pos + replacement.size());
    }
    return newstring;
}
std::string tolower(const std::string instring) {
    int8_t i;
    std::string outstring = instring;
    for (i = 0; i < 26; i++) {
        std::string char1 = "";
        std::string char2 = "";
        char1 += uppercase[i];
        char2 += lowercase[i];
        outstring = replace(outstring, char1, char2);
    }
    return outstring;
}
void parserule(const std::string rulestring, std::vector<std::string>& conditions) {
    std::string rule = tolower(rulestring);
    rule = replace(rule, "/", "");
    const std::string digits = "012345678";
    const std::string signalchars = "bs|";
    std::string cstring = "";
    bool birth = 1;
    rule += "|";
    size_t i, j;
    for (i = 0; i < rule.length(); i++) {
        char character = rule[i];
        if ((digits.find(character) != digits.npos) || (signalchars.find(character) != signalchars.npos)) {
            cstring = replace(cstring, "b", "");
            cstring = replace(cstring, "s", "");
            if (cstring != "") {
                char digit = cstring[0];
                if (cstring.length() == 1) {
                    for (auto j : conditionvector[digit - '0']) {
                        if (birth) {
                            conditions.push_back("B" + cstring + j);
                        }
                        else {
                            conditions.push_back("S" + cstring + j);
                        }
                    }
                }
                else {
                    std::string subconditions;
                    for (j = 1; j < cstring.length(); j++) {
                        subconditions += cstring[j];
                    }
                    if (subconditions[0] == '-') {
                        std::string subsubconditions;
                        for (j = 2; j < cstring.length(); j++) {
                            subsubconditions += cstring[j];
                        }
                        for (auto j : conditionvector[digit - '0']) {
                            if (subsubconditions.find(j) == subsubconditions.npos) {
                                std::string newcondition;
                                if (birth) {
                                    newcondition = "B";
                                    newcondition += digit;
                                    newcondition += j;
                                }
                                else {
                                    newcondition = "S";
                                    newcondition += digit;
                                    newcondition += j;
                                }
                                conditions.push_back(newcondition);
                            }
                        }
                    }
                    else {
                        for (auto j : conditionvector[digit - '0']) {
                            if (subconditions.find(j) != subconditions.npos) {
                                std::string newcondition;
                                if (birth) {
                                    newcondition = "B";
                                    newcondition += digit;
                                    newcondition += j;
                                }
                                else {
                                    newcondition = "S";
                                    newcondition += digit;
                                    newcondition += j;
                                }
                                conditions.push_back(newcondition);
                            }
                        }
                    }
                } 
            }
            cstring = "";
        }
        if (character == 's') {
            birth = 0;
        }
        cstring += character;
    }
}
void get9bit(const std::string condition, std::vector<int>& conditions) {
    if (condition[0] == 'S') {
        std::vector<int> conditions2;
        get9bit(replace(condition, "S", "B"), conditions2);
        size_t i;
        for (i = 0; i < conditions2.size(); i++) {
            conditions.push_back(conditions2[i] + 16);
        }
        return;
    }
    if (condition == "B1e") {
        conditions = {2, 8, 32, 128};
    }
    else if (condition == "B1c") {
        conditions = {1, 4, 64, 256};
    }
    else if (condition == "B2a") {
        conditions = {3, 6, 9, 36, 72, 192, 288, 384};
    }
    else if (condition == "B2e") {
        conditions = {10, 34, 136, 160};
    }
    else if (condition == "B2c") {
        conditions = {5, 65, 260, 320};
    }
    else if (condition == "B2i") {
        conditions = {40, 130};
    }
    else if (condition == "B2k") {
        conditions = {12, 33, 66, 96, 129, 132, 258, 264};
    }
    else if (condition == "B2n") {
        conditions = {68, 257};
    }
    else if (condition == "B3a") {
        conditions = {11, 38, 200, 416};
    }
    else if (condition == "B3c") {
        conditions = {69, 261, 321, 324};
    }
    else if (condition == "B3e") {
        conditions = {42, 138, 162, 168};
    }
    else if (condition == "B3i") {
        conditions = {7, 73, 292, 448};
    }
    else if (condition == "B3j") {
        conditions = {14, 35, 74, 137, 164, 224, 290, 392};
    }
    else if (condition == "B3k") {
        conditions = {98, 140, 161, 266};
    }
    else if (condition == "B3n") {
        conditions = {13, 37, 67, 193, 262, 328, 352, 388};
    }
    else if (condition == "B3q") {
        conditions = {70, 76, 100, 196, 259, 265, 289, 385};
    }
    else if (condition == "B3r") {
        conditions = {41, 44, 104, 131, 134, 194, 296, 386};
    }
    else if (condition == "B3y") {
        conditions = {97, 133, 268, 322};
    }
    else if (condition == "B4a") {
        conditions = {15, 39, 75, 201, 294, 420, 456, 480};
    }
    else if (condition == "B4c") {
        conditions = {325};
    }
    else if (condition == "B4e") {
        conditions = {170};
    }
    else if (condition == "B4i") {
        conditions = {45, 195, 360, 390};
    }
    else if (condition == "B4j") {
        conditions = {106, 142, 163, 169, 172, 226, 298, 394};
    }
    else if (condition == "B4k") {
        conditions = {99, 141, 165, 225, 270, 330, 354, 396};
    }
    else if (condition == "B4n") {
        conditions = {71, 77, 263, 293, 329, 356, 449, 452};
    }
    else if (condition == "B4q") {
        conditions = {102, 204, 267, 417};
    }
    else if (condition == "B4r") {
        conditions = {43, 46, 139, 166, 202, 232, 418, 424};
    }
    else if (condition == "B4t") {
        conditions = {102, 204, 267, 417};
    }
    else if (condition == "B4w") {
        conditions = {78, 228, 291, 393};
    }
    else if (condition == "B4y") {
        conditions = {101, 197, 269, 323, 326, 332, 353, 389};
    }
    else if (condition == "B4z") {
        conditions = {108, 198, 297, 387};
    }
    else if (condition == "B5a") {
        conditions = {79, 295, 457, 484};
    }
    else if (condition == "B5c") {
        conditions = {171, 174, 234, 426};
    }
    else if (condition == "B5e") {
        conditions = {327, 333, 357, 453};
    }
    else if (condition == "B5i") {
        conditions = {47, 203, 422, 488};
    }
    else if (condition == "B5j") {
        conditions = {103, 205, 271, 331, 358, 421, 460, 481};
    }
    else if (condition == "B5k") {
        conditions = {229, 334, 355, 397};
    }
    else if (condition == "B5n") {
        conditions = {107, 143, 167, 233, 302, 428, 458, 482};
    }
    else if (condition == "B5q") {
        conditions = {110, 206, 230, 236, 299, 395, 419, 425};
    }
    else if (condition == "B5r") {
        conditions = {109, 199, 301, 361, 364, 391, 451, 454};
    }
    else if (condition == "B5y") {
        conditions = {173, 227, 362, 398};
    }
    else if (condition == "B6a") {
        conditions = {111, 207, 303, 423, 459, 486, 489, 492};
    }
    else if (condition == "B6c") {
        conditions = {175, 235, 430, 490};
    }
    else if (condition == "B6e") {
        conditions = {335, 359, 461, 485};
    }
    else if (condition == "B6i") {
        conditions = {365, 455};
    }
    else if (condition == "B6k") {
        conditions = {231, 237, 363, 366, 399, 429, 462, 483};
    }
    else if (condition == "B6n") {
        conditions = {238, 427};
    }
    else if (condition == "B7c") {
        conditions = {239, 431, 491, 494};
    }
    else if (condition == "B7e") {
        conditions = {367, 463, 487, 493};
    }
    else if (condition == "B8") {
        conditions = {495};
    }
    else {                   
        std::cerr << condition << " is not recognised!!" << std::endl;
    }
}
void createconditions(const std::string rulestring, umap<std::string, bool*>* rulemap) {
    bool* conditionarr = (bool*)malloc(512);
    memset(conditionarr, 0, 512);
    std::vector<std::string> conditions;
    parserule(rulestring, conditions);
    std::vector<int> intvec;
    intvec.reserve(9);
    for (std::string i : conditions) {
        get9bit(i, intvec);
        for (int j : intvec) {
            conditionarr[j] = 1;
        }
        intvec.clear();
    }
    (*rulemap)[rulestring] = conditionarr;
}
bool* getmap(std::string rule) {
    if (mainrulemap.count(rule) != 0) {
        return mainrulemap[rule];
    }
    createconditions(rule, &mainrulemap);
    return mainrulemap[rule];
}
