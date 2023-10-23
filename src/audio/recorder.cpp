#include <spiralviz/audio/recorder.hpp>

SampleQueueRecorder::SampleQueueRecorder()
{
    // bleh, should just use portaudio for a callback-based method
    setProcessingInterval(sf::milliseconds(1));
}

bool SampleQueueRecorder::onProcessSamples(const sf::Int16* samples, std::size_t sampleCount)
{
    std::lock_guard lk{m_stream_lock};
    m_sample_stream.insert(m_sample_stream.end(), samples, samples + sampleCount);
    return true;
}

void SampleQueueRecorder::onStop()
{
    std::lock_guard lk{m_stream_lock};
    m_sample_stream.clear();
}

std::size_t SampleQueueRecorder::consume_n_oldest(std::vector<std::int16_t>& target, std::size_t desired)
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