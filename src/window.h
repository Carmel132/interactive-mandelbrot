#pragma once
#include <vector>
#include <src/gpu.h>
constexpr int SCREEN_WIDTH_START_PX = 1920;
constexpr int SCREEN_HEIGHT_START_PX = 1080;
constexpr SDL_Color BG_COLOR{255, 255, 255, 255};

struct Window {
    SDL_Window* m_win = nullptr;
    SDL_GPUDevice* m_device = nullptr;

    Viewport m_view;

    void start();
    void init();
    void run();

    Window() : m_view{4.3, 9.0/16.0, {-2.4, 1.2}} {};
    ~Window();
};