// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 sdelang

#pragma once

#include <SFML/Graphics.hpp>

#include <spiralviz/dsp/windowedfft.hpp>
#include <spiralviz/audio/recorder.hpp>
#include <spiralviz/gui/vizutil.hpp>

struct FFTDebugParams
{
    bool enable_params_gui = false;
    bool enable_fft_gui = false;
};

// TODO: maybe should split this into FFT config & FFT visualization?
class FFTDebugGUI
{
    public:
    FFTDebugGUI(
        FFTHighLevelConfig* config,
        VizParams* viz_params,
        WindowedFFT* fft,
        SampleQueueRecorder* recorder) :
        m_fft_hl_config{*config},
        m_viz_params{*viz_params},
        m_fft{*fft},
        m_recorder{*recorder}
    {}

    void show_params_gui();
    void show_fft_gui(std::span<const float> fft_data);

    FFTDebugParams& params() { return m_params; }
    const FFTDebugParams& params() const { return m_params; }

    private:
    FFTDebugParams m_params;
    FFTHighLevelConfig& m_fft_hl_config;
    VizParams& m_viz_params;
    WindowedFFT& m_fft;
    SampleQueueRecorder& m_recorder;
};