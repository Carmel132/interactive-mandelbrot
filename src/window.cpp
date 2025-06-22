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

    poll_window_coordinates();
    SDL_GetMouseState(&m_mouse_pos.x, &m_mouse_pos.y);

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

void Window::poll_window_coordinates(){
    SDL_GetWindowSize(m_win, &m_window_size.x, &m_window_size.y);
}

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
                m_view.zoomAboutPoint(m_view.fromScreenCoord((int)m_mouse_pos.x, (int)m_mouse_pos.y, m_window_size.x, m_window_size.y), event.wheel.y > 0);
                m_pan.on_zoom();
            }
            else if (event.type == SDL_EVENT_WINDOW_RESIZED) {
                poll_window_coordinates();
            }
            else if (event.type == SDL_EVENT_MOUSE_MOTION) {
                m_mouse_pos.x = event.motion.x;
                m_mouse_pos.y = event.motion.y;

                m_pan.on_mouse_motion();
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                m_pan.on_mouse_down();
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                m_pan.on_mouse_up();
            }
        }

        m_pan.frame();

        SDL_GPUCommandBuffer* cmd_buf = SDL_AcquireGPUCommandBuffer(m_device);

        if (cmd_buf == nullptr) {
            throw SDLException("Failed to acquire command buffer");
            break;
        }

        push_fragment_shader_uniforms(cmd_buf, m_window_size.x, m_window_size.y, m_view);

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