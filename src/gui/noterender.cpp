// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 sdelang

#include <spiralviz/gui/noterender.hpp>

#include <SFML/Graphics.hpp>

#include <spiralviz/gui/util.hpp>

static constexpr std::array<const char*, 12> note_names_cde {
    "A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#"
};

static constexpr std::array<const char*, 12> note_names_doremi {
    "La", "La#", "Si", "Do", "Do#", "Re", "Re#", "Mi", "Fa", "Fa#", "Sol", "Sol#"
};

static constexpr std::array<bool, 12> sharp_table = {
    false, true, false, false, true, false, true, false, false, true, false, true
};

NoteRender::NoteRender(const VizParams* params) :
    m_params(*params)
{
    // TODO: from path
    if (!m_note_font.loadFromFile("Cantarell-Regular.ttf"))
    {
        throw std::runtime_error("Failed to open font file for note display");
    }

    m_text.setFont(m_note_font);
    m_text.setCharacterSize(20);
}

void NoteRender::render_into(sf::RenderTarget& target, sf::FloatRect target_rect)
{
    sf::Vector2f origin{
        target_rect.left + (target_rect.width * 0.5f),
        target_rect.top + (target_rect.height * 0.5f)
    };

    const float line_length = std::min(target_rect.width, target_rect.height) * 0.8f;
    const float note_dist = line_length * 0.6f;
    const sf::Color start_color{110, 110, 150, 255};

    sf::RenderStates lineState;
    lineState.blendMode = sf::BlendMode(sf::BlendMode::SrcColor, sf::BlendMode::DstColor);

    for (int i = 0; i < 12; ++i)
    {
        const float angle = (float(i) / 12.0f) * (std::numbers::pi * 2.0);

        target.draw(
            ThickLine{origin, angle, line_length}
                .with_thickness(sharp_table[i] ? 3.0f : 8.0f)
                .with_color(start_color, sf::Color::Transparent),
            lineState
        );

        const sf::Vector2f angle_vec{std::cos(angle), std::sin(angle)};
        const auto text_pos = origin + angle_vec * note_dist;
        m_text.setString(note_names_doremi[i]);
        m_text.setPosition(text_pos);
        m_text.setOrigin(m_text.getGlobalBounds().width * 0.5f, m_text.getGlobalBounds().height * 0.5f);
        target.draw(m_text);
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
