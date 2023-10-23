// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 sdelang

#include <spiralviz/dsp/windowedfft.hpp>

#include <algorithm>
#include <cmath>
#include <fftw3.h>

#include <spiralviz/dsp/windowfuncs.hpp>

FFTConfig FFTHighLevelConfig::as_fft_config([[maybe_unused]] std::size_t sample_rate) const
{
    FFTConfig ret {
        .window_size_samples = /*ms_to_samples(window_size_ms, sample_rate)*/ 32768
    };

    ret.window_factors.resize(ret.window_size_samples);
    switch (type)
    {
        case WindowType::HAMMING: {
            populate_hamming_factors(ret.window_factors, symmetry_skew_factor);
            break;
        }
        case WindowType::BLACKMAN_HARRIS: {
            populate_blackman_harris_factors(ret.window_factors, symmetry_skew_factor);
            break;
        }
        case WindowType::RECTANGLE: {
            populate_rectangle_factors(ret.window_factors);
            break;
        }
        default:
        assert(false && "unsupported window type");
    }

    return ret;
}

WindowedFFT::WindowedFFT(FFTConfig config) noexcept :
    m_config{std::move(config)},
    m_sample_buffer(m_config.window_size_samples),
    m_fft_in_buffer{fftwf_alloc_real(m_config.window_size_samples)},
    m_fft_out_buffer{fftwf_alloc_complex(m_config.window_size_samples)},
    m_fft_plan{fftwf_plan_dft_r2c_1d(
        m_config.window_size_samples,
        m_fft_in_buffer.get(),
        m_fft_out_buffer.get(),
        FFTW_ESTIMATE
    )}
{}

void WindowedFFT::left_shift_sample_buffer(std::size_t by)
{
    std::move_backward(
        m_sample_buffer.begin() + by,
        m_sample_buffer.end(),
        m_sample_buffer.end() - by
    );
}

void WindowedFFT::populate_fft_buffer()
{
    for (std::size_t i = 0; i < m_sample_buffer.size(); ++i)
    {
        InSample in_sample_int = m_sample_buffer[i];
        float in_sample_norm = (float(in_sample_int)) / 32768.0f;
        float in_sample_windowed = m_config.window_factors[i] * in_sample_norm;

        m_fft_in_buffer[i] = in_sample_windowed;
    }
}

void WindowedFFT::clear()
{
    std::fill(m_sample_buffer.begin(), m_sample_buffer.end(), 0);
}

void WindowedFFT::update_from_config(const FFTConfig& config)
{
    if (config.window_size_samples != m_config.window_size_samples)
    {
        // need to recreate the FFT plan, so just recreate the object
        (*this) = {config};
        return;
    }

    m_config = config;
}

std::span<float> WindowedFFT::consume_samples(std::span<const InSample> incoming)
{
    assert(incoming.size() < m_config.window_size_samples);

    left_shift_sample_buffer(incoming.size());

    std::copy(
        incoming.begin(),
        incoming.end(),
        m_sample_buffer.end() - incoming.size()
    );

    populate_fft_buffer();
    fftwf_execute(m_fft_plan.get());

    // assuming no UB from the use of this cast(?), "theoretically" safe
    // as far as the writes go
    float* float_out_buffer = reinterpret_cast<float*>(m_fft_out_buffer.get());

    const std::size_t final_output_size = m_config.window_size_samples / 2 - 1;

    for (std::size_t i = 0; i < final_output_size; ++i)
    {
        const auto [real, imag] = m_fft_out_buffer[i];
        const auto amplitude = std::sqrt(real*real + imag*imag);
        // float_out_buffer[i] = amplitude / final_output_size; // too harsh..?
        float_out_buffer[i] = amplitude * 0.002f;
    }

    return std::span{float_out_buffer, final_output_size};
}
