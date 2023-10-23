// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 sdelang

#include "imgui.h"
#include <spiralviz/gui/fftdebug.hpp>

#include <spiralviz/dsp/windowedfft.hpp>

void FFTDebugGUI::show_params()
{
    ImGui::Begin("FFT parameters");

    FFTHighLevelConfig new_cfg = m_config;

    // ImGui::SliderFloat("Window size (ms)", &new_cfg.window_size_ms, 5.0f, 1000.0f);

    ImGui::PlotLines(
        "##windowplot",
        m_fft.config().window_factors.data(),
        m_fft.config().window_factors.size(),
        0,
        "",
        0.0f,
        1.0f,
        ImVec2(96.0, 32.0)
    );
    ImGui::SameLine();

    if (ImGui::BeginCombo("##combo", get_window_type_string(m_config.type)))
    {
        for (int n = 0; n < 3; n++)
        {
            bool is_selected = int(m_config.type) == n;
            if (ImGui::Selectable(get_window_type_string(WindowType(n)), is_selected))
                new_cfg.type = WindowType(n);
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    ImGui::SameLine();
    ImGui::Text("Window type\n");

    ImGui::SliderFloat("Symmetry skew", &new_cfg.symmetry_skew_factor, 0.1, 32.0);

    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip(
            "When not equal to 1, \"squishes\" around the window function in an"
            " asymmetric way.\n"
            "\n"
            "The higher the value, the further the window function gets"
            " \"squished\" to the right.\n"
            "This reduces latency and means notes linger for less long.\n"
            "However, it might worsen flickering and blurs out the plot."
        );
    }

    if (new_cfg != m_config)
    {
        m_fft.update_from_config(new_cfg.as_fft_config(m_recorder.getSampleRate()));
        m_config = new_cfg;
    }

    if (ImGui::Button("Catch up"))
    {
        m_recorder.discard_n_oldest();
    }

    ImGui::End();
}

void FFTDebugGUI::show_fft(std::span<const float> fft_data)
{
    ImGui::Begin("FFT");

    ImGui::PlotLines(
        "##fftplot",
        fft_data.data(),
        fft_data.size(),
        0,
        nullptr,
        0.0,
        1.0,
        ImVec2(
            ImGui::GetItemRectMax().x,
            ImGui::GetWindowContentRegionMax().y - 48.0f)
    );
    ImGui::Text("%d values\n", int(fft_data.size()));

    ImGui::End();
}