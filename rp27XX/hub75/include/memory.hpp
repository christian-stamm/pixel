#pragma once
#include "register.hpp"
#include "system.hpp"
#include "utils/logger.hpp"

#include <bitset>
#include <cstdint>
#include <cstdio>
#include <fcntl.h>
#include <ios>
#include <map>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <sys/mman.h>
#include <unistd.h>

class Memory {
  public:
    static inline const uint32_t& read(const uint32_t base, const uint32_t offset)
    {
        // logger.log() << "Read: " << toHexStr(resolve(base + offset));
        return *access(base, offset);
    }

    static inline void write(const uint32_t base, const uint32_t offset, const uint32_t data)
    {
        // logger.log() << "Write: " << toBinStr(data) << " -> " << toHexStr(resolve(base + offset));
        *access(base, offset) = data;
    }

    static inline const uint32_t readwrite(const uint32_t base, const uint32_t offset, uint32_t data, uint32_t mask)
    {
        // logger.log() << "readwrite";
        uint32_t value = read(base, offset);

        value &= ~mask;
        value |= (mask & data);

        write(base, offset, value);

        return value;
    }

    static const std::string toBinStr(uint32_t value)
    {
        std::stringstream ss;
        ss << "0b" << std::bitset<32>(value);
        return ss.str();
    }

    static const std::string toHexStr(uint32_t value)
    {
        std::stringstream ss;
        ss << "0x" << std::uppercase << std::hex << value;
        return ss.str();
    }

  private:
    static inline uint32_t* access(const uint32_t base, const uint32_t offset)
    {
        uint32_t address = resolve(base);

        if (!registry.contains(address)) {
            registry[address] = request(address);
        }

        return registry[address] + offset;
    }

    static uint32_t* request(uint32_t address)
    {
#ifdef SIMULATION
        void* result = malloc(REG_BLOCK_SIZE);
#else
        int mem_fd = open("/dev/mem", O_RDWR | O_SYNC);

        if (mem_fd < 0) {
            throw std::runtime_error("Cannot access '/dev/mem' file... Root permission?");
        }

        void* result = mmap(
            NULL,                   // Any adddress in our space will do
            REG_BLOCK_SIZE,         // Map length
            PROT_READ | PROT_WRITE, // Enable r/w on GPIO registers.
            MAP_SHARED,             // Share
            mem_fd,                 // File to map
            address                 // Address of bcm register
        );

        close(mem_fd);
#endif

        if (result == MAP_FAILED) {
            throw std::runtime_error("Cannot map physical memory address");
        }

        return static_cast<uint32_t*>(result);
    }

    static inline uint32_t resolve(uint32_t peripheralBase)
    {
        return BCM2711_PERI_BASE + peripheralBase;
    }

    static Logger                        logger;
    static std::map<uint32_t, uint32_t*> registry;
};

inline Logger                        Memory::logger = Logger::getLogger("Memory");
inline std::map<uint32_t, uint32_t*> Memory::registry;