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

class PianoHighlights
{
    public:
    PianoHighlights();

    void render_piano(
        sf::RenderTarget& target,
        sf::Vector2f top_left,
        const std::bitset<piano_key_count>& highlighted_keys,
        int first_octave = 0,
        int last_octave = 8
    );

    private:
    sf::Texture m_piano_texture;
};
