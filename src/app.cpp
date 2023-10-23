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
        sf::Style::Default
    },
    m_viz{viz_paths_defaults},
    m_fft_gui{
        &m_hl_config,
        &m_streamer.fft(),
        &m_streamer.recorder()
    }
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
        update_fft(dt);

        // ImGui::ShowDemoWindow();
        m_fft_gui.show_params();


        // render
        // m_window.clear(); // not necessary with a fullscreen shader
        m_viz.render_into(m_window);

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
        m_fft_gui.show_fft(fft_data);
        m_viz.update_fft_texture(fft_data, m_streamer.recorder().getSampleRate());
    }
}