// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 sdelang

#include <spiralviz/gui/pianohighlights.hpp>

PianoHighlights::PianoHighlights()
{
    if (!m_piano_texture.loadFromFile("pianohighlights.png"))
    {
        throw std::runtime_error{"Failed to load piano highlights texture"};
    }
}

void PianoHighlights::render_piano(
    sf::RenderTarget& target,
    sf::Vector2f top_left,
    const std::bitset<piano_key_count>& highlighted_keys,
    int first_octave,
    int last_octave
) {
    assert(first_octave >= 0);
    assert(last_octave <= 8);
    assert(first_octave <= last_octave);

    sf::Sprite piano_sprite{m_piano_texture};

    // highlighted keys
    std::vector<sf::Sprite> highlight_sprites;

    int current_x = 0, current_key = 0;

    for (int octave = 0; octave <= 8; ++octave)
    {
        const bool is_octave_rendered = octave >= first_octave && octave <= last_octave;

        piano_sprite.setPosition(top_left + sf::Vector2f{float(current_x), 0.0f});

        if (octave == 0)
        {
            // A0/A0#/B0
            if (is_octave_rendered)
            {
                for (int i = 0; i < 3; ++i)
                {
                    if (!highlighted_keys[current_key + i]) { continue; }

                    highlight_sprites.emplace_back(
                        m_piano_texture,
                        sf::IntRect{0, 32 + i * 16, 29, 16}
                    ).setPosition({top_left.x + current_x, top_left.y});
                }
            }

            piano_sprite.setTextureRect({0, 224, 29, 16});
            current_x += 8;
            current_key += 3;
        }
        else if (octave <= 7)
        {
            // full octave
            if (is_octave_rendered)
            {
                for (int i = 0; i < 8; ++i)
                {
                    if (!highlighted_keys[current_key + i]) { continue; }

                    highlight_sprites.emplace_back(
                        m_piano_texture,
                        sf::IntRect{0, 32 + i * 16, 29, 16}
                    ).setPosition({top_left.x + current_x, top_left.y});
                }
            }

            piano_sprite.setTextureRect({0, 0, 29, 16});
            current_x += 28;
            current_key += 8;
        }
        else
        {
            // C8
            if (is_octave_rendered && highlighted_keys[current_key])
            {
                highlight_sprites.emplace_back(
                    m_piano_texture,
                    sf::IntRect{0, 256, 29, 16}
                ).setPosition({top_left.x + current_x, top_left.y});
            }

            piano_sprite.setTextureRect({0, 240, 29, 16});
            current_x += 5;
            current_key += 1;
        }

        if (is_octave_rendered)
        {
            target.draw(piano_sprite);
        }
    }

    assert(current_key == 88);

    for (sf::Sprite& highlight : highlight_sprites)
    {
        target.draw(highlight);
    }
}
