// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 sdelang

#pragma once

#include <SFML/System.hpp>

// FIXME: float type consistency

struct VizParams
{
    float spiral_start = 0.05;
    float spiral_dis = 0.05;
    float spiral_width = 0.03;
    float spiral_blur = 0.02;
    float vol_min = 0.0;
    float vol_max = 0.3;

    bool smooth_fft = true;
};

struct VizPointInformation
{
    float cents;
    float frequency;
    float bin_float;
    int bin;
};

struct VizLocationInformation
{
    float angle;
    float length;
    float band_start_length;
    float band_end_length;
};

sf::Vector2f viz_origin(sf::Vector2f target_resolution);
VizPointInformation viz_info_at_position(sf::Vector2f screen_pos, sf::Vector2f target_resolution, const VizParams& params);
VizLocationInformation viz_points_from_cents(double cents, sf::Vector2f target_resolution, const VizParams& params);
double viz_cents_from_frequency(double frequency);