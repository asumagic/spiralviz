// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 sdelang

#pragma once

#include <SFML/Audio.hpp>
#include <deque>
#include <limits>
#include <mutex>

class SampleQueueRecorder : public sf::SoundRecorder
{
    public:
    SampleQueueRecorder();

    // SFML overrides
    // bool onStart() override;
    bool onProcessSamples(const sf::Int16* samples, std::size_t sampleCount) override;
    void onStop() override;

    std::size_t consume_n_oldest(
        std::vector<std::int16_t>& target,
        std::size_t desired = std::numeric_limits<std::size_t>::max()
    );
    void discard_n_oldest(std::size_t count = std::numeric_limits<std::size_t>::max());

    std::size_t num_available_samples() const;

    private:
    mutable std::mutex m_stream_lock;
    std::deque<std::int16_t> m_sample_stream;
};