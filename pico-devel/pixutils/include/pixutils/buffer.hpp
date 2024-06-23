#pragma once
#include "pixutils/config.hpp"
#include "pixutils/system.hpp"
#include "pixutils/types.hpp"

#include <algorithm>
#include <format>
#include <iostream>
#include <map>
#include <ostream>
#include <span>
#include <vector>

template <typename T> using Memory = std::vector<T>;

template <typename T> class Buffer : public std::span<T> {
  public:
    Buffer(const Buffer<T>& other)
        : Buffer<T>(other.identifier, other.offset, other.length)
    {
        System::log(INFO) << "COPY";
    }

    Buffer(Buffer<T>&& other) noexcept
        : std::span<T>(std::move(other))
        , offset(other.offset)
        , length(other.length)
        , identifier(other.identifier)
    {
        System::log(INFO) << "MOVE";
    }

    ~Buffer()
    {
        Word remaining = memoryInstances[identifier] - 1;

        System::log(INFO) << "DESTROYED: Remaining = " << remaining;

        if (remaining == 0) {
            memoryInstances.erase(identifier);
            memoryRegistry.erase(identifier);
        }
        else {
            memoryInstances[identifier] = remaining;
        }
    }

    static Buffer<T> build(const Word size)
    {
        const Word identifier       = counter++;
        memoryRegistry[identifier]  = std::vector<T>(size);
        memoryInstances[identifier] = 0;
        return Buffer<T>(identifier, 0, size);
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

    inline void fill(const T& value)
    {
        std::fill(this->begin(), this->end(), value);
    }

    inline Buffer<T> subrange(Word offset, Word length) const
    {
        return Buffer<T>(identifier, offset, length);
    }

    const Word offset;
    const Word length;
    const Word identifier;

  private:
    Buffer(const Word identifier, const Word offset, const Word length)
        : std::span<T>({memoryRegistry[identifier].data() + offset, length})
        , offset(offset)
        , length(length)
        , identifier(identifier)
    {
        Word existing               = memoryInstances[identifier] + 1;
        memoryInstances[identifier] = existing;

        System::log(INFO) << "INITIALIZED: EXISTING = " << existing;
    }

    static Word counter;

    static std::map<Word, Memory<T>> memoryRegistry;
    static std::map<Word, Word>      memoryInstances;
};

template <typename T> inline Word                      Buffer<T>::counter = 0;
template <typename T> inline std::map<Word, Memory<T>> Buffer<T>::memoryRegistry;
template <typename T> inline std::map<Word, Word>      Buffer<T>::memoryInstances;