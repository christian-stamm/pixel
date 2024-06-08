#pragma once
#include "pixutils/types.hpp"

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
        os << obj.to_string();
        return os;
    }

    const std::string to_string() const
    {
        std::stringstream ss;

        const Word lines    = this->size();
        const Word ldigits  = std::log10(lines) + 1;
        const Word wordsize = std::numeric_limits<T>::digits;

        ss << std::endl << "Buffer(" << std::endl;

        for (Word line = 0; line < lines; line++) {
            std::bitset<wordsize> bank(this->at(line));
            ss << std::string(4, ' ');
            ss << std::setw(ldigits) << line << ": ";
            ss << std::setw(wordsize) << bank;
            if (line < lines - 1) {
                ss << std::endl;
            }
        }

        ss << std::endl << ")";

        return ss.str();
    }

    inline void fill(const T& value)
    {
        std::fill(this->begin(), this->end(), value);
    }

    inline SubBuffer<T> subrange(Word offset, Word length)
    {
        return SubBuffer<T>({this->data() + offset, length});
    }
};
