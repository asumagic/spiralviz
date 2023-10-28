// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 sdelang

#include <spiralviz/gui/audioinput.hpp>

#include <imgui.h>

AudioInputGUI::AudioInputGUI(sf::SoundRecorder* recorder)
    : m_recorder(*recorder)
{}

void AudioInputGUI::show_gui()
{
    if (!m_params.enable_input_gui) { return; }

    const auto flags = (
        ImGuiWindowFlags_AlwaysAutoResize
    );
    ImGui::Begin("Audio input settings", &m_params.enable_input_gui, flags);

    check_current_device();

    const auto available_devices = sf::SoundRecorder::getAvailableDevices();

    if (ImGui::BeginCombo("##inputdevice", m_params.input_name.c_str()))
    {
        for (const auto& available_device : available_devices)
        {
            const bool is_current = available_device == m_params.input_name;
            if (ImGui::Selectable(available_device.c_str(), is_current))
            {
                m_params.input_name = available_device;
                m_recorder.setDevice(available_device);
            }

            if (is_current)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
    }

    ImGui::End();
}

void AudioInputGUI::check_current_device()
{
    m_params.input_name = m_recorder.getDevice();
}
