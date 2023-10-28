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
    float frequency;
};

struct VizLocationInformation
{
    float angle;
    float length;
};

VizPointInformation viz_info_at_position(sf::Vector2f screen_pos, float viz_size, const VizParams& params);
VizLocationInformation viz_point_from_frequency(double halftones_from_ref, float viz_size, const VizParams& params);
