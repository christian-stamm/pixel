#pragma once
#include "memory.hpp"
#include "register.hpp"
#include "system.hpp"
#include "utils/logger.hpp"

#include <bitset>
#include <cstdint>
#include <sstream>
#include <stdint.h>

#define MASK(x) (static_cast<uint32_t>(1 << x))

class GPIO {
  public:
    enum IOMode : uint32_t
    {
        IO_INPUT  = 0b000,
        IO_OUTPUT = 0b001,
        IO_ALT0   = 0b100,
        IO_ALT1   = 0b101,
        IO_ALT2   = 0b110,
        IO_ALT3   = 0b111,
        IO_ALT4   = 0b011,
        IO_ALT5   = 0b010
    };

    enum PUD : uint32_t
    {
        PUD_NONE      = 0b00,
        PUD_PULL_UP   = 0b01,
        PUD_PULL_DOWN = 0b10,
    };

    enum Register : uint32_t
    {
        REG_FSEL0 = 0x00, // GPIO Function Select 0
        REG_SET0  = 0x1c, // GPIO Pin Output Set 0
        REG_SET1  = 0x20, // GPIO Pin Output Set 1
        REG_CLR0  = 0x28, // GPIO Pin Output Clear 0
        REG_CLR1  = 0x2c, // GPIO Pin Output Clear 1
        REG_LEV0  = 0x34, // GPIO Pin Level 0
        REG_LEV1  = 0x38, // GPIO Pin Level 1
        REG_PUD0  = 0xe4, // GPIO Pull-up / Pull-down Register 0
    };

    static inline void setFunc(uint32_t pin, IOMode mode)
    {
        Encoding encoding = encode(Register::REG_FSEL0, pin, 3, 10);
        write(encoding, static_cast<uint32_t>(mode), true);
    }

    static inline uint32_t getFunc(uint32_t pin)
    {
        Encoding encoding = encode(Register::REG_FSEL0, pin, 3, 10);
        return read(encoding);
    }

    static inline void setPin(uint32_t pin, bool enabled = true)
    {
        Register target   = enabled ? Register::REG_SET0 : Register::REG_CLR0;
        Encoding encoding = encode(target, pin, 1, 30);
        write(encoding, 1);
    }

    static inline void clrPin(uint32_t pin)
    {
        setPin(pin, false);
    }

    static inline void pulsePin(uint32_t pin, bool invPulse = false)
    {
        setPin(!invPulse);
        setPin(invPulse);
    }

    static inline void setPins(uint32_t pins, uint32_t mask = 0xFFFFFFFF, bool lower = true)
    {
        Register target    = lower ? Register::REG_SET0 : Register::REG_SET1;
        Encoding encoding  = encode(target, 0, 30, 1);
        uint32_t pinLevels = getPins(lower);

        pinLevels &= ~mask;
        pinLevels |= (mask & pins);

        write(encoding, pinLevels);
    }

    static inline void clrPins(uint32_t pins, uint32_t mask = 0xFFFFFFFF, bool lower = true)
    {
        Register target    = lower ? Register::REG_CLR0 : Register::REG_CLR1;
        Encoding encoding  = encode(target, 0, 30, 1);
        uint32_t pinLevels = getPins(lower);

        pinLevels &= ~mask;
        pinLevels |= (mask & pins);

        write(encoding, pinLevels);
    }

    static inline bool getPin(uint32_t pin)
    {
        Encoding encoding = encode(Register::REG_LEV0, pin, 1, 32);
        return read(encoding);
    }

    static inline uint32_t getPins(bool lower = true)
    {
        Register target   = lower ? Register::REG_LEV0 : Register::REG_LEV1;
        Encoding encoding = encode(target, 0, 32, 1);
        return read(encoding);
    }

    static inline void setPUD(uint32_t pin, PUD pud)
    {
        Encoding encoding = encode(Register::REG_FSEL0, pin, 2, 16);
        write(encoding, static_cast<uint32_t>(pud), true);
    }

    static inline uint32_t getPUD(uint32_t pin)
    {
        Encoding encoding = encode(Register::REG_FSEL0, pin, 2, 16);
        return read(encoding);
    }

    static inline void status()
    {
        logger.log() << "PinStatus: " << Memory::toBinStr(getPins());
    }

  private:
    struct Encoding {
        uint32_t address;
        uint32_t bitShift;
        uint32_t bitMask;
    };

    static inline Encoding encode(
        const Register regBase, const uint32_t bcmPin, const uint32_t bitsPerPin, const uint32_t pinsPerReg)
    {
        uint32_t address  = static_cast<std::uint32_t>(regBase);
        uint32_t bitShift = (bcmPin % pinsPerReg) * bitsPerPin;
        uint32_t regShift = (bcmPin / pinsPerReg) * sizeof(uint32_t);
        uint32_t request  = (address + regShift) / sizeof(uint32_t);

        uint32_t ones = (1ul << bitsPerPin) - 1;
        uint32_t mask = ones << bitShift;

        Encoding encoding{request, bitShift, mask};

        // logger.log() << "Encoding:"                           //
        //              << " Reg: " << Memory::toHexStr(request) //
        //              << " Bit: " << encoding.bitShift         //
        //              << " Mask:" << Memory::toBinStr(mask);

        return encoding;
    }

    static inline void write(const Encoding& encoding, uint32_t data, bool rw = false)
    {
        data = (data << encoding.bitShift) & encoding.bitMask;

        if (!rw) {
            Memory::write(GPIO_REG_OFFSET, encoding.address, data);
        }
        else {
            Memory::readwrite(GPIO_REG_OFFSET, encoding.address, data, encoding.bitMask);
        }

        System::sleep(1e-3);
    }

    static inline uint32_t read(const Encoding& encoding)
    {
        uint32_t data = Memory::read(GPIO_REG_OFFSET, encoding.address);
        return (data & encoding.bitMask) >> encoding.bitShift;
    }

    static Logger logger;
};

inline Logger GPIO::logger = Logger::getLogger("GPIO");