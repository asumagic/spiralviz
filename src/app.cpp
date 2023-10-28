// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 sdelang

#include "SFML/Window/ContextSettings.hpp"
#include <spiralviz/app.hpp>

#include <spiralviz/audio/recorder.hpp>
#include <spiralviz/dsp/windowedfft.hpp>
#include <spiralviz/gui/pianohighlights.hpp>

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
        &m_viz.params(),
        &m_streamer.fft(),
        &m_streamer.recorder()
    },
    m_audio_input_gui(
        &m_streamer.recorder()
    )
{
    m_window.setFramerateLimit(240);
    if (!ImGui::SFML::Init(m_window, false))
    {
        throw std::runtime_error{"Failed to initialize imgui-SFML"};
    }

    imgui_apply_theme();
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

        // static PianoHighlights ph;
        // std::bitset<piano_key_count> test_bitset;
        // for (int i = 0; i < 88; i += 2)
        // {
        //     test_bitset[i] = true;
        // }
        // ph.render_piano(m_window, sf::Vector2f(32, 32), test_bitset);

        ImGui::SFML::Render(m_window);

        m_window.display();
    }
}

void App::imgui_apply_theme()
{
    auto& IO = ImGui::GetIO();

    ImFontConfig font_config;
    // I don't know why this is less blurry... if it's rendering text at
    // subpixel positions, I'm annoyed
    font_config.OversampleH = 2;
    font_config.OversampleV = 2;

    IO.Fonts->AddFontFromFileTTF("Cantarell-Regular.ttf", 20.f, &font_config);
    if (!ImGui::SFML::UpdateFontTexture())
    {
        throw std::runtime_error("Failed to load imgui font");
    }

    // The rest of this function is based on this modified scheme:
    // https://github.com/ocornut/imgui/issues/707#issuecomment-1372640066

    auto &colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4{0.1f, 0.1f, 0.13f, 1.0f};
    colors[ImGuiCol_MenuBarBg] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};

    // Border
    colors[ImGuiCol_Border] = ImVec4{0.44f, 0.37f, 0.61f, 0.29f};
    colors[ImGuiCol_BorderShadow] = ImVec4{0.0f, 0.0f, 0.0f, 0.24f};

    // Text
    colors[ImGuiCol_Text] = ImVec4{1.0f, 1.0f, 1.0f, 1.0f};
    colors[ImGuiCol_TextDisabled] = ImVec4{0.5f, 0.5f, 0.5f, 1.0f};

    // Headers
    colors[ImGuiCol_Header] = ImVec4{0.13f, 0.13f, 0.17, 1.0f};
    colors[ImGuiCol_HeaderHovered] = ImVec4{0.19f, 0.2f, 0.25f, 1.0f};
    colors[ImGuiCol_HeaderActive] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};

    // Buttons
    colors[ImGuiCol_Button] = ImVec4{0.13f, 0.13f, 0.17, 1.0f};
    colors[ImGuiCol_ButtonHovered] = ImVec4{0.19f, 0.2f, 0.25f, 1.0f};
    colors[ImGuiCol_ButtonActive] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
    colors[ImGuiCol_CheckMark] = ImVec4{0.74f, 0.58f, 0.98f, 1.0f};

    // Popups
    colors[ImGuiCol_PopupBg] = ImVec4{0.1f, 0.1f, 0.13f, 0.92f};

    // Slider
    colors[ImGuiCol_SliderGrab] = ImVec4{0.44f, 0.37f, 0.61f, 0.54f};
    colors[ImGuiCol_SliderGrabActive] = ImVec4{0.74f, 0.58f, 0.98f, 0.54f};

    // Frame BG
    colors[ImGuiCol_FrameBg] = ImVec4{0.13f, 0.13, 0.17, 1.0f};
    colors[ImGuiCol_FrameBgHovered] = ImVec4{0.19f, 0.2f, 0.25f, 1.0f};
    colors[ImGuiCol_FrameBgActive] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
    colors[ImGuiCol_TabHovered] = ImVec4{0.24, 0.24f, 0.32f, 1.0f};
    colors[ImGuiCol_TabActive] = ImVec4{0.2f, 0.22f, 0.27f, 1.0f};
    colors[ImGuiCol_TabUnfocused] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};

    // Title
    colors[ImGuiCol_TitleBg] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
    colors[ImGuiCol_TitleBgActive] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};

    // Scrollbar
    colors[ImGuiCol_ScrollbarBg] = ImVec4{0.1f, 0.1f, 0.13f, 1.0f};
    colors[ImGuiCol_ScrollbarGrab] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4{0.19f, 0.2f, 0.25f, 1.0f};
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4{0.24f, 0.24f, 0.32f, 1.0f};

    // Seperator
    colors[ImGuiCol_Separator] = ImVec4{0.44f, 0.37f, 0.61f, 1.0f};
    colors[ImGuiCol_SeparatorHovered] = ImVec4{0.74f, 0.58f, 0.98f, 1.0f};
    colors[ImGuiCol_SeparatorActive] = ImVec4{0.84f, 0.58f, 1.0f, 1.0f};

    // Resize Grip
    colors[ImGuiCol_ResizeGrip] = ImVec4{0.44f, 0.37f, 0.61f, 0.29f};
    colors[ImGuiCol_ResizeGripHovered] = ImVec4{0.74f, 0.58f, 0.98f, 0.29f};
    colors[ImGuiCol_ResizeGripActive] = ImVec4{0.84f, 0.58f, 1.0f, 0.29f};

    // // Docking
    // colors[ImGuiCol_DockingPreview] = ImVec4{0.44f, 0.37f, 0.61f, 1.0f};

    auto &style = ImGui::GetStyle();
    style.TabRounding = 4;
    style.ScrollbarRounding = 9;
    style.WindowRounding = 7;
    style.GrabRounding = 3;
    style.FrameRounding = 3;
    style.PopupRounding = 4;
    style.ChildRounding = 4;
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

    const auto menu_bool = [](const char* name, bool* setting) {
        if (ImGui::MenuItem(name, nullptr, *setting))
        {
            *setting = !*setting;
        }
    };

    if (ImGui::BeginMenu("View"))
    {
        ImGui::SeparatorText("Audio");
        menu_bool("Input settings", &m_audio_input_gui.params().enable_input_gui);

        ImGui::SeparatorText("Spectrogram");
        menu_bool("Spectrogram settings", &m_fft_gui.params().enable_params_gui);
        menu_bool("Raw FFT view", &m_fft_gui.params().enable_fft_gui);
        ImGui::Spacing();

        ImGui::SeparatorText("Note display");
        menu_bool("Overlay settings", &m_note_render.params().enable_controls_gui);

        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
}
