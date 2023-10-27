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

NoteRender::NoteRender(const VizParams* params) :
    m_viz_params(*params)
{
    // TODO: from path
    if (!m_note_font.loadFromFile("Cantarell-Regular.ttf"))
    {
        throw std::runtime_error("Failed to open font file for note display");
    }

    m_note_text.setFont(m_note_font);
    m_note_text.setCharacterSize(20);

    m_freq_text.setFont(m_note_font);
    m_freq_text.setCharacterSize(10);
}

void NoteRender::render_into(sf::RenderTarget& target, sf::FloatRect target_rect)
{
    if (!m_params.enable_note_render)
    {
        return;
    }

    sf::Vector2f origin{
        target_rect.left + (target_rect.width * 0.5f),
        target_rect.top + (target_rect.height * 0.5f)
    };

    const float viz_size = target_rect.height;
    const float line_length = viz_size * 0.8f;
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
                    .with_thickness(/*sharp_table[i] ? 3.0f : 8.0f*/ 8.0f)
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
                - sf::Vector2f{m_note_text.getLocalBounds().width * 0.5f, 10.0f}
            ));

            target.draw(m_note_text);
        }

        if (m_params.show_freqs)
        {
            for (int piano_octave = 0; piano_octave < 7; ++piano_octave)
            {
                const auto halftones_from_ref = i + 12 * piano_octave;
                const auto viz_point = viz_point_from_frequency(halftones_from_ref, viz_size, m_viz_params);
                const float note_freq = note_frequency(halftones_from_ref, 55.0);

                std::ostringstream ss;
                ss << std::fixed << std::setprecision(0) << note_freq << "Hz";
                m_freq_text.setString(ss.str());

                auto text_pos = round(
                    origin
                    + angle_vec * viz_point.length
                    - sf::Vector2f{m_freq_text.getLocalBounds().width * 0.5f, 5.0f}
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

void NoteRender::render_into(sf::RenderTarget& target)
{
    const sf::Vector2u target_size = target.getSize();
    const sf::FloatRect target_rect {
        {0.0f, 0.0f},
        {float(target_size.x), float(target_size.y)}
    };
    render_into(target, target_rect);
}

void NoteRender::show_controls_gui()
{
    if (!m_params.enable_controls_gui) { return; }

    const auto flags = (
        ImGuiWindowFlags_AlwaysAutoResize
    );
    ImGui::Begin("12-TET overlay", &m_params.enable_controls_gui, flags);

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

    ImGui::End();
}