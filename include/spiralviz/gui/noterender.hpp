// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 sdelang

#pragma once

#include <spiralviz/gui/vizutil.hpp>

enum class SeriesAnalyzerMode : int
{
    HARMONIC_SERIES = 0,
    CHORDS_BEGIN,
    MAJOR_CHORD = CHORDS_BEGIN,
    MINOR_CHORD,
    SCALES_BEGIN,
    SCALE_MAJOR = SCALES_BEGIN,
    TOTAL
};

static constexpr const char* get_series_analyzer_mode_string(SeriesAnalyzerMode mode)
{
    switch (mode)
    {
    case SeriesAnalyzerMode::HARMONIC_SERIES: return "Harmonic series";
    case SeriesAnalyzerMode::MAJOR_CHORD: return "Major chord";
    case SeriesAnalyzerMode::MINOR_CHORD: return "Minor chord";
    case SeriesAnalyzerMode::SCALE_MAJOR: return "Major scale";
    default: return "???";
    }
}

struct NoteRenderParams
{
    bool enable_controls_gui = false;

    bool enable_note_render = false;
    bool show_notes = true;
    bool show_lines = true;
    bool show_freqs = false;
    bool use_doremi = true;

    bool enable_series_analyzer = false;
    bool lock_cursor_to_notes = true;
    SeriesAnalyzerMode series_analyzer_mode = SeriesAnalyzerMode::HARMONIC_SERIES;

    int note_font_size = 20;
    int freq_font_size = 10;
};

class NoteRender
{
    public:
    NoteRender(const VizParams* params);

    void render_into(sf::RenderTarget& target, sf::FloatRect target_rect);
    void render_into(sf::RenderTarget& target);

    void render_note_indicator_into(sf::RenderTarget& target, sf::FloatRect target_rect);
    void render_series_analyzer_into(sf::RenderTarget& target, sf::FloatRect target_rect);

    void render_freq_indicator(sf::RenderTarget& target, sf::FloatRect target_rect, float frequency, float thickness, sf::Color color);

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