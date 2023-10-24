// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 sdelang

#include "SFML/Graphics/BlendMode.hpp"
#include "SFML/Graphics/PrimitiveType.hpp"
#include "SFML/Graphics/Transform.hpp"
#include <spiralviz/gui/noterender.hpp>

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
    // render_line(target, {16, 16}, {64, 64}, 8.0f, sf::Color::White, sf::Color::Transparent);

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
        const sf::Vector2f angle_vec{std::cos(angle), std::sin(angle)};
        
        const auto line_end = origin + angle_vec * line_length;
        const auto text_pos = origin + angle_vec * note_dist;

        const float thickness = sharp_table[i] ? 3.0f : 8.0f;

        render_line(target, origin, line_end, thickness, start_color, sf::Color::Transparent, lineState);

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

void NoteRender::render_line(
    sf::RenderTarget& target,
    sf::Vector2f a,
    sf::Vector2f b,
    float thickness,
    sf::Color a_color,
    sf::Color b_color,
    const sf::RenderStates& states
)
{
    // I wrote this terrible code so that you suffer just as much as I did
    // from the fact that SFML figured exposing an API for lines with thickness
    // was way too complicated, and that you should Just™ use a RectangleShape,
    // despite the fact that:
    // (1.) their example to get around the lack of such an API is broken
    //      because the rotation is incorrectly offset due to the thickness of
    //      the box
    // (2.) they don't actually even bother showcasing how to infer that angle
    //      from a pair of position vectors so you have to do that yourself
    // (3.) there is apparently no vector math library in SFML so you just piss
    //      off and write all the math manually
    // (4.) using RectangleShape actually does not actually support setting the
    //      color of each vertex individually

    const sf::Vector2f diff = b - a;

    const float angle_rad = std::atan2(diff.y, diff.x);
    const float length = std::sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));

    sf::Transform transform;
    transform
        .translate(a)
        .rotate(angle_rad * (180.0 / std::numbers::pi))
        .scale(length, thickness)
        .translate(0.0f, -0.5f);

    std::array<sf::Vertex, 4> rectangle {{
        {{0.0f, 0.0f}, a_color},
        {{1.0f, 0.0f}, b_color},
        {{1.0f, 1.0f}, b_color},
        {{0.0f, 1.0f}, a_color}
    }};

    for (sf::Vertex& vtx : rectangle)
    {
        vtx.position = transform.transformPoint(vtx.position);
    };

    target.draw(rectangle.data(), rectangle.size(), sf::PrimitiveType::TriangleFan, states);
}