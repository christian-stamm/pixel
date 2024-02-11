#pragma once
#include "buffer.hpp"
#include "gpio.hpp"
#include "utils/logger.hpp"

#include <cstdint>
#include <stdint.h>
#include <sys/types.h>

class Matrix {
    using Pin  = uint32_t;
    using Mask = uint32_t;

  public:
    struct PinConfig {
        Pin a, b, c, d, e;

        Pin r1, r2;
        Pin g1, g2;
        Pin b1, b2;

        Pin clk, lat, oe;
    };

    Matrix(const PinConfig& cfg)
        : pinConfig(cfg)
        , ctrlMask(MASK(cfg.clk) | MASK(cfg.lat) | MASK(cfg.oe))
        , muxMask(MASK(cfg.a) | MASK(cfg.b) | MASK(cfg.c) | MASK(cfg.d) | MASK(cfg.e))
        , colMask(MASK(cfg.r1) | MASK(cfg.r2) | MASK(cfg.g1) | MASK(cfg.g2) | MASK(cfg.b1) | MASK(cfg.b2))
    {
        configure();
    }

    void configure() const
    {
        GPIO::setFunc(pinConfig.a, GPIO::IOMode::IO_OUTPUT);
        GPIO::setFunc(pinConfig.b, GPIO::IOMode::IO_OUTPUT);
        GPIO::setFunc(pinConfig.c, GPIO::IOMode::IO_OUTPUT);
        GPIO::setFunc(pinConfig.d, GPIO::IOMode::IO_OUTPUT);
        GPIO::setFunc(pinConfig.e, GPIO::IOMode::IO_OUTPUT);

        GPIO::setFunc(pinConfig.r1, GPIO::IOMode::IO_OUTPUT);
        GPIO::setFunc(pinConfig.g1, GPIO::IOMode::IO_OUTPUT);
        GPIO::setFunc(pinConfig.b1, GPIO::IOMode::IO_OUTPUT);
        GPIO::setFunc(pinConfig.r2, GPIO::IOMode::IO_OUTPUT);
        GPIO::setFunc(pinConfig.g2, GPIO::IOMode::IO_OUTPUT);
        GPIO::setFunc(pinConfig.b2, GPIO::IOMode::IO_OUTPUT);

        GPIO::setFunc(pinConfig.oe, GPIO::IOMode::IO_OUTPUT);
        GPIO::setFunc(pinConfig.clk, GPIO::IOMode::IO_OUTPUT);
        GPIO::setFunc(pinConfig.lat, GPIO::IOMode::IO_OUTPUT);

        disable();

        InitFM6126(64);
    }

    void InitFM6126(int columns) const
    {
        const uint32_t bits_on  = colMask | MASK(pinConfig.a); // Address bit 'A' is always on.
        const uint32_t bits_off = MASK(pinConfig.a);
        const uint32_t mask     = bits_on | MASK(pinConfig.lat);

        // Init bits. TODO: customize, as we can do things such as brightness here,
        // which would allow more higher quality output.
        static const char* init_b12 = "0111111111111111"; // full bright
        static const char* init_b13 = "0000000001000000"; // panel on.

        GPIO::clrPins(MASK(pinConfig.clk) | MASK(pinConfig.lat));

        for (int i = 0; i < columns; ++i) {
            uint32_t value = init_b12[i % 16] == '0' ? bits_off : bits_on;

            if (i > columns - 12)
                value |= MASK(pinConfig.lat);

            GPIO::setPins(value, mask);
            GPIO::pulsePin(pinConfig.clk);
        }

        GPIO::clrPin(pinConfig.lat);

        for (int i = 0; i < columns; ++i) {
            uint32_t value = init_b13[i % 16] == '0' ? bits_off : bits_on;

            if (i > columns - 13)
                value |= MASK(pinConfig.lat);

            GPIO::setPins(value, mask);
            GPIO::pulsePin(pinConfig.clk);
        }

        GPIO::clrPin(pinConfig.lat);
    }

    inline void enable() const
    {
        GPIO::clrPin(pinConfig.oe);
    }

    inline void disable() const
    {
        GPIO::clrPins(0xFFFFFFFF, colMask | muxMask | ctrlMask);
        GPIO::setPin(pinConfig.oe);
    }

    inline void dump(const FrameBuffer& buffer) const
    {
        bool swapScanLine;

        for (const ScanLine& scanLine : buffer.scanLines) {
            logger.log() << "Process ScanLine: " << scanLine.lineID;
            swapScanLine = true;

            for (const BitPlane& bitplane : scanLine.bitPlanes) {
                logger.log() << "Process BitPlane: " << bitplane.planeID;

                for (const uint32_t& bitdata : bitplane.rgbdata) {
                    shift(bitdata);
                }

                if (swapScanLine) {
                    disable();
                    select(scanLine.lineID);
                }

                flush();
                enable();

                swapScanLine = false;

                GPIO::status();
                System::sleep(1);
            }
        }
    }

    const PinConfig pinConfig;
    const Mask      ctrlMask, muxMask, colMask;

    static const PinConfig ADAFRUIT_CFG;

    //   private:
    inline void shift(uint32_t bitdata) const
    {
        GPIO::setPins(bitdata, colMask);
        GPIO::pulsePin(pinConfig.clk);
    }

    inline void flush() const
    {
        logger.log() << "flush";

        GPIO::setPin(pinConfig.lat);
        GPIO::pulsePin(pinConfig.clk);
        GPIO::pulsePin(pinConfig.clk);
        // GPIO::pulsePin(pinConfig.clk);
        GPIO::clrPin(pinConfig.lat);
    }

    inline void select(const uint32_t& mux) const
    {
        logger.log() << "select";

        Mask muxLanes = 0;

        muxLanes |= ((mux & (1 << 0)) != 0) << pinConfig.a;
        muxLanes |= ((mux & (1 << 1)) != 0) << pinConfig.b;
        muxLanes |= ((mux & (1 << 2)) != 0) << pinConfig.c;
        muxLanes |= ((mux & (1 << 3)) != 0) << pinConfig.d;
        muxLanes |= ((mux & (1 << 4)) != 0) << pinConfig.e;

        GPIO::setPins(muxLanes, muxMask);
        GPIO::clrPins(~muxLanes, muxMask);
    }

    static Logger logger;
};

inline const Matrix::PinConfig Matrix::ADAFRUIT_CFG = PinConfig{22, 26, 27, 20, 24, 5, 12, 13, 16, 6, 23, 17, 21, 4};
inline Logger                  Matrix::logger       = Logger::getLogger("Matrix");