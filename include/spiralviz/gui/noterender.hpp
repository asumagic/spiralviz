// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 sdelang

#pragma once

#include <spiralviz/gui/vizutil.hpp>

struct NoteRenderParams
{
    bool enable_controls_gui = false;
    bool enable_note_render = false;
    bool show_notes = true;
    bool show_lines = true;
    bool show_freqs = false;
    bool use_doremi = true;
};

class NoteRender
{
    public:
    NoteRender(const VizParams* params);

    void render_into(sf::RenderTarget& target, sf::FloatRect target_rect);
    void render_into(sf::RenderTarget& target);

    void show_controls_gui();

    NoteRenderParams& params() { return m_params; }
    const NoteRenderParams& params() const { return m_params; }

    private:
    const VizParams& m_viz_params;

    NoteRenderParams m_params;

    sf::Font m_note_font;
    sf::Text m_note_text;
    sf::Text m_freq_text;
};