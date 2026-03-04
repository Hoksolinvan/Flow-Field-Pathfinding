#pragma once
#include <SDL3/SDL.h>
#include <cmath>

class Terrain {
    Terrain(SDL_Renderer* Renderer);
    void draw(int terrain_position_x, int terrain_position_y,float width, float height);


    private:
    SDL_FRect rect;
    int terrain_position_x;
    int terrain_position_y;
    bool is_terrain;
};