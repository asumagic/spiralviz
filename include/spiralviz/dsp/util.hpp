// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 sdelang

#pragma once

// 12-TET constants: https://en.wikipedia.org/wiki/12_equal_temperament
constexpr double tune_ref_freq = 440.0 / 2.0;
constexpr double tet_root = 1.05946309435929;

constexpr std::size_t ms_to_samples(std::size_t milliseconds, std::size_t sample_rate)
{
    return (milliseconds * sample_rate) / 1000;
}

constexpr double samples_per_us(std::size_t sample_rate)
{
    return 1.0e6 / sample_rate;
}