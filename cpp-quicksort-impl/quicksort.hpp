#pragma once
#include <cstdint>
#include <cmath>
#include <string>
#include <vector>
#include <cstdlib>
#include <random>
#include <algorithm>

struct persistent_random_device
{
    using generator = typename std::conditional< (sizeof(std::ptrdiff_t) <= 4), std::mt19937, std::mt19937_64 >::type;
    generator gen;

    persistent_random_device(std::mt19937::result_type value = std::random_device{}())
        : gen(value)
    { }
    std::ptrdiff_t operator () (std::ptrdiff_t start, std::ptrdiff_t end) {
        return std::uniform_int_distribution< std::ptrdiff_t >{start, end}(gen);
    }
} random_gen;

struct quicksort_impl
{
    quicksort_impl() = default;

    template <typename T>
    typename std::vector<T>::iterator partition(
                std::vector<T>& vec,
                const typename std::vector<T>::iterator& low,
                const typename std::vector<T>::iterator& high
                )
    {
        typename std::vector<T>::iterator pivot = high;
        typename std::vector<T>::iterator previous = std::prev(low);

        for(typename std::vector<T>::iterator it = low; it < high; ++it)
        {
            if(*it <= *pivot)
            {
                std::advance(previous, 1);
                std::iter_swap(previous, it);
            }
        }

        std::iter_swap(std::next(previous), high);
        return previous;
    }

    template <typename T>
    typename std::vector<T>::iterator random_partition(
            std::vector<T>& vec,
            const typename std::vector<T>::iterator& low,
            const typename std::vector<T>::iterator& high
            )
    {
        if(low >= high || low == vec.end())
            return low;

        typename std::vector<T>::iterator pivot = low;
        std::advance(pivot, random_gen(0, std::distance(low, high)));

        std::iter_swap(pivot, high);
        return partition(vec, low, high);
    }

    template <typename T>
    void operator () (
                     std::vector<T>& vec,
                     typename std::vector<T>::iterator low,
                     typename std::vector<T>::iterator high
                     )
    {
        if(vec.empty())
            return;

        if(low >= high)
            return;

        typename std::vector<T>::iterator random_it = random_partition(vec, low, high);

        operator()(vec, low, std::prev(random_it));
        operator()(vec, std::next(random_it), high);
    }

} quicksort;
