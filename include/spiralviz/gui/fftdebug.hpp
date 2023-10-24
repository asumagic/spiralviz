// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 sdelang

#pragma once

#include <SFML/Graphics.hpp>

#include <spiralviz/dsp/windowedfft.hpp>
#include <spiralviz/audio/recorder.hpp>

class FFTDebugGUI
{
    public:
    FFTDebugGUI(FFTHighLevelConfig* config, WindowedFFT* fft, SampleQueueRecorder* recorder) :
        m_config{*config}, m_fft{*fft}, m_recorder{*recorder}
    {}

    void show_params_gui();
    void show_fft_gui(std::span<const float> fft_data);

    private:
    FFTHighLevelConfig& m_config;
    WindowedFFT& m_fft;
    SampleQueueRecorder& m_recorder;
};