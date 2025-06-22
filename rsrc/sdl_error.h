#pragma once
#include <iostream>
#include <SDL3/SDL.h>
class SDLException final : public std::runtime_error {
    public:
    SDLException(const std::string &message) : std::runtime_error(message + '\n' + SDL_GetError()) {

    }
};