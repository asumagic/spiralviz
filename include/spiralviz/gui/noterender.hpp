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

    const VizParams& m_params;

    sf::Font m_note_font;
    sf::Text m_text;
};