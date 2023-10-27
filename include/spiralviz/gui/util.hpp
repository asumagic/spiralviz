// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 sdelang

#pragma once

#include <SFML/Graphics.hpp>

class ThickLine : public sf::Drawable
{
    public:
    ThickLine(sf::Vector2f a, sf::Vector2f b)
    {
        with_points(a, b);
    }

    ThickLine(sf::Vector2f origin, float angle_rad, float length)
    {
        with_points(origin, angle_rad, length);
    }

    ThickLine& with_points(sf::Vector2f a, sf::Vector2f b);
    ThickLine& with_points(sf::Vector2f origin, float angle_rad, float length);

    ThickLine& with_thickness(float thickness);

    ThickLine& with_color(sf::Color a_color, sf::Color b_color);
    ThickLine& with_color(sf::Color color);

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    private:
    sf::Vector2f m_origin;
    float m_angle;
    float m_length;
    float m_thickness = 1.0f;

    sf::Color m_origin_color = sf::Color::White;
    sf::Color m_target_color = sf::Color::White;
};

template<class T>
sf::Vector2<T> round(sf::Vector2<T> vec)
{
    return { std::round(vec.x), std::round(vec.y) };
}
