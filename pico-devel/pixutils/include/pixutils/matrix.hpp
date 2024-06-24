#pragma once
#include "pixutils/buffer.hpp"
#include "pixutils/types.hpp"

#include <pico/bit_ops.h>

class Matrix32 : public Buffer<Word> {
  public:
    Matrix32()
        : Buffer<Word>(Buffer<Word>::build(32))
    {
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
        Buffer<Word>& buffer = *this;
        Word          j, k, m, t;

        m = 0x0000FFFF;

        for (j = 16; j != 0; j >>= 1, m ^= (m << j)) {
            for (k = 0; k < 32; k = ((k + j + 1) & ~j)) {
                t = (buffer[k] ^ (buffer[k + j] >> j)) & m;

                buffer[k]     = buffer[k] ^ t;
                buffer[k + j] = buffer[k + j] ^ (t << j);
            }
        }

        return *this;
    }
};

class Matrix8 : public Buffer<Byte> {
  public:
    Matrix8()
        : Buffer<Byte>(Buffer<Byte>::build(8))
    {
    }

    inline Matrix8& transpose()
    {
        Word x, y, t;
        // Load the array and pack it into x and y.

        Buffer<Byte>& buffer = *this;

        x = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
        y = (buffer[4] << 24) | (buffer[5] << 16) | (buffer[6] << 8) | buffer[7];

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

        buffer[0] = x >> 24;
        buffer[1] = x >> 16;
        buffer[2] = x >> 8;
        buffer[3] = x;

        buffer[4] = y >> 24;
        buffer[5] = y >> 16;
        buffer[6] = y >> 8;
        buffer[7] = y;

        return *this;
    }
};