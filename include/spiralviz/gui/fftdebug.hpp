// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 sdelang

#pragma once

#include <SFML/Graphics.hpp>

#include <spiralviz/dsp/windowedfft.hpp>
#include <spiralviz/audio/recorder.hpp>

struct FFTDebugParams
{
    bool enable_params_gui = false;
    bool enable_fft_gui = false;
};

// TODO: maybe should split this into FFT config & FFT visualization?
class FFTDebugGUI
{
    public:
    FFTDebugGUI(FFTHighLevelConfig* config, WindowedFFT* fft, SampleQueueRecorder* recorder) :
        m_fft_hl_config{*config}, m_fft{*fft}, m_recorder{*recorder}
    {}

    void show_params_gui();
    void show_fft_gui(std::span<const float> fft_data);

    FFTDebugParams& params() { return m_params; }
    const FFTDebugParams& params() const { return m_params; }

    private:
    FFTDebugParams m_params;
    FFTHighLevelConfig& m_fft_hl_config;
    WindowedFFT& m_fft;
    SampleQueueRecorder& m_recorder;
};