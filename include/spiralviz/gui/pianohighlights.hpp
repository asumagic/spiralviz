// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 sdelang

#pragma once

#include <SFML/Graphics.hpp>

#include <bitset>
#include <cstddef>

constexpr std::size_t piano_key_count = 88;

// TODO: make this a sf::Renderable so that it can be positioned more easily
// TODO: probably should render to a RenderTexture to embed in imgui
// TODO: also return the size? or have a global static for the max piano size

class PianoHighlights : public sf::Drawable
{
    public:
    PianoHighlights();

    PianoHighlights& with_octave_range(int first_octave, int last_octave);
    PianoHighlights& with_highlighted_keys(std::bitset<piano_key_count> highlighted_keys);

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    private:
    std::bitset<piano_key_count> m_highlighted_keys;
    int m_first_octave = 0, m_last_octave = 8;
    sf::Texture m_piano_texture;
};
