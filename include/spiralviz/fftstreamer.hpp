#pragma once

#include <spiralviz/audio/recorder.hpp>
#include <spiralviz/dsp/windowedfft.hpp>

class FFTStreamer
{
    public:
    FFTStreamer(FFTHighLevelConfig config = default_hl_config, std::size_t sample_rate = 44100);
    ~FFTStreamer();

    /// Attempts to pull up to `sample_count` samples from the audio recorder,
    /// then performs a FFT over the updated input window (keeping past samples
    /// in the window if `sample_count < N`).
    ///
    /// The lifetime properties of the returned span are documented in
    /// `WindowedFFT::consume_samples`. If there were no samples to pull, this
    /// function fails by returning an empty span (0-sized).
    std::span<float> update_fft(std::size_t sample_count);

    SampleQueueRecorder& recorder() { return m_recorder; }
    const SampleQueueRecorder& recorder() const { return m_recorder; }

    WindowedFFT& fft() { return m_fft; }
    const WindowedFFT& fft() const { return m_fft;}

    private:
    SampleQueueRecorder m_recorder;
    WindowedFFT m_fft;

    std::vector<std::int16_t> m_sample_buffer;
};