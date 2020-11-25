#include "include/language_layer.h"
#include "include/sdl_buffer.h"

#define RED_OFFSET 2
#define GREEN_OFFSET 1
#define BLUE_OFFSET 0

internal void
draw_filled_rect(sdl_offscreen_buffer* buffer, v4 color, v2 position, v2 size)
{
    i32 lower_bound_x = (i32)position.x;
    i32 lower_bound_y = (i32)position.y;
    i32 upper_bound_x = lower_bound_x + (i32)size.x;
    i32 upper_bound_y = lower_bound_y + (i32)size.y;
    i32 pixel_index = 0;
    
    u8* row = (u8*)buffer->memory;
    
    for (i32 y = lower_bound_y; y <= upper_bound_y; ++y)
    {
        for (i32 x = lower_bound_x; x <= upper_bound_x; ++x)
        {
            pixel_index = y * buffer->width + x;
            
            row[pixel_index * buffer->bytes_per_pixel +  RED_OFFSET] =   (u8)(color.r * 255.f);
            row[pixel_index * buffer->bytes_per_pixel +  GREEN_OFFSET] = (u8)(color.g * 255.f);
            row[pixel_index * buffer->bytes_per_pixel +  BLUE_OFFSET] =  (u8)(color.b * 255.f);
        }
    }
}