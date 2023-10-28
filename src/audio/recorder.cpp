// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 sdelang
#include <spiralviz/audio/recorder.hpp>

SampleQueueRecorder::SampleQueueRecorder()
{
    // bleh, should just use portaudio for a callback-based method
    setProcessingInterval(sf::milliseconds(1));
}

bool SampleQueueRecorder::onProcessSamples(const sf::Int16* samples, std::size_t sampleCount)
{
    std::lock_guard lk{m_stream_lock};
    
    const auto old_size = m_sample_stream.size();
    m_sample_stream.resize(old_size + sampleCount);

    for (std::size_t i = 0; i < sampleCount; ++i)
    {
        m_sample_stream[old_size + i] = samples[i] / 32768.0f;
    }

    return true;
}

void SampleQueueRecorder::onStop()
{
    std::lock_guard lk{m_stream_lock};
    m_sample_stream.clear();
}

std::size_t SampleQueueRecorder::consume_n_oldest(std::vector<float>& target, std::size_t desired)
{
    std::lock_guard lk{m_stream_lock};

    std::size_t target_new_size = std::min(desired, m_sample_stream.size());
    target.assign(m_sample_stream.begin(), m_sample_stream.begin() + target_new_size);
    m_sample_stream.erase(m_sample_stream.begin(), m_sample_stream.begin() + target_new_size);
    return target_new_size;
}

std::size_t SampleQueueRecorder::num_available_samples() const
{
    std::lock_guard lk{m_stream_lock};
    return m_sample_stream.size();
}

void SampleQueueRecorder::discard_n_oldest(std::size_t count)
{
    std::lock_guard lk{m_stream_lock};
    m_sample_stream.erase(
        m_sample_stream.begin(),
        m_sample_stream.begin() + std::min(count, m_sample_stream.size())
    );
}