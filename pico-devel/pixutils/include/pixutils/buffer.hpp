#pragma once
#include "pixutils/types.hpp"

#include <algorithm>
#include <format>
#include <iostream>
#include <memory>
#include <ostream>
#include <span>
#include <stdexcept>
#include <vector>

template <typename T> using Memory    = std::vector<T>;
template <typename T> using MemoryPtr = std::shared_ptr<Memory<T>>;

template <typename T> class Buffer : public std::span<T> {
  public:
    Buffer(const Buffer<T>& other)
        : Buffer<T>(other.storage, other.offset, other.length)
    {
    }

    Buffer(Buffer<T>&& other) noexcept
        : Buffer<T>(std::move(other.storage), other.offset, other.length)
    {
    }

    static Buffer<T> build(const Word size)
    {
        return Buffer<T>(std::make_shared<Memory<T>>(size, 0), 0, size);
    }

    friend std::ostream& operator<<(std::ostream& os, const Buffer<T>& obj)
    {
        os << "Buffer(" << std::endl;

        for (Word line = 0; line < obj.size(); line++) {
            os << std::format("{:06}: 0b{:032b}", line, obj[line]) << std::endl;
        }

        os << ")";
        return os;
    }

    inline void reset()
    {
        fill(0);
    }

    inline void fill(const T& value)
    {
        std::fill(this->begin(), this->end(), value);
    }

    inline Buffer<T> subrange(Word offset, Word length) const
    {
        const Word shift = std::max<Word>(offset - this->offset, 0);
        const Word range = std::max<Word>(this->length - shift, 0);

        if (range < length) {
            throw std::runtime_error(std::format("Out of Bounds: Requested Length={}, Valid Range={}", range, length));
        }

        return Buffer<T>(storage, this->offset + shift, length);
    }

    const Word         offset;
    const Word         length;
    const MemoryPtr<T> storage;

  protected:
    Buffer(const MemoryPtr<T>& storage, const Word offset, const Word length)
        : std::span<T>({storage->data() + offset, length})
        , offset(offset)
        , length(length)
        , storage(storage)
    {
    }
};