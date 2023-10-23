// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 sdelang

#include <spiralviz/fftstreamer.hpp>

FFTStreamer::FFTStreamer(FFTHighLevelConfig config, std::size_t sample_rate) :
    m_fft{config.as_fft_config(sample_rate)}
{
    m_recorder.start(sample_rate);
}

FFTStreamer::~FFTStreamer()
{
    m_recorder.stop();
}

std::span<float> FFTStreamer::update_fft(std::size_t samples_to_load)
{
    if (samples_to_load >= m_fft.config().window_size_samples)
    {
        // discard what we will be unable to use, max out count to the FFT size
        m_recorder.discard_n_oldest(m_fft.config().window_size_samples - samples_to_load);
        samples_to_load = m_fft.config().window_size_samples;
    }

    m_recorder.consume_n_oldest(m_sample_buffer, samples_to_load);

    if (m_sample_buffer.size() > 0)
    {
        return m_fft.consume_samples(m_sample_buffer);
    }

    return {};
}