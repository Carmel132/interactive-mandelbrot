#pragma once
#include <SDL3/SDL.h>
#include <iostream>
constexpr int SCREEN_WIDTH_PX = 1920;
constexpr int SCREEN_HEIGHT_PX = 1080;

struct Window {
    SDL_Window* m_win = NULL;
    SDL_Renderer* m_renderer = NULL;

    void start();
    void init();
    void run();

    ~Window();
};