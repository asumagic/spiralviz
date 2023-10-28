// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 sdelang

#include "imgui.h"
#include "spiralviz/gui/vizutil.hpp"

#include <spiralviz/dsp/util.hpp>
#include <spiralviz/gui/noterender.hpp>
#include <spiralviz/gui/util.hpp>

#include <SFML/Graphics.hpp>

#include <sstream> // sorry
#include <iomanip>

// TODO: move to dsp/music common?

static constexpr std::array<const char*, 12> note_names_cde {
    "A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#"
};

static constexpr std::array<const char*, 12> note_names_doremi {
    "La", "La#", "Si", "Do", "Do#", "Re", "Re#", "Mi", "Fa", "Fa#", "Sol", "Sol#"
};

// Currently not in use because I am very clueless at music theory and I don't
// think it was actually very meaningful to make sharps look different.
// static constexpr std::array<bool, 12> sharp_table = {
//     false, true, false, false, true, false, true, false, false, true, false, true
// };

static constexpr std::array<float, 3> chord_major = { 0.0f, 4.0f, 7.0f };
static constexpr std::array<float, 3> chord_minor = { 0.0f, 3.0f, 7.0f };

// Based on the Set1 colormap from matplotlib, see
// https://matplotlib.org/stable/gallery/color/colormap_reference.html
static const std::array<sf::Color, 8> default_palette {
    sf::Color{0x1F77B4FF},
    sf::Color{0xFF7F0EFF},
    sf::Color{0x4DAF4AFF},
    sf::Color{0x984EA3FF},
    sf::Color{0xFF7F00FF},
    sf::Color{0xFFFF33FF},
    sf::Color{0xA65628FF},
    sf::Color{0xF781BFFF}
};

NoteRender::NoteRender(const VizParams* params) :
    m_viz_params(*params)
{
    // TODO: from path
    if (!m_note_font.loadFromFile("Cantarell-Regular.ttf"))
    {
        throw std::runtime_error("Failed to open font file for note display");
    }

    m_note_text.setFont(m_note_font);
    m_freq_text.setFont(m_note_font);
}

void NoteRender::render_into(sf::RenderTarget& target, sf::FloatRect target_rect)
{
    render_series_analyzer_into(target, target_rect);
    render_note_indicator_into(target, target_rect);
}

void NoteRender::render_into(sf::RenderTarget& target)
{
    const sf::Vector2u target_size = target.getSize();
    const sf::FloatRect target_rect {
        {0.0f, 0.0f},
        {float(target_size.x), float(target_size.y)}
    };
    render_into(target, target_rect);
}

void NoteRender::render_note_indicator_into(sf::RenderTarget& target, sf::FloatRect target_rect)
{
    if (!m_params.enable_note_render)
    {
        return;
    }

    m_note_text.setCharacterSize(m_params.note_font_size);
    m_freq_text.setCharacterSize(m_params.freq_font_size);

    sf::Vector2f origin{
        target_rect.left + (target_rect.width * 0.5f),
        target_rect.top + (target_rect.height * 0.5f)
    };

    const float line_length = target_rect.height * 0.8f;
    const float note_dist = line_length * 0.55f;
    const sf::Color start_color{110, 110, 150, 255};

    sf::RenderStates lineState;
    lineState.blendMode = sf::BlendMode(sf::BlendMode::SrcColor, sf::BlendMode::DstColor);

    for (int i = 0; i < 12; ++i)
    {
        const float angle = (float(i) / 12.0f) * (std::numbers::pi * 2.0);
        const sf::Vector2f angle_vec{std::cos(angle), std::sin(angle)};

        if (m_params.show_lines)
        {
            target.draw(
                ThickLine{origin, angle, line_length}
                    .with_thickness(8.0f)
                    .with_color(start_color, sf::Color::Transparent),
                lineState
            );
        }

        if (m_params.show_notes)
        {
            const auto note_name = m_params.use_doremi ? note_names_doremi[i] : note_names_cde[i]; 
            m_note_text.setString(note_name);

            m_note_text.setPosition(round(
                origin
                + angle_vec * note_dist
                - sf::Vector2f{m_note_text.getLocalBounds().width * 0.5f, m_note_text.getCharacterSize() * 0.45f}
            ));

            target.draw(m_note_text);
        }

        if (m_params.show_freqs)
        {
            for (int piano_octave = 0; piano_octave < 7; ++piano_octave)
            {
                const auto cents = (i + 12 * piano_octave) * 100.0;
                const auto viz_point = viz_points_from_cents(cents, { target_rect.width, target_rect.height }, m_viz_params);
                const float note_freq = note_frequency(cents, 55.0);

                std::ostringstream ss;
                ss << std::fixed << std::setprecision(0) << note_freq << "Hz";
                m_freq_text.setString(ss.str());

                auto text_pos = round(
                    origin
                    + angle_vec * viz_point.length
                    - sf::Vector2f{m_freq_text.getLocalBounds().width * 0.5f, m_freq_text.getCharacterSize() * 0.45f}
                );

                // draw with a shadow offset
                m_freq_text.setFillColor(sf::Color::Black);
                m_freq_text.setPosition(text_pos + sf::Vector2f{1.0f, 1.0f});
                target.draw(m_freq_text);

                m_freq_text.setFillColor(sf::Color::White);
                m_freq_text.setPosition(text_pos);
                target.draw(m_freq_text);
            }
        }
    }
}

void NoteRender::render_series_analyzer_into(sf::RenderTarget& target, sf::FloatRect target_rect)
{
    if (!m_params.enable_series_analyzer)
    {
        return;
    }

    const auto mouse_pos_imvec2 = ImGui::GetMousePos();
    const sf::Vector2f mouse_pos { mouse_pos_imvec2.x, mouse_pos_imvec2.y };
    const auto freq_info = viz_info_at_position(
        mouse_pos - sf::Vector2f{ target_rect.left, target_rect.top },
        { target_rect.width, target_rect.height },
        m_viz_params
    );
    float main_cents = freq_info.cents;
    float main_freq = freq_info.frequency;

    if (ImGui::IsKeyDown(ImGuiKey_LeftShift))
    {
        main_cents = std::round(main_cents / 100.0) * 100.0;
        main_freq = note_frequency(main_cents, 55.0);
    }

    ImGui::SetTooltip(
        "%.1f cents\n"
        "%.1fHz\n",
        main_cents,
        main_freq
    );

    // TODO: refactor this in a better way
    const auto mode = m_params.series_analyzer_mode;
    switch (mode)
    {
    case SeriesAnalyzerMode::HARMONIC_SERIES:
    {
        for (int harmonic_idx = -1000; harmonic_idx < 1000; ++harmonic_idx)
        {
            const float multiplier = harmonic_idx < 0 ? (1.0f / -harmonic_idx) : harmonic_idx + 1;

            const float harmonic_freq = main_freq * multiplier;

            auto color = (
                harmonic_idx == 0.0f ? sf::Color::White :
                harmonic_idx < 0.0f ? sf::Color::Red :
                sf::Color::Green
            );

            const float thickness = harmonic_idx == 0.0f ? 8.0f : 1.5f;

            render_freq_indicator(target, target_rect, harmonic_freq, thickness, color);
        }
        break;
    }

    case SeriesAnalyzerMode::MAJOR_CHORD:
    case SeriesAnalyzerMode::MINOR_CHORD:
    {
        using Chord = std::span<const float>;
        const auto chord_list = (
            mode == SeriesAnalyzerMode::MAJOR_CHORD ? Chord(chord_major) :
            Chord(chord_minor)
        );

        std::size_t palette_index = 0;
        for (const float semitones_offset : chord_list)
        {
            const float chord_note_freq = main_freq * std::pow(tet_root, semitones_offset);
            const float thickness = semitones_offset != 0.0f ? 4.0f : 8.0f;
            sf::Color color = sf::Color::White;

            if (semitones_offset != 0.0f)
            {
                color = default_palette[palette_index];
                palette_index = (palette_index + 1) % default_palette.size();
            }

            render_freq_indicator(target, target_rect, chord_note_freq, thickness, color);
        }

        break;
    }

    default: break;
    }
}

void NoteRender::render_freq_indicator(sf::RenderTarget& target, sf::FloatRect target_rect, float frequency, float thickness, sf::Color color)
{
    const sf::Vector2f target_resolution { target_rect.width, target_rect.height };
    const auto origin = viz_origin(target_resolution);

    if (frequency < 30.0) { return; }
    if (frequency > 22000.0) { return; }

    const float cents = viz_cents_from_frequency(frequency);
    const auto info = viz_points_from_cents(cents, target_resolution, m_viz_params);

    sf::Vector2f angle_unit_vec { std::cos(info.angle), std::sin(info.angle) };
    const float band_width_px = target_rect.height * m_viz_params.spiral_dis;

    const auto band_start_pos = origin + angle_unit_vec * (info.band_start_length + 0.1f * band_width_px);
    const auto band_end_pos = origin + angle_unit_vec * (info.band_end_length - 0.1f * band_width_px);

    target.draw(
        ThickLine(band_start_pos, band_end_pos)
            .with_color(color)
            .with_thickness(thickness)
    );
}

void NoteRender::show_controls_gui()
{
    if (!m_params.enable_controls_gui) { return; }

    const auto flags = (
        ImGuiWindowFlags_AlwaysAutoResize
    );

    ImGui::Begin("12-TET overlay", &m_params.enable_controls_gui, flags);

    const auto header_flags = (
        ImGuiTreeNodeFlags_Framed
    );

    if (ImGui::TreeNodeEx("12-TET overlay", header_flags | ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Checkbox("Enable overlay", &m_params.enable_note_render);

        if (!m_params.enable_note_render)
        {
            ImGui::BeginDisabled(); 
        }

        ImGui::Checkbox("Show note names", &m_params.show_notes);
        ImGui::Checkbox("Show all note frequencies", &m_params.show_freqs);
        ImGui::Checkbox("Show note lines", &m_params.show_lines);
        ImGui::Checkbox("Use do-re-mi instead of C-D-E", &m_params.use_doremi);

        if (!m_params.enable_note_render)
        {
            ImGui::EndDisabled();
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("Series analyzer", header_flags | ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Checkbox("Enable series analyzer", &m_params.enable_series_analyzer);

        // if (!m_params.enable_series_analyzer)
        // {
        //     ImGui::BeginDisabled();
        // }

        if (ImGui::BeginCombo("##seriesanalysismode", get_series_analyzer_mode_string(m_params.series_analyzer_mode)))
        {
            for (int i = 0; i < int(SeriesAnalyzerMode::TOTAL); ++i)
            {
                const auto current = SeriesAnalyzerMode(i);
                const bool is_selected = m_params.series_analyzer_mode == current;

                if (ImGui::Selectable(get_series_analyzer_mode_string(current), is_selected))
                {
                    m_params.series_analyzer_mode = current;
                }

                if (is_selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }

        // if (!m_params.enable_series_analyzer)
        // {
        //     ImGui::EndDisabled();
        // }

        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("Visual tweaks", header_flags))
    {
        ImGui::SliderInt("Note font size", &m_params.note_font_size, 2, 60);
        ImGui::SliderInt("Text font size", &m_params.freq_font_size, 2, 60);

        ImGui::TreePop();
    }

    ImGui::End();
}