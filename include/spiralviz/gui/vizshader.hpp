// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 sdelang

#pragma once

#include <spiralviz/gui/vizutil.hpp>

#include <SFML/Graphics.hpp>

#include <span>

struct VizPaths
{
    const char* frag_path = "viz.frag.glsl";
    const char* vert_path = "viz.vert.glsl";
    const char* colormap_path = "cmap.png";
};

static constexpr VizPaths viz_paths_defaults {};

class VizShader
{
    public:
    VizShader(const VizPaths& paths);

    void render_into(sf::RenderTarget& target, sf::FloatRect target_rect);
    void render_into(sf::RenderTarget& target);

    void update_fft_texture(std::span<const float> fft_data, std::size_t sample_rate);

    VizParams& params() { return m_params; }
    const VizParams& params() const { return m_params; }

    void reload_shader_from_paths(const char* frag_path = viz_paths_defaults.frag_path, const char* vert_path = viz_paths_defaults.vert_path);
    void reload_colormap_from_path(const char* colormap_path = viz_paths_defaults.colormap_path);

    private:
    void reload_uniforms();

    sf::Texture m_fft;
    sf::Texture m_colormap;

    std::size_t m_sample_rate;

    sf::Shader m_shader;

    VizParams m_params;
};