#pragma once
#include "pixutils/types.hpp"

#include <cmath>
#include <format>
#include <iostream>
#include <string>

template <typename T> class Estimator {

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

    void update(const T v)
    {
        const T D = v - A;
        const T K = 1.0 / (N + 1.0);

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
        return std::format(
            "Estimator(Samples={:3} Mean={:3} Variance={:3})", //
            getNumSamples(), getMean(), getVariance()          //
        );
    }

    friend std::ostream& operator<<(std::ostream& os, const Estimator& obj)
    {
        os << obj.to_string();
        return os;
    }

  private:
    DWord  N;
    double A;
    double Q;
};
