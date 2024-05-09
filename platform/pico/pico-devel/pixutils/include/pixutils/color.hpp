#include "pixutils/types.hpp"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

class Color {
  public:
    struct HSV {
        float h, s, v;

        std::string to_string() const
        {
            std::stringstream ss;
            ss << "H:" << std::setw(3) << h << ", ";
            ss << "S:" << std::setw(3) << s << ", ";
            ss << "V:" << std::setw(3) << v;
            return ss.str();
        }

        friend std::ostream& operator<<(std::ostream& os, const HSV& obj)
        {
            os << "Color(" << obj.to_string() << ")";
            return os;
        }
    };

    struct RGB {
        Byte r, g, b;

        std::string to_string() const
        {
            std::stringstream ss;
            ss << "R:" << std::setw(3) << r << ", ";
            ss << "G:" << std::setw(3) << g << ", ";
            ss << "B:" << std::setw(3) << b;
            return ss.str();
        }

        friend std::ostream& operator<<(std::ostream& os, const RGB& obj)
        {
            os << "Color(" << obj.to_string() << ")";
            return os;
        }
    };

    static RGB convert(const HSV& color)
    {
        const auto [h, s, v] = color;
        const float c        = v * s;
        const float x        = c * (1 - std::abs(std::fmod(h / 60.0, 2) - 1));
        const float m        = v - c;
        const float rs[]     = {c, x, 0, 0, x, c};
        const float gs[]     = {x, c, c, x, 0, 0};
        const float bs[]     = {0, 0, x, c, c, x};
        return {
            static_cast<Byte>((rs[(static_cast<int>(h) / 60) % 6] + m) * 255),
            static_cast<Byte>((gs[(static_cast<int>(h) / 60) % 6] + m) * 255),
            static_cast<Byte>((bs[(static_cast<int>(h) / 60) % 6] + m) * 255)};
    }

    static HSV convert(const RGB& rgb)
    {
        const float r      = rgb.r / 255.0f;
        const float g      = rgb.g / 255.0f;
        const float b      = rgb.b / 255.0f;
        const float maxVal = std::max({r, g, b});
        const float minVal = std::min({r, g, b});
        const float delta  = maxVal - minVal;

        const float s = maxVal == 0 ? 0 : delta / maxVal;
        const float v = maxVal;

        float h;
        if (delta == 0)
            h = 0;
        else if (maxVal == r)
            h = 60 * ((g - b) / delta);
        else if (maxVal == g)
            h = 60 * ((b - r) / delta + 2);
        else if (maxVal == b)
            h = 60 * ((r - g) / delta + 4);
        else
            h = 0;

        if (h < 0) {
            h = std::fmod(h, 360) + 360;
        }

        return {h, s, v};
    }
};
