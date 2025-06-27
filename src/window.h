#pragma once
#include <vector>
#include <src/gpu.h>
#include <rsrc/vec.h>
constexpr int SCREEN_WIDTH_START_PX = 1920;
constexpr int SCREEN_HEIGHT_START_PX = 1080;
constexpr SDL_Color BG_COLOR{255, 255, 255, 255};

struct Window {
    SDL_Window* m_win = nullptr;
    SDL_GPUDevice* m_device = nullptr;

    Viewport m_view;
    Vec2<int> m_window_size;
    Vec2<float> m_mouse_pos;
    Pan m_pan;
    
    bool m_update_colormap_data;
    ColormapChainChain m_colormap_chain;
    
    bool m_update_screen_data;
    SDL_GPUTexture* m_render_target_texture = nullptr;

    /// @brief Initializes window and GPU context
    void init();

    /// @brief Runs the program
    void run();

    /// @brief Polls window instance for pixel dimensions
    void poll_window_size();

    Window() : m_view{/*arbitrary view i drew up lol*/ 4.3, 9.0/16.0, {-2.4, 1.2}, &m_update_screen_data}, m_pan{&m_mouse_pos, &m_view, &m_window_size, &m_update_screen_data}, m_update_colormap_data{false},  m_colormap_chain{buildColormapChain(&m_update_colormap_data)}, m_update_screen_data{true} {};
    ~Window() = default;
};