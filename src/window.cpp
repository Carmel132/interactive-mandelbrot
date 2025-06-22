#include <window.h>

void Window::start() {

}

void Window::init() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cout << "SDL failed to initialize\n" << SDL_GetError() << "\n";
        return;
    }

    m_win = SDL_CreateWindow("Interactive Mandelbrot", SCREEN_WIDTH_START_PX, SCREEN_HEIGHT_START_PX, 0);
    if (m_win == nullptr) {
        throw SDLException("Failed to create window");
    }

    m_device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, nullptr);
    if (m_device == nullptr) {
        throw SDLException("Failed to create GPU device");
    }

    const char* device_driver = SDL_GetGPUDeviceDriver(m_device);
    std::cout << "Created device with driver " << device_driver << "\n";

    if (!SDL_ClaimWindowForGPUDevice(m_device, m_win)) {
        throw SDLException("Failed to bind GPU device to window");
    }
}

// TODO: Store mandelbrot, to prevent recomputing same data. Store it as {breakout idx : [pts]}
// void Window::render() const {
//     for (int x = 0; x < SCREEN_WIDTH_START_PX; x++) {
//         for (int y = 0; y < SCREEN_HEIGHT_START_PX; y++) {

//             Color color = mandelbrot_eval(m_view.fromScreenCoord(x, y, SCREEN_WIDTH_START_PX, SCREEN_HEIGHT_START_PX));
//             SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, 255);
//             SDL_RenderPoint(m_renderer, x, y);
//         } 
//     }
// }

void Window::run() {
    // ensures [init()] has been called
    if (m_win == nullptr) {init();}

    Shaders shaders{m_device};
    Pipeline pipeline{m_device, shaders, SDL_GetGPUSwapchainTextureFormat(m_device, m_win)};
    shaders.free(m_device);

    SDL_GPUBuffer* storage_buffer = upload_colormap_to_fragment_storage_buffer(m_device);

    bool quit = false;
    SDL_Event event;
    while (!quit) {
        // Event
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                quit = true;
            }
            else if (event.type == SDL_EVENT_KEY_UP && event.key.key == SDLK_ESCAPE) {
                quit = true;
            }
            else if (event.type == SDL_EVENT_MOUSE_WHEEL) {
                float m_x, m_y;
                int w_x, w_y;
                SDL_GetWindowSize(m_win, &w_x, &w_y);
                SDL_GetMouseState(&m_x, &m_y);
                std::cout << m_x << ", " << m_y << "\n";
                m_view.zoomAboutPoint(m_view.fromScreenCoord(m_x, m_y, w_x, w_y), (double)event.wheel.y > 0);
            }
        }

        SDL_GPUCommandBuffer* cmd_buf = SDL_AcquireGPUCommandBuffer(m_device);

        if (cmd_buf == nullptr) {
            throw SDLException("Failed to acquire command buffer");
            break;
        }

        int window_width;
        int window_height;

        SDL_GetWindowSize(m_win, &window_width, &window_height);

        push_fragment_shader_uniforms(cmd_buf, window_width, window_height, m_view);

        SDL_GPURenderPass* render_pass = start_render_pass(cmd_buf, get_swapchain_texture(cmd_buf, m_win));

        SDL_BindGPUFragmentStorageBuffers(render_pass, 0, &storage_buffer, 1);

        SDL_BindGPUGraphicsPipeline(render_pass, pipeline.pipeline);

        end_render_pass(cmd_buf, render_pass);
    }

    pipeline.free(m_device);
    SDL_ReleaseGPUBuffer(m_device, storage_buffer);

    SDL_ReleaseWindowFromGPUDevice(m_device, m_win);
    SDL_DestroyWindow(m_win);
    SDL_DestroyGPUDevice(m_device);
    SDL_Quit();
}

Window::~Window() {

}