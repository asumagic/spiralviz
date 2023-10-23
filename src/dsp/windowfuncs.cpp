// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 sdelang

#pragma once

#include <spiralviz/dsp/windowfuncs.hpp>

#include <cmath>
#include <numbers>

constexpr auto pi = std::numbers::pi;

void populate_hamming_factors(std::span<float> factors, float alpha)
{
    for (std::size_t i = 0; i < factors.size(); ++i)
    {
        const double x = std::pow(double(i) / (factors.size() - 1), alpha);
        factors[i] = 0.54 - (0.46 * std::cos(2.0 * pi * x));
    }
}

void populate_blackman_harris_factors(std::span<float> factors, float alpha)
{
    for (std::size_t i = 0; i < factors.size(); ++i)
    {
        const double x = std::pow(double(i) / (factors.size() - 1), alpha);
        factors[i] = (
            0.35875
            - (0.48829 * std::cos(2.0 * pi * x))
            + (0.14128 * std::cos(4.0 * pi * x))
            - (0.01168 * std::cos(6.0 * pi * x))
        );
    }
}

void populate_rectangle_factors(std::span<float> factors)
{
    for (std::size_t i = 0; i < factors.size(); ++i)
    {
        factors[i] = 1.0f;
    }
}