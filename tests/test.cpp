#include "../pattern.hpp"
#include <iostream>
int main() {
    Pattern pt("2o$b2o$bo!");
    std::cout << pt.population() << std::endl;
    std::cout << pt.digest() << std::endl;
    std::cout << pt.getrle() << std::endl;
    pt = pt.advance(1103);
    std::cout << pt.population() << std::endl;
    Pattern pt2("x = 9, y = 10, rule = B3/S23\n2bo$2bo$bo$b2o$o2bo$o5b2o$o3b4o$o4b2obo$o2bo2b3o$3o4bo!");
    pt2 = pt2.transform("rot90")[5];
    std::cout << pt2.period() << std::endl;
    std::cout << pt2.apgcode() << std::endl;
    std::pair<int32_t, int32_t> disp = pt2.displacement();
    std::cout << "(" << disp.first << "," << disp.second << ")" << std::endl;
    Pattern pt3("b!");
    std::cout << pt3.apgcode() << std::endl;
    Pattern pt4("xp177_y3343x6bacy6cab6x343zkk8yy8kkzgo8gywg8ogz0123yw321zzgo4syws4ogzgh1yy1hgz221yy122zy3c2cx6d53y635d6xc2c");
    std::cout << pt4.getrle() << std::endl;
    std::cout << pt4.period() << std::endl;
    std::cout << pt4.apgcode() << std::endl;
    int32_t* bbox = pt4.getrect();
    int32_t x = bbox[0]; int32_t y = bbox[1]; int32_t dx = bbox[2]; int32_t dy = bbox[3];
    std::cout << x << ", " << y << ", " << dx << ", " << dy << std::endl;
    Pattern evpt = pt4[177];
    bool ptmatch = (pt4 == evpt);
    std::cout << ptmatch << std::endl;
    Pattern mess = pt;
    mess += pt2;
    std::cout << mess.rle_string() << std::endl;
}
