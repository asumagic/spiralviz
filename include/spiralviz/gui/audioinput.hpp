// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 sdelang

#pragma once

#include <SFML/Audio.hpp>

struct AudioInputParams
{
    bool enable_input_gui = true;

    std::string input_name;
};

class AudioInputGUI
{
    public:
    AudioInputGUI(sf::SoundRecorder* recorder);

    void show_gui();

    AudioInputParams& params() { return m_params; }
    const AudioInputParams& params() const { return m_params; }

    private:
    void check_current_device();

    AudioInputParams m_params;
    sf::SoundRecorder& m_recorder;
};
