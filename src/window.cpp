#include <window.h>

void Window::start() {

}

void Window::init() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cout << "SDL failed to initialize\n" << SDL_GetError() << "\n";
        return;
    }

    m_win = SDL_CreateWindow("Interactive Mandelbrot", SCREEN_WIDTH_PX, SCREEN_HEIGHT_PX, 0);
    m_renderer = SDL_CreateRenderer(m_win, NULL);
}

void Window::run() {
    if (m_win == NULL) {init();}

    bool l_quit = false;
    SDL_Event l_event;
    while (!l_quit) {
        // Event
        if (SDL_PollEvent(&l_event)) {
            if (l_event.type == SDL_EVENT_QUIT) {
                l_quit = true;
            }
            else if (l_event.type == SDL_EVENT_KEY_UP && l_event.key.key == SDLK_ESCAPE) {
                l_quit = true;
            }
        }

        // Physics

        // Render

    }
}

Window::~Window() {
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_win);
}