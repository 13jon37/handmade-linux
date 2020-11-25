#include "include/language_layer.h"


void
draw_filled_rect(v4 color, v2 position, v2 size)
{
    i32 lower_bound_x = (i32)position.x;
    i32 lower_bound_y = (i32)position.y;
    i32 upper_bound_x = lower_bound_x + (i32)size.x;
    i32 upper_bound_y = lower_bound_y + (i32)size.y;
    i32 pixel_index = 0;
    
    for (i32 y = lower_bound_y; y <= upper_bound_y; ++y)
    {
        for (i32 x = lower_bound_x; x <= upper_bound_x; ++x)
        {
            pixel_index = y * (bitmap_width * bytes_per_pixel + x);
        }
    }
}