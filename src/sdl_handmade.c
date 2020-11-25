#include <SDL2/SDL.h>

#include "include/language_layer.h"

//#include "simulate_game.c"

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

global_variable SDL_Texture* texture;
global_variable void* bitmap_memory; // backbuffer
global_variable int bitmap_width;
global_variable int bitmap_height;
global_variable int bytes_per_pixel = 4;

#define RED_OFFSET 2
#define GREEN_OFFSET 1
#define BLUE_OFFSET 0

internal void
draw_filled_rect(v4 color, v2 position, v2 size)
{
    i32 lower_bound_x = (i32)position.x;
    i32 lower_bound_y = (i32)position.y;
    i32 upper_bound_x = lower_bound_x + (i32)size.x;
    i32 upper_bound_y = lower_bound_y + (i32)size.y;
    i32 pixel_index = 0;
    
    u8* row = (u8*)bitmap_memory;
    
    for (i32 y = lower_bound_y; y <= upper_bound_y; ++y)
    {
        for (i32 x = lower_bound_x; x <= upper_bound_x; ++x)
        {
            pixel_index = y * bitmap_width + x;
            
            row[pixel_index * bytes_per_pixel +  RED_OFFSET] =   (u8)(color.r * 255.f);
            row[pixel_index * bytes_per_pixel +  GREEN_OFFSET] = (u8)(color.g * 255.f);
            row[pixel_index * bytes_per_pixel +  BLUE_OFFSET] =  (u8)(color.b * 255.f);
        }
    }
}

internal void
SDL_resize_texture(SDL_Renderer* renderer, int width, int height)
{
    if (bitmap_memory)
    {
        munmap(bitmap_memory, bitmap_width * bitmap_height * bytes_per_pixel);
    }
    
    if (bitmap_width)
    {
        SDL_DestroyTexture(texture);
    }
    
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                SDL_TEXTUREACCESS_STREAMING,
                                width,
                                height);
    
    bitmap_width = width; // save copy of global_variable to calculate pitch
    bitmap_height = height;
    
    // mmap is == to virtualalloc and mapviewoffile on windows
    
    bitmap_memory = mmap(0,
                         width * height * bytes_per_pixel,
                         PROT_READ | PROT_WRITE,
                         MAP_PRIVATE | MAP_ANONYMOUS,
                         -1,
                         0);
}

internal void
SDL_update_window(SDL_Window* window, SDL_Renderer* renderer)
{
    SDL_UpdateTexture(texture, 0, bitmap_memory, bitmap_width * 4);
    
    SDL_RenderCopy(renderer, texture, 0, 0);
    
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
                    SDL_resize_texture(renderer, event->window.data1, event->window.data2);
                } break;
                
                case SDL_WINDOWEVENT_EXPOSED: 
                {
                    SDL_Window* window = SDL_GetWindowFromID(event->window.windowID);
                    SDL_Renderer* renderer = SDL_GetRenderer(window);
                    SDL_update_window(window, renderer);
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
            int width, height;
            
            SDL_GetWindowSize(window, &width, &height);
            SDL_resize_texture(renderer, width, height);
            
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
                
                SDL_update_window(window, renderer);
                
                draw_filled_rect(v4(1, 0, 0, 1), v2(32, 32), v2(64, 64));
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
