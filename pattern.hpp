// Header storing the main Pattern class.
#pragma once
#include <ostream>
#include <string>
#include <utility>
#include <vector>
#include "includes/advance.hpp"
#include "includes/gridops.hpp"
#include "includes/hashsoup.hpp"
#include "includes/svg.hpp"
class Pattern {
    public:
    ptvec lifevector;
    std::string rule;
    Pattern(ptvec ptvector, std::string ptrule) {
        lifevector = ptvector;
        rule = ptrule;
    }
    Pattern(const std::string rle, std::string ptrule) {
        if (isapgcode(rle)) {
            lifevector = apgcodetogrid(rle);
        }
        else {
            lifevector = rle_to_vector(rle);
        }
        rule = ptrule;
    }
    Pattern(Pattern& pt) {
        lifevector = pt.coords();
        rule = pt.getrule();
    }
    Pattern(Pattern* ptr) {
        lifevector = ptr->coords();
        rule = ptr->getrule();
    }
    Pattern& operator=(const Pattern& that) {
        lifevector = that.lifevector;
        rule = that.rule;
        return *this;
    }
    ~Pattern() {}
    Pattern advance(const uint32_t generations) const {
        ptvec lifevector2(lifevector);
        cppadvance(lifevector2, generations, rule);
        Pattern* newptr = new Pattern(lifevector2, rule);
        return *newptr;
    }
    Pattern operator[](const uint32_t numgens) const {
        return advance(numgens);
    }
    ptvec coords() const {
        return lifevector;
    }
    uint64_t population() const {
        return lifevector.size();
    }
    int32_t* getrect() const {
        return getgridrect(lifevector);
    }
    int64_t digest() const {
        return digestvector(lifevector);
    }
    std::string getrule() const {
        return rule;
    }
    bool empty() const {
        return (lifevector.size() == 0);
    }
    bool nonempty() const {
        return (lifevector.size() != 0);
    }
    std::string getrle() const {
        return vector_to_RLE(lifevector, slashrule(rule));
    }
    std::string rle_string() const {
        return vector_to_RLE(lifevector, slashrule(rule));
    }
    uint32_t period() const {
        uint32_t i;
        int64_t initdigest = this->digest();
        Pattern pt2 = Pattern(lifevector, rule);
        for (i = 1; i <= MAX_PERIOD; i++) {
            pt2 = pt2[1];
            int64_t newdigest = pt2.digest();
            if (newdigest == initdigest) {
                return i;
            }
        }
        return 0;
    }
    std::pair<int32_t, int32_t> displacement() const {
        uint32_t ptperiod = this->period();
        if (ptperiod == 0) {
            return std::make_pair(0, 0);
        }
        int32_t* bbox1 = this->getrect();
        Pattern pt2 = this->advance(ptperiod);
        int32_t* bbox2 = pt2.getrect();
        return std::make_pair(bbox2[0] - bbox1[0], bbox2[1] - bbox1[1]);
    }
    std::pair<int32_t, int32_t> displacement(const uint32_t ptperiod) const {
        if (ptperiod == 0) {
            return std::make_pair(0, 0);
        }
        int32_t* bbox1 = this->getrect();
        Pattern pt2 = this->advance(ptperiod);
        int32_t* bbox2 = pt2.getrect();
        return std::make_pair(bbox2[0] - bbox1[0], bbox2[1] - bbox1[1]);
    }
    Pattern translate(const int32_t dx, const int32_t dy) const {
        ptvec lifevector2;
        lifevector2.reserve(lifevector.size());
        lifevector2 = translategrid(lifevector, dx, dy);
        return Pattern(lifevector2, rule);
    }
    Pattern operator()(const int32_t dx, const int32_t dy) const {
        return this->translate(dx, dy);
    }
    Pattern transform(const std::string transformation) const {
        ptvec lifevector2;
        lifevector2.reserve(lifevector.size());
        lifevector2 = transformgrid(lifevector, transformation);
        return Pattern(lifevector2, rule);
    }
    Pattern operator()(std::string transformation) const {
        return this->transform(transformation);
    }
    Pattern addpt(const Pattern& other) const {
        ptvec vector2 = other.coords();
        ptvec newvector = applyADD(lifevector, vector2);
        return Pattern(newvector, rule);
    }
    Pattern operator+(const Pattern& other) const {
        return this->addpt(other);
    }
    Pattern operator+=(const Pattern& other) {
        Pattern newpt = this->addpt(other);
        lifevector = newpt.coords();
        return *this;
    }
    Pattern subpt(const Pattern& other) const {
        ptvec vector2 = other.coords();
        ptvec newvector = applySUB(lifevector, vector2);
        return Pattern(newvector, rule);
    }
    Pattern operator-(const Pattern& other) const {
        return this->subpt(other);
    }
    Pattern operator-=(const Pattern& other) {
        Pattern newpt = this->subpt(other);
        lifevector = newpt.coords();
        return *this;
    }
    Pattern andpt(const Pattern& other) const {
        ptvec vector2 = other.coords();
        ptvec newvector = applyAND(lifevector, vector2);
        return Pattern(newvector, rule);
    }
    Pattern operator&(Pattern& other) const {
        return this->andpt(other);
    }
    std::string apgcode() const {
        const uint32_t ptperiod = this->period();
        if (ptperiod == 0) {
            return "aperiodic";
        }
        const std::string suffix = getapgcodesuffix(lifevector, ptperiod, rule);
        const std::pair<int32_t, int32_t> disp = this->displacement(ptperiod);
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
    std::string wechsler() const {
        return getwechsler(lifevector);
    }
    bool operator==(Pattern& other) const {
        return (this->digest() == other.digest());
    }
    bool operator!=(Pattern& other) const {
        return (!(*this == other));
    }
    size_t write_svg(std::ostream& outstream, const int width, const int height, const int period) const {
        std::string graphic;
        if ((period <= 1) || (period > SVG_MAX)) {
            graphic = svg_still(lifevector, width, height);
        }
        else {
            graphic = svg_osc(lifevector, rule, width, height, period);
        }
        outstream << graphic;
        return graphic.length();
    }
    size_t write_svg(std::ostream& outstream, const int width, const int height) const {
        const std::string code = this->apgcode();
        if (code[0] != 'x') {
            return this->write_svg(outstream, width, height, 1);
        }
        if (code[1] == 'q') {
            std::pair<int32_t, int32_t> disp = this->displacement();
            std::string graphic = svg_ship(lifevector, rule, width, height, this->period(), disp.first, disp.second);
            outstream << graphic;
            return graphic.length();
        }
        return this->write_svg(outstream, width, height, this->period());
    }
    std::vector<Pattern*> components() {
        std::vector<ptvec> gridcomp = getcomponents(lifevector);
        std::vector<Pattern*> outvector;
        outvector.reserve(gridcomp.size());
        for (auto i : gridcomp) {
            Pattern* ptr = new Pattern(i, rule);
            outvector.push_back(ptr);
        }
        return outvector;
    }
};
// Other functions:
Pattern hashsoup(const std::string rule, const std::string instring, std::string sym) {
    ptvec soup = _hashsoup(instring, sym);
    return Pattern(soup, rule);
}
