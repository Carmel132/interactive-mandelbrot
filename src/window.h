#pragma once
#include <SDL3/SDL.h>
#include <iostream>
#include <rsrc/viewport.h>
#include <src/mandelbrot.h>
#include <vector>

constexpr int SCREEN_WIDTH_PX = 1920;
constexpr int SCREEN_HEIGHT_PX = 1080;
constexpr SDL_Color BG_COLOR{255, 255, 255, 255};

struct Window {
    SDL_Window* m_win = NULL;
    SDL_Renderer* m_renderer = NULL;

    Viewport m_view;

    void start();
    void init();
    void run();
    void render() const;

    Window() : m_view{4.3, 9.0/16.0, {-2.4, 1.2}} {};
    ~Window();
};