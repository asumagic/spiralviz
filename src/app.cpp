// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 sdelang

#include "SFML/Window/ContextSettings.hpp"
#include <spiralviz/app.hpp>

#include <spiralviz/audio/recorder.hpp>
#include <spiralviz/dsp/windowedfft.hpp>

#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/WindowStyle.hpp>
#include <imgui-SFML.h>
#include <imgui.h>

#include <cmath>
#include <stdexcept>
#include <GL/gl.h>

App::App() :
    m_window{
        sf::VideoMode{1280, 720},
        "spiralviz",
        sf::Style::Default,
        sf::ContextSettings{0, 0, 8} // 8x MSAA
    },
    m_viz{viz_paths_defaults},
    m_note_render{
        &m_viz.params()
    },
    m_fft_gui{
        &m_hl_config,
        &m_streamer.fft(),
        &m_streamer.recorder()
    },
    m_audio_input_gui(
        &m_streamer.recorder()
    )
{
    m_window.setFramerateLimit(240);
    if (!ImGui::SFML::Init(m_window))
    {
        throw std::runtime_error{"Failed to initialize imgui-SFML"};
    }
}

void App::show_until_closed()
{
    sf::Clock delta_clock;

    while (m_window.isOpen())
    {
        for (sf::Event ev; m_window.pollEvent(ev);)
        {
            handle_event(ev);
        }

        const sf::Time dt = delta_clock.restart();

        ImGui::SFML::Update(m_window, dt);

        show_main_bar_gui();

        update_fft(dt);

        {
            // imgui stuff -- note that this actually gets pushed on the screen
            // when ImGui::SFML::Render is called, so it's ok to do this here.
            // ImGui::ShowDemoWindow();
            m_note_render.show_controls_gui();
            m_fft_gui.show_params_gui();
            m_audio_input_gui.show_gui();
        }

        // Rendering
        // m_window.clear(); // not necessary with a fullscreen shader
        m_viz.render_into(m_window);
        m_note_render.render_into(m_window);

        ImGui::SFML::Render(m_window);

        m_window.display();
    }
}

void App::handle_event(const sf::Event& ev)
{
    ImGui::SFML::ProcessEvent(m_window, ev);

    switch (ev.type)
    {
        case sf::Event::Closed:
        {
            m_window.close();
            break;
        }

        case sf::Event::KeyPressed:
        {
            if (ev.key.code == sf::Keyboard::R)
            {
                try
                {
                    m_viz.reload_shader_from_paths();
                }
                catch(...) {}
            }

            break;
        }

        case sf::Event::Resized:
        {
            m_window.setView(sf::View{{0.0f, 0.0f, float(ev.size.width), float(ev.size.height)}});
            break;
        }

        default: break;
    }
}

void App::update_fft(sf::Time dt)
{
    m_samples_dt += double(dt.asMicroseconds()) / samples_per_us(m_streamer.recorder().getSampleRate());
    std::size_t samples_to_load = std::floor(m_samples_dt);
    m_samples_dt -= samples_to_load;

    auto fft_data = m_streamer.update_fft(samples_to_load);

    if (!fft_data.empty())
    {
        // TODO: it's kinda ugly that we only show the window here, we probably
        // should just store the FFT data somewhere...
        m_fft_gui.show_fft_gui(fft_data);
        m_viz.update_fft_texture(fft_data, m_streamer.recorder().getSampleRate());
    }
}

void App::show_main_bar_gui()
{
    ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.0, 0.0, 0.0, 0.0));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0, 0.0, 0.0, 0.0));
    ImGui::BeginMainMenuBar();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();

    ImGui::TextColored(ImVec4(1.0, 1.0, 1.0, 0.5), "Spiralviz");
    ImGui::Spacing();

    const ImVec4 heading_color(1.0, 1.0, 1.0, 0.8);

    const auto menu_bool = [](const char* name, bool* setting) {
        if (ImGui::MenuItem(name, nullptr, *setting))
        {
            *setting = !*setting;
        }
    };

    if (ImGui::BeginMenu("View"))
    {
        ImGui::TextColored(heading_color, "- Audio");
        menu_bool("Input settings", &m_audio_input_gui.params().enable_input_gui);
        ImGui::Spacing();

        ImGui::TextColored(heading_color, "- Spectrogram");
        menu_bool("FFT parameters", &m_fft_gui.params().enable_params_gui);
        menu_bool("Raw FFT view", &m_fft_gui.params().enable_fft_gui);
        ImGui::Spacing();

        ImGui::TextColored(heading_color, "- Note display");
        menu_bool("Overlay settings", &m_note_render.params().enable_controls_gui);

        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
}
