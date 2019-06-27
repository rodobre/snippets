#include "quicksort.hpp"

#include <iostream>
#include <cstdint>
#include <cmath>
#include <string>
#include <vector>
#include <cstdlib>
#include <random>
#include <algorithm>

int main()
{
    std::vector<uint64_t> a = {6, 9, 1206, 94, 43276, 8182, 283 };
    quicksort(a, a.begin(), a.end());

    for(uint64_t& elem : a)
        std::cout << elem << ' ';
    return 0;
}
