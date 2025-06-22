#pragma once
#include <SDL3/SDL.h>
#include <iostream>
#include <rsrc/viewport.h>
#include <src/mandelbrot.h>
#include <vector>

constexpr int SCREEN_WIDTH_START_PX = 1920;
constexpr int SCREEN_HEIGHT_START_PX = 1080;
constexpr SDL_Color BG_COLOR{255, 255, 255, 255};

struct Window {
    SDL_Window* m_win = NULL;

    Viewport m_view;

    std::vector<Color> screen_data;

    void start();
    void init();
    void run();
    void render() const;

    Window() : m_view{4.3, 9.0/16.0, {-2.4, 1.2}}, screen_data{} {};
    ~Window();
};