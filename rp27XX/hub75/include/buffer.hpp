#pragma once
#include <cstdint>
#include <stdint.h>
#include <vector>

class BitPlane {
  public:
    BitPlane(uint32_t length, uint32_t planeID)
        : length(length)
        , planeID(planeID)
        , rgbdata(length)
    {
    }

    const uint32_t        length;
    const uint32_t        planeID;
    std::vector<uint32_t> rgbdata;
};

class ScanLine {
  public:
    ScanLine(uint32_t width, uint32_t lineID, uint32_t numPlanes)
        : width(width)
        , lineID(lineID)
        , numPlanes(numPlanes)
    {
        setup();
    }

    void setup()
    {
        bitPlanes.clear();

        for (uint32_t plane = 0; plane < numPlanes; plane++) {
            bitPlanes.push_back(BitPlane(width, plane));
        }
    }

    const uint32_t        width;
    const uint32_t        lineID;
    const uint32_t        numPlanes;
    std::vector<BitPlane> bitPlanes;
};

class FrameBuffer {
  public:
    FrameBuffer(uint32_t width, uint32_t height, uint32_t depth)
        : width(width)
        , height(height)
        , numPlanes(depth)
    {
        setup();
    }

    void setup()
    {
        scanLines.clear();

        for (uint32_t dualLine = 0; dualLine < height / 2; dualLine++) {
            scanLines.push_back(ScanLine(width, dualLine, numPlanes));
        }
    }

    const uint32_t        width, height, numPlanes;
    std::vector<ScanLine> scanLines;
};
