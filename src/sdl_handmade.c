#include <SDL2/SDL.h>

#include "include/language_layer.h"
#include "include/sdl_buffer.h"

#include "simulate_game.c"

internal sdl_window_dimension
sdl_get_window_dimension(SDL_Window* window)
{
    sdl_window_dimension result;
    
    SDL_GetWindowSize(window, &result.width, &result.height);
    
    return result;
}

internal void
SDL_resize_texture(sdl_offscreen_buffer* buffer, 
                   SDL_Renderer* renderer, 
                   int width, int height)
{
    buffer->bytes_per_pixel = 4;
    
    if (buffer->memory)
    {
        munmap(buffer->memory, buffer->width * buffer->height * buffer->bytes_per_pixel);
    }
    
    if (buffer->width)
    {
        SDL_DestroyTexture(buffer->texture);
    }
    
    buffer->texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                        SDL_TEXTUREACCESS_STREAMING,
                                        width,
                                        height);
    
    buffer->width = width; // save copy of global_variable to calculate pitch
    buffer->height = height;
    
    // mmap is == to virtualalloc and mapviewoffile on windows
    
    buffer->memory = mmap(0,
                          width * height * buffer->bytes_per_pixel,
                          PROT_READ | PROT_WRITE,
                          MAP_PRIVATE | MAP_ANONYMOUS,
                          -1,
                          0);
}

internal void
SDL_update_window(sdl_offscreen_buffer* buffer, 
                  SDL_Window* window, SDL_Renderer* renderer)
{
    SDL_UpdateTexture(buffer->texture, 0, buffer->memory, buffer->width * 4);
    
    SDL_RenderCopy(renderer, buffer->texture, 0, 0);
    
    SDL_RenderPresent(renderer);
}

bool handle_event(SDL_Event* event)
{
    bool should_quit = false;
    
    switch(event->type)
    {
        case SDL_QUIT: 
        {
            printf("SDL_QUIT\n");
            should_quit = true;
        } break;
        
        case SDL_WINDOWEVENT: 
        {
            switch(event->window.event)
            {
                case SDL_WINDOWEVENT_SIZE_CHANGED: 
                {
                    SDL_Window* window = SDL_GetWindowFromID(event->window.windowID);
                    SDL_Renderer* renderer = SDL_GetRenderer(window);
                    printf("SDL_WINDOWEVENT_SIZE_CHANGED (%d, %d)\n", event->window.data1, event->window.data2);
                    SDL_resize_texture(&global_back_buffer, renderer, event->window.data1, event->window.data2);
                } break;
                
                case SDL_WINDOWEVENT_EXPOSED: 
                {
                    SDL_Window* window = SDL_GetWindowFromID(event->window.windowID);
                    SDL_Renderer* renderer = SDL_GetRenderer(window);
                    SDL_update_window(&global_back_buffer, window, renderer);
                } break;
                
            } break;
        }
        
    }
    
    return should_quit;
}

int main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Failed to init SDL video. Error: %s\n", SDL_GetError());
        return 1;
    }
    
    SDL_Window* window = SDL_CreateWindow("Handmade Linux", 
                                          SDL_WINDOWPOS_UNDEFINED, 
                                          SDL_WINDOWPOS_UNDEFINED,
                                          640, 
                                          480,
                                          SDL_WINDOW_RESIZABLE);
    
    if (window)
    {
        SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
        
        if (renderer)
        {
            bool running = true;
            sdl_window_dimension dimension = sdl_get_window_dimension(window);
            SDL_resize_texture(&global_back_buffer, renderer, dimension.width, dimension.height);
            
            while(running)
            {
                SDL_Event event;
                while (SDL_PollEvent(&event))
                {
                    if (handle_event(&event))
                    {
                        running = false; // break out of loop if SDL_QUIT
                    }
                }
                
                SDL_update_window(&global_back_buffer, window, renderer);
                
                draw_filled_rect(&global_back_buffer, v4(1, 0, 0, 1), v2(32, 32), v2(64, 64));
            }
        }
        else
        {
            // TODO(1337): Logging
        }
    }
    else
    {
        // TODO(1337): logging
    }
    
    SDL_Quit();
    return 0;
}
