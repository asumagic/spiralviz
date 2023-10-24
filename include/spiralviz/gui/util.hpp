// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 sdelang

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

    ThickLine& with_thickness(float thickness)
    {
        m_thickness = thickness;
        return *this;
    }

    ThickLine& with_color(sf::Color a_color, sf::Color b_color)
    {
        m_origin_color = a_color;
        m_target_color = b_color;
        return *this;
    }

    ThickLine& with_color(sf::Color color)
    {
        with_color(color, color);
        return *this;
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    private:
    sf::Vector2f m_origin;
    float m_angle;
    float m_length;
    float m_thickness = 1.0f;

    sf::Color m_origin_color = sf::Color::White;
    sf::Color m_target_color = sf::Color::White;
};
