#pragma once
#include "pixutils/memory/buffer.hpp"
#include "pixutils/types.hpp"

#include <pico/bit_ops.h>

class Matrix32 : public Buffer<Word> {
  public:
    Matrix32()
        : Buffer<Word>(32)
    {
        this->fill(0);
    }

    inline Matrix32& reverse()
    {
        for (Word& value : *this) {
            value = __rev(value);
        }

        return *this;
    }

    inline Matrix32& transpose()
    {
        Word j, k, m, t;
        m = 0x0000FFFF;

        for (j = 16; j != 0; j >>= 1, m ^= (m << j)) {
            for (k = 0; k < 32; k = ((k + j + 1) & ~j)) {
                t = ((*this)[k] ^ ((*this)[k + j] >> j)) & m;

                (*this)[k]     = (*this)[k] ^ t;
                (*this)[k + j] = (*this)[k + j] ^ (t << j);
            }
        }

        return *this;
    }
};

class Matrix8 : public Buffer<Byte> {
  public:
    Matrix8()
        : Buffer<Byte>(8)
    {
        this->fill(0);
    }

    inline Matrix8& reverse()
    {
        for (Byte& value : *this) {
            value = __rev(value);
        }

        return *this;
    }

    inline Matrix8& transpose()
    {
        Word x, y, t;
        // Load the array and pack it into x and y.

        x = (this->at(0) << 24) | (this->at(1) << 16) | (this->at(2) << 8) | this->at(3);
        y = (this->at(4) << 24) | (this->at(5) << 16) | (this->at(6) << 8) | this->at(7);

        t = (x ^ (x >> 7)) & 0x00AA00AA;
        x = x ^ t ^ (t << 7);
        t = (y ^ (y >> 7)) & 0x00AA00AA;
        y = y ^ t ^ (t << 7);

        t = (x ^ (x >> 14)) & 0x0000CCCC;
        x = x ^ t ^ (t << 14);
        t = (y ^ (y >> 14)) & 0x0000CCCC;
        y = y ^ t ^ (t << 14);

        t = (x & 0xF0F0F0F0) | ((y >> 4) & 0x0F0F0F0F);
        y = ((x << 4) & 0xF0F0F0F0) | (y & 0x0F0F0F0F);
        x = t;

        this->at(0) = x >> 24;
        this->at(1) = x >> 16;
        this->at(2) = x >> 8;
        this->at(3) = x;

        this->at(4) = y >> 24;
        this->at(5) = y >> 16;
        this->at(6) = y >> 8;
        this->at(7) = y;

        return *this;
    }
};