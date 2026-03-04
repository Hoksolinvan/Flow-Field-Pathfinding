#include "arrow.hpp"



Arrow::Arrow(SDL_Renderer* renderer) : renderer(renderer) {}
     

void Arrow::draw(float cell_x, float cell_y, float cell_size, Direction dir){
    float center_x = cell_x + cell_size * 0.5f;
        float center_y = cell_y + cell_size * 0.5f;

        float dx = 0.0f;
        float dy = 0.0f;

        switch (dir) {
            case Direction::Up:    dx = 0;  dy = -1; break;
            case Direction::Down:  dx = 0;  dy =  1; break;
            case Direction::Left:  dx = -1; dy =  0; break;
            case Direction::Right: dx =  1; dy =  0; break;
        }

        float length = cell_size * 0.4f;
        float head_size = cell_size * 0.2f;

        float end_x = center_x + dx * length;
        float end_y = center_y + dy * length;

        // Arrow shaft
        SDL_RenderLine(renderer,
                       center_x, center_y,
                       end_x, end_y);

        // Perpendicular vector
        float perp_x = -dy;
        float perp_y = dx;

        float left_x  = end_x - dx * head_size + perp_x * head_size;
        float left_y  = end_y - dy * head_size + perp_y * head_size;

        float right_x = end_x - dx * head_size - perp_x * head_size;
        float right_y = end_y - dy * head_size - perp_y * head_size;

        // Arrow head
        SDL_RenderLine(renderer, end_x, end_y, left_x, left_y);
        SDL_RenderLine(renderer, end_x, end_y, right_x, right_y);

    
}


