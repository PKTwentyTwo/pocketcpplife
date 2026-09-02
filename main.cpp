#include "pattern.hpp"
#include <cstring>
#include <fstream>
extern "C" {
void* NewPattern(const char* rle) {
    Pattern* pt = new Pattern(rle);
    return reinterpret_cast<void*>(pt);
}
void* CopyPattern(void* ptr) {
    Pattern* pt = reinterpret_cast<Pattern*>(ptr);
    Pattern* pt2 = new Pattern(pt->coords());
    return reinterpret_cast<void*>(pt2);
}
uint64_t GetPopulation(void* ptr) {
    Pattern* pt = reinterpret_cast<Pattern*>(ptr);
    return pt->population();
}
uint32_t GetPeriod(void* ptr) {
    Pattern* pt = reinterpret_cast<Pattern*>(ptr);
    return pt->period();
}
int64_t GetDigest(void* ptr) {
    Pattern* pt = reinterpret_cast<Pattern*>(ptr);
    return pt->digest();
}
bool IsEmpty(void* ptr) {
    Pattern* pt = reinterpret_cast<Pattern*>(ptr);
    return pt->empty();
}
bool IsNonEmpty(void* ptr) {
    Pattern* pt = reinterpret_cast<Pattern*>(ptr);
    return pt->nonempty();
}
bool IsEqual(void* ptr1, void* ptr2) {
    Pattern* pt1 = reinterpret_cast<Pattern*>(ptr1);
    Pattern* pt2 = reinterpret_cast<Pattern*>(ptr2);
    return (pt1->digest() == pt2->digest());
}
void* AddPattern(void* ptr1, void* ptr2) {
    Pattern* pt1 = reinterpret_cast<Pattern*>(ptr1);
    Pattern* pt2 = reinterpret_cast<Pattern*>(ptr2);
    Pattern* pt3 = new Pattern(pt1->addpt(*pt2));
    return reinterpret_cast<void*>(pt3);
}
void* SubtractPattern(void* ptr1, void* ptr2) {
    Pattern* pt1 = reinterpret_cast<Pattern*>(ptr1);
    Pattern* pt2 = reinterpret_cast<Pattern*>(ptr2);
    Pattern* pt3 = new Pattern(pt1->subpt(*pt2));
    return reinterpret_cast<void*>(pt3);
}    
void* AdvancePattern(void* ptr, const int32_t gens) {
    Pattern* pt = reinterpret_cast<Pattern*>(ptr);
    Pattern* pt2 = new Pattern(pt->advance(gens).coords());
    return reinterpret_cast<void*>(pt2);
}
void* TranslatePattern(void* ptr, const int32_t dx, const int32_t dy) {
    Pattern* pt = reinterpret_cast<Pattern*>(ptr);
    Pattern* pt2 = new Pattern(pt->translate(dx, dy));
    return reinterpret_cast<void*>(pt2);
}
void* TransformPattern(void* ptr, const char* transformation) {
    Pattern* pt = reinterpret_cast<Pattern*>(ptr);
    Pattern* pt2 = new Pattern(pt->transform(transformation));
    return reinterpret_cast<void*>(pt2);
}
void GetPatternRLE(void* ptr, char* buffer, const int buflen) {
    Pattern* pt = reinterpret_cast<Pattern*>(ptr);
    std::string rle = pt->rle_string();
    if (rle.length() < buflen) {
        const char* crle = rle.c_str();
        memcpy(buffer, crle, strlen(crle));
    }
    else {
        std::string errormsg = "!" + std::to_string(rle.length() + 5);
        const char* cerrormsg = errormsg.c_str();
        memcpy(buffer, cerrormsg, strlen(cerrormsg));
    }
}
void GetPatternApgcode(void* ptr, char* buffer, const int buflen) {
    Pattern* pt = reinterpret_cast<Pattern*>(ptr);
    std::string apgcode = pt->apgcode();
    if (apgcode.length() < buflen) {
        const char* crle = apgcode.c_str();
        memcpy(buffer, crle, strlen(crle));
    }
    else {
        std::string errormsg = "!" + std::to_string(apgcode.length() + 5);
        const char* cerrormsg = errormsg.c_str();
        memcpy(buffer, cerrormsg, strlen(cerrormsg));
    }
}
void GetDisplacement(void* ptr, int32_t* buffer) {
    Pattern* pt = reinterpret_cast<Pattern*>(ptr);
    std::pair<int32_t, int32_t> disp = pt->displacement();
    buffer[0] = disp.first;
    buffer[1] = disp.second;
}
void GetPatternRect(void* ptr, int32_t* buffer) {
    Pattern* pt = reinterpret_cast<Pattern*>(ptr);
    int32_t* rect = pt->getrect();
    for (int i = 0; i < 4; i++) {
        buffer[i] = rect[i];
    }
    free(rect);
}
uint64_t WriteSVG(void* ptr, const char* filename, const int width, const int height) {
    Pattern* pt = reinterpret_cast<Pattern*>(ptr);
    std::ofstream outfile(filename);
    if (!outfile.is_open()) {
        return 0;
    }
    uint64_t outlen = pt->write_svg(outfile, width, height);
    outfile.close();
    return outlen;
}
uint64_t WriteSVGGens(void* ptr, const char* filename, const int width, const int height, const int generations) {
    Pattern* pt = reinterpret_cast<Pattern*>(ptr);
    std::ofstream outfile(filename);
    if (!outfile.is_open()) {
        return 0;
    }
    uint64_t outlen = pt->write_svg(outfile, width, height, generations);
    outfile.close();
    return outlen;
}
void* PatternHashsoup(const char* instring, const char* symmetry) {
    Pattern* pt = new Pattern(hashsoup(instring, symmetry));
    return reinterpret_cast<void*>(pt);
}
void DeletePattern(void* ptr) {
    delete reinterpret_cast<Pattern*>(ptr);
}
}
