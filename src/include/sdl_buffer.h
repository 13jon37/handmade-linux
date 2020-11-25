/* date = November 25th 2020 4:58 pm */

#ifndef SDL_BUFFER_H
#define SDL_BUFFER_H

typedef struct sdl_offscreen_buffer {
    SDL_Texture* texture;
    void* memory; 
    int width;
    int height;
    int bytes_per_pixel;
} sdl_offscreen_buffer;

typedef struct sdl_window_dimension {
    int width;
    int height;
} sdl_window_dimension;

global_variable sdl_offscreen_buffer global_back_buffer;

#endif //SDL_BUFFER_H
