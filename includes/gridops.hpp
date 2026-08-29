// As the name suggests, grid operations to work out pattern properties.
#pragma once
#include <algorithm>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>
#include "advance.hpp"
#define ptvec std::vector<std::pair<int32_t, int32_t> >
const std::vector<std::string> orientations = {"identity", "rot90", "rot180", "rot270", "flip_x", "flip_y", "swap_xy", "swap_xy_flip"};
const char characters[37] = "0123456789abcdefghijklmnopqrstuvwxyz";
std::string characterstring(characters);
std::string replace(const std::string string1, const std::string target, const std::string replacement) {
    std::string newstring(string1);
    int32_t pos = newstring.find(target);
    while (pos != newstring.npos) {
        newstring.replace(pos, target.size(), replacement);
        pos = newstring.find(target, pos + replacement.size());
    }
    return newstring;
}
ptvec rle_to_vector(const std::string rle) {
    // Converts an RLE to a grid.
    int32_t x = 0;
    int32_t y = 0;
    ptvec outvector;
    int32_t position = 0;
    std::string cstring = "";
    bool isnum = 0;
    uint8_t asciipos = 0;
    int32_t integer;
    int32_t i;
    bool notfirstloop = 0;
    std::string op;
    while ((position + 1) < rle.length()) {
        if (notfirstloop) {
            position++;
        }
        else {
            notfirstloop = 1;
        }
        asciipos = rle[position];
        if (isnum == 0) {
            if ((rle[position] == '#') || (rle[position] == 'x')) {
                while ((rle[position] != '\n') && (position < rle.length() + 1)) {
                    position++;
                }
                continue;
            }
            if ((asciipos >= 48) && (asciipos <= 58)) {
                isnum = 1;
                cstring = rle[position];
            }
            else {
                op = rle[position];
                if (op == "\n") {
                    continue;
                }
                if (cstring != "") {
                    try {
                        integer = std::stoi(cstring);
                    }
                    catch (...) {
                        integer = 0;
                    }
                }
                else {
                    integer = 1;
                }
                if (op == "o") {
                    for (i = 0; i < integer; i++) {
                        outvector.push_back(std::make_pair(x + i, y));
                    }
                    x += integer;
                }
                else if (op == "b") {
                    x += integer;
                }
                else if (op == "$") {
                    x = 0;
                    y += integer;
                }
                else if (op == "!") {
                    break;
                }
                cstring = "";
            }
        }
        else {
            if ((asciipos < 48) || (asciipos > 58)) {
                isnum = 0;
                position--;
            }
            else {
                if (rle[position] != '\n') {
                    cstring += rle[position];
                }
            }
        }
    }
    return outvector;
}
int32_t* getgridrect(ptvec& ptvector) {
    int32_t* bbox = (int32_t*)malloc(4 * sizeof(int32_t));
    uint32_t i;
    for (i = 0; i < 4; i++) {
        bbox[i] = 0;
    }
    if (ptvector.size() == 0) {
        return bbox;
    }
    int32_t minx = ptvector[0].first;
    int32_t miny = ptvector[0].second;
    int32_t maxx = minx;
    int32_t maxy = miny;
    int32_t x, y;
    for (i = 1; i < ptvector.size(); i++) {
        auto [x, y] = ptvector[i];
        if (x > maxx) {
            maxx = x;
        }
        else if (x < minx) {
            minx = x;
        }
        if (y > maxy) {
            maxy = y;
        }
        else if (y < miny) {
            miny = y;
        }
    }
    bbox[0] = minx;
    bbox[1] = miny;
    bbox[2] = maxx - minx + 1;
    bbox[3] = maxy - miny + 1;
    return bbox;
}
std::string vector_to_RLE(ptvec& ptvector) {
    int32_t* bbox = getgridrect(ptvector);
    const int32_t x = *bbox;
    const int32_t dx = *(bbox + 2);
    const int32_t dy = *(bbox + 3);
    std::string rle = "x = " + std::to_string(dx) + ", y = " + std::to_string(dy)  + ", rule = " + slashedrule + "\n";
    umap<int32_t, std::vector<int32_t> > rows;
    int32_t cx, cy;
    uint32_t i, j;
    for (i = 0; i < ptvector.size(); i++) {
        auto [cx, cy] = ptvector[i];
        if (rows.count(cy) == 0) {
            rows[cy] = {};
        }
        rows[cy].push_back(cx);
    }
    std::vector<int32_t> keys;
    keys.reserve(dy);
    std::vector<int32_t> newrow;
    for (auto i : rows) {
        keys.push_back(i.first);
    }
    sort(keys.begin(), keys.end());
    int32_t prevrow = keys[0];
    int32_t row;
    int32_t prevcolumn, ccolumn;
    for (i = 0; i < keys.size(); i++) {
        row = keys[i];
        int32_t dollars = row - prevrow;
        prevrow = row;
        if (dollars > 1) {
            std::string dollarstring = std::to_string(dollars) + '$';
            rle += dollarstring;
        }
        else if (dollars == 1) {
            rle += '$';
        }
        prevcolumn = x;
        for (j = 0; j < rows[row].size(); j++) {
            std::vector<int32_t> newvector(rows[row]);
            newvector.reserve(rows[row].size());
            sort(newvector.begin(), newvector.end());
            ccolumn = newvector[j];
            int32_t bees = ccolumn - prevcolumn - 1;
            if (j == 0) {
                bees++;
            }
            if (bees > 1) {
                std::string beestring = std::to_string(bees) + 'b';
                rle += beestring;
            }
            else if (bees == 1) {
                rle += 'b';
            }
            rle += 'o';
            prevcolumn = ccolumn;
        }
    }
    // Losslessly compress the RLE:
    i = 1;
    bool oseqvalid = 1;
    while (oseqvalid) {
        i++;
        std::string oseq;
        for (j = 0; j < i; j++) {
            oseq += 'o';
        }
        oseqvalid = (rle.find(oseq) != rle.npos);
    }
    while (i > 2) {
        i--;
        std::string oseq;
        for (j = 0; j < i; j++) {
            oseq += 'o';
        }
        rle = replace(rle, oseq, std::to_string(i) + 'o');
    }
    rle += '!';
    free(bbox);
    return rle;
}
ptvec translategrid(ptvec& ptvector, const int32_t dx, const int32_t dy) {
    ptvec newvec;
    int32_t i;
    for (i = 0; i < ptvector.size(); i++) {
        auto [x, y] = ptvector[i];
        newvec.push_back(std::make_pair(x + dx, y + dy));
    }
    return newvec;
}
ptvec transformgrid(ptvec& ptvector, const std::string transformation) {
    // Applies the given transformation to a vector of coordinates.
    // (I'm not exactly a fan of coordinate geometry).
    ptvec newvec;
    newvec.reserve(ptvector.size());
    int32_t i;
    int32_t x, y;
    if (transformation == "identity") {
        for (i = 0; i < ptvector.size(); i++) {
            auto [x, y] = ptvector[i];
            newvec.push_back(std::make_pair(x, y));
        }
    }
    else if (transformation == "rot90") {
        for (i = 0; i < ptvector.size(); i++) {
            auto [x, y] = ptvector[i];
            newvec.push_back(std::make_pair(y, -x));
        }
    }
    else if (transformation == "rot180") {
        for (i = 0; i < ptvector.size(); i++) {
            auto [x, y] = ptvector[i];
            newvec.push_back(std::make_pair(-x, -y));
        }
    }
    else if (transformation == "rot270") {
        for (i = 0; i < ptvector.size(); i++) {
            auto [x, y] = ptvector[i];
            newvec.push_back(std::make_pair(-y, x));
        }
    }
    else if (transformation == "flip_x") {
        for (i = 0; i < ptvector.size(); i++) {
            auto [x, y] = ptvector[i];
            newvec.push_back(std::make_pair(-x, y));
        }
    }
    else if (transformation == "flip_y") {
        for (i = 0; i < ptvector.size(); i++) {
            auto [x, y] = ptvector[i];
            newvec.push_back(std::make_pair(x, -y));
        }
    }
    else if (transformation == "swap_xy") {
        for (i = 0; i < ptvector.size(); i++) {
            auto [x, y] = ptvector[i];
            newvec.push_back(std::make_pair(-y, -x));
        }
    }
    else if (transformation == "swap_xy_flip") {
        for (i = 0; i < ptvector.size(); i++) {
            auto [x, y] = ptvector[i];
            newvec.push_back(std::make_pair(y, x));
        }
    }
    else {
        for (i = 0; i < ptvector.size(); i++) {
            auto [x, y] = ptvector[i];
            newvec.push_back(std::make_pair(x, y));
        }
    }
    return newvec;
}
std::pair<int32_t, int32_t> getfirstcell(ptvec ptvector) {
    int32_t* bbox = getgridrect(ptvector);
    int32_t y = bbox[1];
    int32_t i;
    int32_t minx = bbox[0] + bbox[2];
    for (auto i : ptvector) {
        if (i.second == y) {
            if (i.first < minx) {
                minx = i.first;
            }
        }
    }
    std::pair<int32_t, int32_t> outpair = std::make_pair(minx, y);
    free(bbox);
    return outpair;
}
bool getcell(ptvec& grid, const int32_t x, const int32_t y) {
    return (std::count(grid.begin(), grid.end(), std::make_pair(x, y)) != 0);
}
int64_t hashpair(const std::pair<int32_t, int32_t> coordpair) {
    int64_t hash = 17;
    hash = ((hash + coordpair.first) << 5) - (hash + coordpair.first);
    hash = ((hash + coordpair.second) << 5) - (hash + coordpair.second);
    hash += coordpair.first + coordpair.second + coordpair.first * coordpair.second;
    return hash;
}
ptvec defaultshiftgrid(ptvec& ptvector) {
    if (ptvector.size() != 0) {
        std::pair<int32_t, int32_t> firstcell = getfirstcell(ptvector);
        ptvec ptvector2 = translategrid(ptvector, -firstcell.first, -firstcell.second);
        return ptvector2;
    }
    else {
        return translategrid(ptvector, 0, 0);
    }
}
int64_t digestvector(ptvec& ptvector) {
    ptvec ptvector2 = defaultshiftgrid(ptvector);
    int64_t hash = 0;
    int32_t i;
    for (auto i : ptvector2) {
        hash += hashpair(i);
    }
    return hash;
}
ptvec applyADD(ptvec vector1, ptvec vector2) {
    ptvec newvector(vector1);
    umap<int64_t,bool> trackermap;
    newvector.reserve(vector1.size() + vector2.size());
    trackermap.reserve(vector1.size() + vector2.size());
    int32_t i;
    int64_t digest;
    for (auto i : vector1) {
        digest = hashpair(i);
        trackermap[digest] = 1;
    }
    for (auto i : vector2) {
        digest = hashpair(i);
        if (trackermap[digest] == 0) {
            newvector.push_back(i);
        }
    }
    return newvector;
}
ptvec applyAND(ptvec vector1, ptvec vector2) {
    ptvec newvector;
    umap<int64_t,bool> trackermap;
    int32_t maxsize;
    if (vector1.size() > vector2.size()) {
        maxsize = vector1.size();
    }
    else {
        maxsize = vector2.size();
    }
    newvector.reserve(maxsize);
    trackermap.reserve(vector1.size() + vector2.size());
    int32_t i;
    int64_t digest;
    for (auto i : vector1) {
        digest = hashpair(i);
        trackermap[digest] = 1;
    }
    for (auto i : vector2) {
        digest = hashpair(i);
        if (trackermap[digest]) {
            newvector.push_back(i);
        }
    }
    return newvector;
}
ptvec applySUB(ptvec vector1, ptvec vector2) {
    ptvec newvector;
    umap<int64_t,bool> trackermap;
    newvector.reserve(vector1.size());
    trackermap.reserve(vector1.size() + vector2.size());
    int32_t i;
    int64_t digest;
    for (auto i : vector1) {
        digest = hashpair(i);
        trackermap[digest] = 1;
    }
    for (auto i : vector2) {
        digest = hashpair(i);
        if (trackermap[digest]) {
            trackermap[digest] = 0;
        }
    }
    for (auto i : vector1) {
        if (trackermap[hashpair(i)]) {
            newvector.push_back(i);
        }
    }
    return newvector;
}
std::string getgridapgcode(ptvec& grid) {
    std::string apgcode;
    ptvec newgrid = defaultshiftgrid(grid);
    int32_t* bbox = getgridrect(newgrid);
    int32_t x = bbox[0];
    int32_t y = bbox[1];
    int32_t dx = bbox[2];
    int32_t dy = bbox[3];
    int32_t w, l, h;
    int32_t val = 0;
    for (w = 0; w < ((dy - 1) / 5) + 1; w++) {
        if (w) {
            apgcode += 'z';
        }
        for (l = 0; l < dx; l++) {
            val = 0;
            for (h = 0; h < 5; h++) {
                val += ((1 << h) * getcell(newgrid, x+l, y + 5*w + h));
            }
            apgcode += characters[val];
        }
    }
    // Compression time:
    while (apgcode.find("0z") != apgcode.npos) {
        apgcode = replace(apgcode, "0z", "z");
    }
    for (w = 39; w > 3; w--) {
        std::string zstr;
        for (l = 0; l < w; l++) {
            zstr += '0';
        }
        std::string ystr = "y";
        ystr += characters[w - 4];
        apgcode = replace(apgcode, zstr, ystr);
    }
    apgcode = replace(apgcode, "000", "x");
    apgcode = replace(apgcode, "00", "w");
    free(bbox);
    return apgcode;
}
std::string compareapgcode(const std::string apgcode1, const std::string apgcode2) {
    const int32_t size1 = apgcode1.length();
    const int32_t size2 = apgcode2.length();
    if (size1 < size2) {
        return apgcode1;
    }
    else if (size1 > size2) {
        return apgcode2;
    }
    if (apgcode1 < apgcode2) {
        return apgcode1;
    }
    return apgcode2;
}
std::string getapgcodesuffix(ptvec& grid, const int32_t period) {
    bool apgcodeknown = 0;
    std::string bestapgcode = "";
    int32_t i, j;
    ptvec cgrid(grid);
    for (i = 0; i < period; i++) {
        for (auto j : orientations) {
            ptvec cgrid2 = transformgrid(cgrid, j);
            std::string gridapgcode = getgridapgcode(cgrid2);
            if (apgcodeknown == 0) {
                bestapgcode = gridapgcode;
                apgcodeknown = 1;
            }
            else {
                bestapgcode = compareapgcode(bestapgcode, gridapgcode);
            }
        }
        cppadvance(cgrid, 1);
    }
    if (bestapgcode.length() == 0) {
        bestapgcode = "0";
    }
    return bestapgcode;
}
ptvec apgcodetogrid(const std::string apgcode) {
    int32_t xpos = 0;
    int32_t ypos = 0;
    int32_t readpos = 0;
    int32_t value = 0;
    ptvec newvector;
    int32_t i, j;
    std::string apgcode2, cstring;
    int32_t underscorepos = apgcode.find("_");
    for (i = underscorepos+1; i < apgcode.length(); i++) {
        apgcode2 += apgcode[i];
    }
    for (i = 35; i > -1; i--) {
        std::string ystring = "y";
        ystring += characters[i];
        std::string zstring = "";
        for (j = -4; j < i; j++) {
            zstring += "0";
        }
        apgcode2 = replace(apgcode2, ystring, zstring);
    }
    apgcode2 = replace(apgcode2, "x", "000");
    apgcode2 = replace(apgcode2, "w", "00");
    apgcode2 = replace(apgcode2, "\n", "");
    while (readpos < apgcode2.length()) {
        char cchar = apgcode2[readpos];
        value = characterstring.find(cchar);
        if (value == characterstring.npos) {
            //Illegal character; return empty list:
            ptvec fakevector;
            return fakevector;
        }
        if ((value >= 0) & (value < 32)) {
            for (i = 0; i < 5; i++) {
                if ((value / (1 << i))%2) {
                    newvector.push_back(std::make_pair(xpos, ypos + i));
                }
            }
            xpos++;
        }
        else if (value == 35) {
            xpos = 0;
            ypos += 5;
        }
        readpos++;
    }
    return newvector;
}
bool isapgcode(const std::string apgcode) {
    if (apgcode[0] != 'x') {
        return 0;
    }
    if (apgcode.find("_") != apgcode.npos) {
        return 1;
    }
    return 0;
}
