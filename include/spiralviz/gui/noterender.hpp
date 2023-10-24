// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 sdelang

#pragma once

#include <spiralviz/gui/vizshader.hpp>

class NoteRender
{
    public:
    NoteRender(const VizParams* params);

    void render_into(sf::RenderTarget& target, sf::FloatRect target_rect);
    void render_into(sf::RenderTarget& target);

    private:
    // TODO: move outside of this
    void render_line(
        sf::RenderTarget& target,
        sf::Vector2f a,
        sf::Vector2f b,
        float thickness = 1.0f,
        sf::Color a_color = sf::Color::White,
        sf::Color b_color = sf::Color::White,
        const sf::RenderStates& states = sf::RenderStates::Default
    );

    const VizParams& m_params;

    sf::Font m_note_font;
    sf::Text m_text;
};