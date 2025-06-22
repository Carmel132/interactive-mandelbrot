#include <window.h>

void Window::start() {

}

void Window::init() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cout << "SDL failed to initialize\n" << SDL_GetError() << "\n";
        return;
    }

    m_win = SDL_CreateWindow("Interactive Mandelbrot", SCREEN_WIDTH_START_PX, SCREEN_HEIGHT_START_PX, 0);
}

// TODO: Store mandelbrot, to prevent recomputing same data. Store it as {breakout idx : [pts]}
void Window::render() const {
    for (int x = 0; x < SCREEN_WIDTH_START_PX; x++) {
        for (int y = 0; y < SCREEN_HEIGHT_START_PX; y++) {

            Color color = mandelbrot_eval(m_view.fromScreenCoord(x, y, SCREEN_WIDTH_START_PX, SCREEN_HEIGHT_START_PX));
            SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, 255);
            SDL_RenderPoint(m_renderer, x, y);
        } 
    }
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






    }
}

Window::~Window() {
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_win);
}