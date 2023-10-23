#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <spiralviz/gui/vizshader.hpp>
#include <spiralviz/gui/fftdebug.hpp>
#include <spiralviz/fftstreamer.hpp>

#include <optional>

class App
{
    public:
    App();

    void show_until_closed();

    private:
    void handle_event(const sf::Event& ev);

    void update_fft(sf::Time dt);

    double m_samples_dt = 0.0;

    sf::RenderWindow m_window;

    FFTStreamer m_streamer;
    FFTHighLevelConfig m_hl_config = default_hl_config;

    VizShader m_viz;

    // Debug GUIs at the end since they refer to our fields
    FFTDebugGUI m_fft_gui;
};
