// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 sdelang

#include <spiralviz/gui/vizutil.hpp>

/*VizPointInformation viz_info_at_position(sf::Vector2f screen_pos, float viz_size, const VizParams& params)
{
    // FIXME
}*/

VizLocationInformation viz_point_from_frequency(double halftones_from_ref, float viz_size, const VizParams& params)
{
    const float freq_dist = (
        params.spiral_start
        + params.spiral_dis * (halftones_from_ref / 12.0)
        - params.spiral_dis * 0.4f // roughly align to the center of the stripe
    ) * viz_size;

    // TODO: double-check behavior on negative values wrt fmod
    const float freq_angle = (
        std::fmod(halftones_from_ref, 12.0) / 12.0
        * std::numbers::pi * 2.0
    );

    return {
        .angle = freq_angle,
        .length = freq_dist,
    };
}
