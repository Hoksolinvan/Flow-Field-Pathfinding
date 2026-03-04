#pragma once
#include <SDL3/SDL.h>
#include <cmath>


enum class Direction {Up, Left, Down, Right};


class Arrow {
    public:

    Arrow(SDL_Renderer* renderer);

    void draw(float cell_x, float cell_y, float cell_size, Direction dir);







    private:
    SDL_Renderer* renderer;
};