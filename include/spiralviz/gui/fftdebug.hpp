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

    void show_params();
    void show_fft(std::span<const float> fft_data);

    private:
    FFTHighLevelConfig& m_config;
    WindowedFFT& m_fft;
    SampleQueueRecorder& m_recorder;
};