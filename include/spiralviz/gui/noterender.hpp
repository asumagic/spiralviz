// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 sdelang

#pragma once

#include <spiralviz/gui/vizshader.hpp>

class NoteRender
{
    public:
    NoteRender(const VizParams* params);

    void render_into(sf::RenderTarget& target, sf::FloatRect target_rect);
    void render_into(sf::RenderTarget& target);

    void show_controls_gui();

    private:
    const VizParams& m_params;

    bool m_shown = false;
    bool m_show_notes = true;
    bool m_show_lines = true;
    bool m_use_doremi = true;

    sf::Font m_note_font;
    sf::Text m_text;
};