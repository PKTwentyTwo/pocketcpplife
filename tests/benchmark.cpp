// Basic benchmark using Lidka.
// Only measures time to the nearest second, so mainly used for testing on less powerful devices.
#include "../pattern.hpp"
#include <iostream>
#include <ctime>
int main() {
    std::time_t t1 = std::time(0);
    Pattern pt("x = 9, y = 15, rule = B3/S23\nbo$obo$bo8$8bo$6bobo$5b2obo2$4b3o!");
    std::cout << "Initial population: " << pt.population() << std::endl;
    pt = pt.advance(29055);
    std::time_t t2 = std::time(0);
    std::cout << "Simulated 29055 generations of Lidka in " << t2 - t1 << " seconds." << std::endl;
    std::cout << "Final population: " << pt.population() << " (If not 1625, something has gone wrong)" << std::endl;
}
