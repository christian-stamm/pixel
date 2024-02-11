#pragma once
#include "pixtools/types.hpp"
#include "pixtools/utils/buffer.hpp"

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