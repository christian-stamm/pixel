#pragma once
#include "pixutils/types.hpp"

#include <Eigen/Dense>

using Eigen::Vector3;

class Position : public Vector3<Byte> {
  public:
    Position(Byte x = 0, Byte y = 0, Byte z = 0)
        : Eigen::Vector3<Byte>(x, y, z)
        , x((*this)[0])
        , y((*this)[1])
        , z((*this)[2])
    {
    }

    Byte &x, &y, &z;
};

class Color : public Eigen::Vector3<Byte> {
  public:
    Color(Byte r = 0, Byte g = 0, Byte b = 0)
        : Eigen::Vector3<Byte>(r, g, b)
        , r((*this)[0])
        , g((*this)[1])
        , b((*this)[2])
    {
    }

    Byte &r, &g, &b;
};

struct Pixel {
    Color    color;
    Position position;
};