#include <terrain.hpp>

Terrain::Terrain(SDL_Renderer* Renderer) : Renderer(Renderer) {};

void Terrain::draw(float terrain_position_x, float terrain_position_y,float width, float height){
SDL_FRect rect{terrain_position_x,terrain_position_y,width,height};
SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
 SDL_RenderFillRect(renderer, &rect);
        SDL_RenderRect(renderer, &rect);

}