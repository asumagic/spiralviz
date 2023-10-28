// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 sdelang

#include "imgui.h"
#include <spiralviz/gui/fftdebug.hpp>

#include <spiralviz/dsp/windowedfft.hpp>

void FFTDebugGUI::show_params_gui()
{
    if (!m_params.enable_params_gui) { return; }

    const auto flags = (
        ImGuiWindowFlags_AlwaysAutoResize
    );
    ImGui::Begin("Spectrogram settings", &m_params.enable_params_gui, flags);

    const auto header_flags = (
        ImGuiTreeNodeFlags_DefaultOpen
    );

    if (ImGui::CollapsingHeader("FFT parameters", header_flags))
    {
        FFTHighLevelConfig new_cfg = m_fft_hl_config;

        // ImGui::SliderFloat("Window size (ms)", &new_cfg.window_size_ms, 5.0f, 1000.0f);

        ImGui::PlotLines(
            "##windowplot",
            m_fft.config().window_factors.data(),
            m_fft.config().window_factors.size(),
            0,
            "",
            0.0f,
            1.0f,
            ImVec2(ImGui::GetContentRegionAvail().x, 32.0)
        );

        if (ImGui::BeginCombo("##ffttype", get_window_type_string(m_fft_hl_config.type)))
        {
            for (int n = 0; n < 3; n++)
            {
                bool is_selected = int(m_fft_hl_config.type) == n;
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

        if (new_cfg != m_fft_hl_config)
        {
            m_fft.update_from_config(new_cfg.as_fft_config(m_recorder.getSampleRate()));
            m_fft_hl_config = new_cfg;
        }
    }

    if (ImGui::CollapsingHeader("Visualization settings", header_flags))
    {
        const auto volume_slider_flags = (
            ImGuiSliderFlags_Logarithmic
        );
        ImGui::SliderFloat("Minimum volume", &m_viz_params.vol_min, 0.0f, 1.0f, "%.3f", volume_slider_flags);
        ImGui::SliderFloat("Maximum volume", &m_viz_params.vol_max, 0.001f, 1.0f, "%.3f", volume_slider_flags);
        ImGui::Separator();

        ImGui::SliderFloat("Scale", &m_viz_params.spiral_dis, 0.01f, 0.2f);
        ImGui::SliderFloat("Start width", &m_viz_params.spiral_start, 0.0f, 0.5f);
        ImGui::SliderFloat("Band width", &m_viz_params.spiral_width, 0.001f, 0.2f);
        ImGui::SliderFloat("Band blur", &m_viz_params.spiral_blur, 0.001f, 0.2f);
        ImGui::Checkbox("Smooth", &m_viz_params.smooth_fft);
    }

    // Currently hasn't been useful whatsoever
    // if (ImGui::Button("Catch up"))
    // {
    //     m_recorder.discard_n_oldest();
    // }

    ImGui::End();
}

void FFTDebugGUI::show_fft_gui(std::span<const float> fft_data)
{
    if (!m_params.enable_fft_gui) { return; }

    ImGui::SetNextWindowSizeConstraints(ImVec2(200.0, 100.0), ImVec2(2000.0, 1000.0));

    ImGui::Begin("Raw FFT view", &m_params.enable_fft_gui);

    ImGui::PlotLines(
        "##fftplot",
        fft_data.data(),
        fft_data.size(),
        0,
        (std::string() + std::to_string(fft_data.size()) + " values").c_str(),
        0.0,
        1.0,
        ImVec2(
            ImGui::GetContentRegionAvail().x,
            ImGui::GetContentRegionAvail().y)
    );

    ImGui::End();
}