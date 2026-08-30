
# pocketcpplife
Simple and compact C++ library for manipulation of patterns in cellular automata.
## Introduction
This project was mainly written as a way to develop my proficiency in C++ by writing a header-only library. I may have a practical use for it in the future, but only on non-x86 systems, as the much faster [lifelib](https://gitlab.com/apgoucher/lifelib), which inspired this project, can be used instead.

## Configuration
The library supports all non-B0 [isotropic rules](https://conwaylife.com/wiki/Isotropic_non-totalistic_rule), but only one rule can be used at a time. To configure the library for a rule, run [config.py](/config.py) with the rulestring as an argument - e.g ```python3 config.py B3/S23``` or ```python3 config.py b3s23-a5```. This should be done ahead of compilation.

You can change certain parameters in the file [userparams.hpp](/includes/userparams.hpp), which defines values such as the unordered map to use (default std::unordered_map) and maximum period for periodicity detection.


## Usage
The library contains a C++ class, ```Pattern```, in [pattern.hpp](/pattern.hpp), which can be manipulated using a range of methods. The class supports two string formats for initialisation - [RLE](https://conwaylife.com/wiki/Run_Length_Encoded) (recommended for most cases) and [apgcodes](https://conwaylife.com/wiki/Apgcode) (useful in occasional cases). Example usage of class methods can be found in [test.cpp](/tests/test.cpp). 

The library can be used on both POSIX and Windows systems, but is much faster on the former.
## Data structure
The internal structure storing the cells of a pattern is a ```std::vector<std::pair<int32_t, int32_t> >```, or in other words, a vector of coordinates. The exact structure can be accessed by using ```Pattern::coords()```.

## Python bindings
Python bindings are avaliable by importing the library as a Python module. The function ```configparams``` is loaded, as well as a Python class ```Pattern```, which can be initialised with an RLE/apgcode and a rule (default ```b3s23```). 

Each ```Pattern``` instance stores only three pieces of data: the rule, a wrapper class around a C++ shared object that contains library functions, and a pointer to a corresponding C++ pattern. Thus, all of the heavy lifting is done by C++, massively reducing the Python overhead.

Compilation is done automatically if using the library from Python, using g++/clang on POSIX and mingw64 on Windows.

### Other functions
Other functions are avaliable for the Python bindings:
- `hashsoup(rule, instring, sym)`: Produces a Pattern instance in `rule` based on the SHA-256 hash of `instring` and the specified symmetry.
- `download_soups(apgcode, rule)`: Returns a dictionary mapping symmetries to lists of sample soups, downloaded from Catagolue.
- `download_synthesis(apgcode)`: Downloads the glider synthesis for a pattern from Catagolue, returning None if it has none, and a Pattern otherwise.
