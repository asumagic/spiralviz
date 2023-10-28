// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 sdelang

#include <spiralviz/gui/vizutil.hpp>

#include <spiralviz/dsp/util.hpp>
#include <spiralviz/gui/util.hpp>

sf::Vector2f viz_origin(sf::Vector2f target_resolution)
{
    return target_resolution * 0.5f;
}

// TODO: we should really express all cents as using 440Hz as a tuning
// reference instead of using a weird one...

VizPointInformation viz_info_at_position(sf::Vector2f screen_pos, sf::Vector2f screen_resolution, const VizParams& params)
{
    // this closely matches the logic within the GLSL fragment, which means we
    // sort of have duplicate code...

    const sf::Vector2f uv = {
        screen_pos.x / screen_resolution.x,
        (1.0f - screen_pos.y / screen_resolution.y)
    };
    const float aspect = screen_resolution.x / screen_resolution.y;

    sf::Vector2f uvcorrected = uv - sf::Vector2f{0.5f, 0.5f};
    uvcorrected.x *= aspect;

    const float angle = std::atan2(uvcorrected.y, uvcorrected.x);
    const float base_offset = length(uvcorrected) - params.spiral_start;
    const float offset = base_offset + (angle / (std::numbers::pi * 2.0)) * params.spiral_dis;
    const float which_turn = std::floor(offset / params.spiral_dis);
    const float cents = (which_turn - (angle / (std::numbers::pi * 2.0))) * 1200.0f;

    // TODO: use note_frequency() or similar
    // FIXME: why 55Hz??
    const float frequency = 55.0 * std::pow(tet_root, cents / 100.0f);

    // FIXME: hardcoded sample rate
    const float bin_float = frequency / 44100;
    const int bin = std::round(bin_float);

    return {
        .cents = cents,
        .frequency = frequency,
        .bin_float = bin_float,
        .bin = bin
    };
}

VizLocationInformation viz_points_from_cents(double cents, sf::Vector2f target_resolution, const VizParams& params)
{
    const float band_start_length = (
        params.spiral_start
        + params.spiral_dis * (cents / cents_per_octave)
    ) * target_resolution.y;

    const float band_end_length = (
        band_start_length
        + 1.0 * params.spiral_dis * target_resolution.y
    );

    const float freq_dist = (
        band_start_length
        + 0.4 * params.spiral_dis * target_resolution.y
    );

    const float freq_angle = (
        std::fmod(cents, cents_per_octave) / cents_per_octave
        * std::numbers::pi * 2.0
    );

    const sf::Vector2f freq_unit_vector { std::cos(freq_angle), std::sin(freq_angle) };

    return {
        .angle = freq_angle,
        .length = freq_dist,
        .band_start_length = band_start_length,
        .band_end_length = band_end_length
    };
}

double viz_cents_from_frequency(double frequency)
{
    // freq = ref * (2^(1/12)) ^ (cents / 100.0)
    // freq = ref * (2^(cents / 1200.0))
    // freq / ref = 2 ^ (cents / 1200.0)
    // freq / ref = cents * ln(2) / 1200
    // cents = (ln(freq) - ln(ref)) * (1200 / ln(2))

    // this might be very overcomplicated
    // FIXME: 55 or 110?
    return (std::log2(frequency) - std::log2(55)) * 1200.0 / std::log2(2);
}
