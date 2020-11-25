#include <SDL2/SDL.h>

#include "include/language_layer.h"

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

global_variable SDL_Texture* texture;
global_variable void* bitmap_memory;
global_variable int bitmap_width;
global_variable int bitmap_height;
global_variable int bytes_per_pixel = 4;

internal void
render_weird_gradient(int blue_offset, int green_offset)
{
    int width = bitmap_width;
    int height = bitmap_height;
    
    int pitch = width * bytes_per_pixel;
    u8* row = (u8*)bitmap_memory;
    
    for (int y = 0; y < bitmap_height; ++y)
    {
        u32* pixel = (u32*)row;
        
        for (int x = 0; x < bitmap_width; ++x)
        {
            u8 blue = (x + blue_offset);
            u8 green = (y + green_offset);
            
            *pixel++ = ((green << 8) | blue);
        }
        
        row += pitch;
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
            int xoffset = 0;
            int yoffset = 0;
            
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
                
                //render_weird_gradient(xoffset, yoffset);
                SDL_update_window(window, renderer);
                //++xoffset;
                //yoffset += 2;
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
