#pragma once
#include <cstdint>
#include <cstdlib>
#include <format>
#include <iostream>

class UUID {
  public:
    static UUID generate()
    {
        const uint32_t blockA = rand();
        const uint32_t blockB = rand();
        const uint32_t blockC = rand();
        const uint32_t blockD = rand();

        return UUID(blockA, blockB, blockC, blockD);
    }

    UUID clone()
    {
        return UUID(blockA, blockB, blockC, blockD);
    }

    bool operator==(const UUID& other) const
    {
        return (this->blockA == other.blockA) && (this->blockB == other.blockB) && //
               (this->blockC == other.blockC) && (this->blockD == other.blockD);
    }

    friend std::ostream& operator<<(std::ostream& os, const UUID& obj)
    {
        os << std::format("UUID({})", obj.toString());
        return os;
    }

    std::string toString() const
    {
        return std::format("{:08x}-{:08x}-{:08x}-{:08x}", blockA, blockB, blockC, blockD);
    }

  private:
    UUID(const uint32_t& blockA, const uint32_t& blockB, const uint32_t& blockC, const uint32_t& blockD)
        : blockA(blockA)
        , blockB(blockB)
        , blockC(blockC)
        , blockD(blockD)
    {
    }

    uint32_t blockA, blockB, blockC, blockD;
};
