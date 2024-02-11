#pragma once
#include "pixtools/types.hpp"

#include <algorithm>
#include <bitset>
#include <cmath>
#include <iomanip>
#include <ios>
#include <iostream>
#include <limits>
#include <ostream>
#include <span>
#include <sstream>
#include <string>
#include <vector>

template <typename T> using SubBuffer = std::span<T>;

template <typename T> class Buffer : public std::vector<T> {
  public:
    using std::vector<T>::vector;

    friend std::ostream& operator<<(std::ostream& os, const Buffer<T>& obj)
    {
        os << "Buffer(" << std::endl;
        os << obj.to_string() << std::endl;
        os << ")";
        return os;
    }

    const std::string to_string() const
    {
        std::stringstream ss;

        const Word lines    = this->size();
        const Word ldigits  = std::log10(lines) + 1;
        const Word wordsize = std::numeric_limits<T>::digits;

        ss << std::right;
        ss << std::setfill('0');

        for (Word line = 0; line < lines; line++) {
            std::bitset<wordsize> bank(this->at(line));
            ss << std::setw(ldigits) << line << ": ";
            ss << std::setw(wordsize) << bank << std::endl;
        }

        return ss.str();
    }

    inline void fill(const T& value)
    {
        std::fill(this->begin(), this->end(), value);
    }

    inline SubBuffer<T> subrange(const Word offset, const Word length)
    {
        return SubBuffer<T>(this->begin() + offset, this->begin() + offset + length);
    }
};