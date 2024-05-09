#pragma once
#include "pixutils/types.hpp"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

class Estimator {

  public:
    Estimator()
    {
        reset();
    }

    void reset()
    {
        N = 0;
        A = 0;
        Q = 0;
    }

    void update(const float v)
    {
        const double D = v - A;
        const double K = 1.0 / (N + 1.0);

        A += (K * D);
        Q += (1.0 - K) * (D * D);
        N += 1;
    }

    inline const DWord getNumSamples() const
    {
        return N;
    }

    inline const double getMean() const
    {
        return A;
    }

    inline const double getVariance() const
    {
        return 1 <= N ? std::sqrt(Q / N) : 0;
    }

    const std::string to_string() const
    {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(3);
        ss << "N=" << getNumSamples() << ", ";
        ss << "M=" << getMean() << ", ";
        ss << "V=" << getVariance();
        return ss.str();
    }

    friend std::ostream& operator<<(std::ostream& os, const Estimator& obj)
    {
        os << "Estimator(" << obj.to_string() << ")";
        return os;
    }

  private:
    DWord  N;
    double A;
    double Q;
};
