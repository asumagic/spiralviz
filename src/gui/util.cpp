// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 sdelang

#include <spiralviz/gui/util.hpp>

ThickLine& ThickLine::with_points(sf::Vector2f a, sf::Vector2f b)
{
    const sf::Vector2f diff = b - a;

    const float angle_rad = std::atan2(diff.y, diff.x);
    const float length = std::sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));

    return with_points(a, angle_rad, length);
}

ThickLine& ThickLine::with_points(sf::Vector2f origin, float angle_rads, float length)
{
    m_origin = origin;
    m_angle = angle_rads;
    m_length = length;

    return *this;
}

ThickLine& ThickLine::with_thickness(float thickness)
{
    m_thickness = thickness;
    return *this;
}

ThickLine &ThickLine::with_color(sf::Color a_color, sf::Color b_color)
{
    m_origin_color = a_color;
    m_target_color = b_color;
    return *this;
}

ThickLine &ThickLine::with_color(sf::Color color)
{
    with_color(color, color);
    return *this;
}

void ThickLine::draw(sf::RenderTarget& target, sf::RenderStates states) const
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

    // Transforms are combined in reverse order; ask me how I know.
    sf::Transform transform;
    transform
        .translate(m_origin)
        .rotate(m_angle * (180.0 / std::numbers::pi))
        .scale(m_length, m_thickness)
        .translate(0.0f, -0.5f);

    // 0----1
    // |    |
    // 3----2
    std::array<sf::Vertex, 4> rectangle {{
        {{0.0f, 0.0f}, m_origin_color},
        {{1.0f, 0.0f}, m_target_color},
        {{1.0f, 1.0f}, m_target_color},
        {{0.0f, 1.0f}, m_origin_color}
    }};

    for (sf::Vertex& vtx : rectangle)
    {
        vtx.position = transform.transformPoint(vtx.position);
    };

    // Rendering as a fan means we render 012 then 023
    target.draw(rectangle.data(), rectangle.size(), sf::PrimitiveType::TriangleFan, states);
}
