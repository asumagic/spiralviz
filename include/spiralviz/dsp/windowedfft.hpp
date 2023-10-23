// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 sdelang

#pragma once

#include <spiralviz/dsp/util.hpp>

#include <fftw3.h>

#include <cassert>
#include <cstdint>
#include <functional>
#include <memory>
#include <vector>
#include <span>

using FFTWFloat = float;
using FFTWComplex = fftwf_complex;
using InSample = std::int16_t;
using FFTWFPlan = std::remove_pointer_t<fftwf_plan>;

struct FFTWFAllocDeleter
{
    void operator()(void* ptr) { fftwf_free(ptr); }
};

struct FFTWFPlanDeleter
{
    void operator()(fftwf_plan ptr) { fftwf_destroy_plan(ptr); }
};

struct FFTConfig
{
    std::size_t window_size_samples; // N
    std::vector<float> window_factors;
};

enum class WindowType
{
    HAMMING = 0,
    BLACKMAN_HARRIS = 1,
    RECTANGLE = 2
};

static constexpr const char* get_window_type_string(WindowType type)
{
    switch (type)
    {
    case WindowType::HAMMING: return "Hamming";
    case WindowType::BLACKMAN_HARRIS: return "Blackman-Harris";
    case WindowType::RECTANGLE: return "Rectangle";
    default: return "???";
    }
}

struct FFTHighLevelConfig
{
    // float window_size_ms;
    float symmetry_skew_factor;
    WindowType type;

    FFTConfig as_fft_config(std::size_t sample_rate) const;

    auto operator<=>(const FFTHighLevelConfig&) const = default;
};

constexpr FFTHighLevelConfig default_hl_config {
    // .window_size_ms = 300,
    .symmetry_skew_factor = 5.0,
    .type = WindowType::BLACKMAN_HARRIS
};

class WindowedFFT
{
public:
    WindowedFFT(FFTConfig config) noexcept;

    /// Left-shifts the buffer to fit new samples at the right, computes the
    /// new FFT with the desired parameters, then returns a span representing
    /// the output of the FFT which will remain valid and can be written to
    /// until any further action is performed on this object.
    std::span<float> consume_samples(std::span<const InSample> samples);

    /// Clears the internal buffer so that all samples become zero. Does not
    /// cause reallocation and does not touch the internal fftw objects.
    void clear();

    void update_from_config(const FFTConfig& config);
    const FFTConfig& config() const { return m_config; }

private:
    std::size_t initial_sample_buffer_cursor() const;
    void left_shift_sample_buffer(std::size_t by);

    void populate_fft_buffer();

    FFTConfig m_config;

    std::vector<InSample> m_sample_buffer;

    std::unique_ptr<FFTWFloat[], FFTWFAllocDeleter> m_fft_in_buffer;
    std::unique_ptr<FFTWComplex[], FFTWFAllocDeleter> m_fft_out_buffer;
    std::unique_ptr<FFTWFPlan, FFTWFPlanDeleter> m_fft_plan;
};