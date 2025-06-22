#pragma once
#include <SDL3/SDL.h>
#include <rsrc/sdl_error.h>
#include <iostream>
#include <rsrc/color.h>
#include <rsrc/viewport.h>
inline auto load_shader(
SDL_GPUDevice* device, 
const char* filename, 
SDL_GPUShaderStage stage, 
Uint32 sampler_count, 
Uint32 uniform_buffer_count, 
Uint32 storage_buffer_count, 
Uint32 storage_texture_count) -> SDL_GPUShader* {

    if(!SDL_GetPathInfo(filename, NULL)) {
        fprintf(stdout, "File (%s) does not exist.\n", filename);
        return NULL;    
    }
        
    const char* entrypoint; 
    SDL_GPUShaderFormat backend_formats = SDL_GetGPUShaderFormats(device); 
    SDL_GPUShaderFormat format = SDL_GPU_SHADERFORMAT_INVALID; 
    if (backend_formats & SDL_GPU_SHADERFORMAT_SPIRV) {
        format = SDL_GPU_SHADERFORMAT_SPIRV;
        entrypoint = "main"; 
    }

    size_t code_size; 
    void* code = SDL_LoadFile(filename, &code_size); 
    if (code == NULL) {
        fprintf(stderr, "ERROR: SDL_LoadFile(%s) failed: %s\n", filename, SDL_GetError());
        return NULL;  
    }

    SDL_GPUShaderCreateInfo shader_info = {
        .code_size = code_size,
        .code = (Uint8*)code,
        .entrypoint = entrypoint,
        .format = format,
        .stage = stage,
        .num_samplers = sampler_count,
        .num_storage_textures = storage_texture_count,
        .num_storage_buffers = storage_buffer_count,
        .num_uniform_buffers = uniform_buffer_count
    };

    SDL_GPUShader* shader = SDL_CreateGPUShader(device, &shader_info);

    if (shader == NULL) {
        fprintf(stderr, "ERROR: SDL_CreateGPUShader failed: %s\n", SDL_GetError());
        SDL_free(code); 
        return NULL;  
    }

    SDL_free(code); 
    return shader;
}


// encapsulates the vertex and fragment shaders
// Vertex shader has 0 inputs
// Fragment shader has 2 uniforms and 1 storage buffer
struct Shaders {
    SDL_GPUShader* m_shader_vertex,* m_shader_fragment;
    Shaders(SDL_GPUDevice* device) : 
    m_shader_vertex{load_shader(device, "shaders/compiled/mandelbrot.vert.spv", SDL_GPU_SHADERSTAGE_VERTEX, 0, 0, 0, 0)},
    m_shader_fragment{load_shader(device, "shaders/compiled/mandelbrot.frag.spv", SDL_GPU_SHADERSTAGE_FRAGMENT, 0, 2, 1, 0)} {
        if (m_shader_vertex == nullptr) {
            throw SDLException("failed to create vertex shader");
        }

        if (m_shader_fragment == nullptr) {
            throw SDLException("failed to create fragment shader");
        }
    }

    // Releases shaders from memory. Should only be called after created the graphics pipeline
    void free(SDL_GPUDevice* device) const {
        SDL_ReleaseGPUShader(device, m_shader_fragment);
        SDL_ReleaseGPUShader(device, m_shader_vertex);
    }
};

// encapsulates the graphics pipeline
struct Pipeline {
    SDL_GPUGraphicsPipeline* pipeline;
    Pipeline(SDL_GPUDevice* device, const Shaders& shaders, SDL_GPUTextureFormat format) {
        SDL_GPUColorTargetDescription color_target_description[] {
            {
                .format = format                
            }
        };

        SDL_GPUGraphicsPipelineCreateInfo pipeline_create_info = {
            .vertex_shader = shaders.m_shader_vertex,
            .fragment_shader = shaders.m_shader_fragment,
            .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
            .rasterizer_state = {
                .fill_mode = SDL_GPU_FILLMODE_FILL
            },
            .target_info = {
                .color_target_descriptions = color_target_description,
                .num_color_targets = 1
            }
        };

        pipeline = SDL_CreateGPUGraphicsPipeline(device, &pipeline_create_info);
    }
    void free(SDL_GPUDevice* device) const {
        SDL_ReleaseGPUGraphicsPipeline(device, pipeline);
    }

};

// creates a [SDL_GPUBuffer*] pointing to the storage buffer
// performs a copy pass and uploads colormap data to the fragment storage buffer
// this needs to be bound to the command buffer BEFORE each draw call
inline auto upload_colormap_to_fragment_storage_buffer(SDL_GPUDevice* device) -> SDL_GPUBuffer* {
    using _PaddedVec3 = _NormalizedColorF;

    SDL_GPUBufferCreateInfo buffer_create_info = {
        .usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ,
        .size = sizeof(COLORMAP)
    };

    SDL_GPUBuffer *storage_buffer = SDL_CreateGPUBuffer(device, &buffer_create_info);

    SDL_GPUCommandBuffer* cmd_buf = SDL_AcquireGPUCommandBuffer(device);

    SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(cmd_buf);

    SDL_GPUTransferBufferCreateInfo transfer_buffer_create_info = {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = sizeof(COLORMAP)
    };
    
    SDL_GPUTransferBuffer* transfer_buffer = SDL_CreateGPUTransferBuffer(device, &transfer_buffer_create_info);
    
    auto* transfer_data = (_PaddedVec3*)SDL_MapGPUTransferBuffer(device, transfer_buffer, false);
    for (int i = 0; i < sizeof(COLORMAP) / sizeof(_PaddedVec3); i++) {
        transfer_data[i] = magma[i];
    }
    SDL_UnmapGPUTransferBuffer(device, transfer_buffer);

    SDL_GPUTransferBufferLocation transfer_buffer_location = {
        .transfer_buffer = transfer_buffer,
        .offset = 0
    };

    SDL_GPUBufferRegion buffer_region = {
        .buffer = storage_buffer,
        .offset = 0,
        .size = sizeof(COLORMAP)
    };

    SDL_UploadToGPUBuffer(copy_pass, &transfer_buffer_location, &buffer_region, false);
    SDL_EndGPUCopyPass(copy_pass);
    SDL_SubmitGPUCommandBuffer(cmd_buf);
    SDL_ReleaseGPUTransferBuffer(device, transfer_buffer);

    return storage_buffer;
}

// formats and uploads uniform data to fragment shader
inline void push_fragment_shader_uniforms(SDL_GPUCommandBuffer* cmd_buf, int window_width, int window_height, const Viewport& view) {
    struct _Viewport {
        float ox, oy, w, r;
    } view_data;
    struct _Vec2 {
        float x, y;
    } window_size ;

    window_size = _Vec2{(float)window_width, (float)window_height};
    view_data = _Viewport{
        (float)view.m_offset.x,
        (float)view.m_offset.y,
        (float)view.m_width,
        (float)window_height / window_width
    };

    SDL_PushGPUFragmentUniformData(cmd_buf, 0, &view_data, sizeof(view_data));
    SDL_PushGPUFragmentUniformData(cmd_buf, 1, &window_size, sizeof(window_size));
}

// Waits for swapchain texture. Will submit command buffer if NULL and throws an SDLException
inline auto get_swapchain_texture(SDL_GPUCommandBuffer* cmd_buf, SDL_Window* window) -> SDL_GPUTexture* {
    SDL_GPUTexture* swapchain_texture;
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmd_buf, window, &swapchain_texture, nullptr, nullptr)) {
        throw SDLException("Could not acquire swapchain texture");
    }

    if (swapchain_texture == nullptr) {
        SDL_SubmitGPUCommandBuffer(cmd_buf);
        throw SDLException("Swapchain texture is NULL");
    }

    return swapchain_texture;
}

inline auto start_render_pass(SDL_GPUCommandBuffer* cmd_buf, SDL_GPUTexture* swapchain_texture) -> SDL_GPURenderPass* {
    SDL_GPUColorTargetInfo color_target_info = {
        .texture = swapchain_texture,
        .clear_color = SDL_FColor { 0.0F, 0.0F, 0.0F, 1.0F },
        .load_op = SDL_GPU_LOADOP_CLEAR,
        .store_op = SDL_GPU_STOREOP_STORE
    };

    SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(cmd_buf, &color_target_info, 1, nullptr);

    return render_pass;
}

inline void end_render_pass(SDL_GPUCommandBuffer* cmd_buf, SDL_GPURenderPass* render_pass) {
    SDL_DrawGPUPrimitives(render_pass, 3, 1, 0, 0);
    SDL_EndGPURenderPass(render_pass);
    SDL_SubmitGPUCommandBuffer(cmd_buf);
}