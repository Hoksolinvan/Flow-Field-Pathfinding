#include "flow_field.hpp"
#include <SDL3/SDL.h>



constexpr int dimension_x = 10;
constexpr int dimension_y = 10;


struct Grid_Cells {

    Grid_Cells(float coordinate_x, float coordinate_y, float width, float height) : rect{coordinate_x,coordinate_y,width,height}, clicked(false) {}

    SDL_FRect rect;
    bool clicked;

};



int main(int argc, char* argv[])
{

    bool global_clicked = false;
    int window_x = 1000;
    int window_y = 1000;
    float increment_x = 1000 / dimension_x;
    float increment_y = 1000 / dimension_y;
    bool running = true;
    std::pair<int,int> previous_index;
    SDL_Event event;

    std::vector<Grid_Cells> Cell_vector;
        for(int i =0; i< dimension_x; i++){
            for(int j=0; j < dimension_y; j++){
               
                Cell_vector.emplace_back(i*increment_x, j*increment_y, increment_x, increment_y);
             

            }

        }


    // 1. Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
      std::cerr << "issues with initializing SDL3: "
          << SDL_GetError()
          << '\n';
       return -1;
       
    }

    // 2. Create Window
    SDL_Window* window = SDL_CreateWindow(
        "SDL3 Boilerplate",
        window_x,
        window_y,
        0
    );

    if (!window) {
        SDL_Quit();
        std::cerr << "issues with initializing SDL3: "
          << SDL_GetError()
          << '\n';
        return -1;
    }

    // 3. Create Renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

    if (!renderer) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        std::cerr << "issues with initializing SDL3: "
          << SDL_GetError()
          << '\n';
        return -1;
    }

    

    // 4. Main Loop
    while (running) {
        // Quitting Handler
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }

            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            {
               int grid_x = event.button.x / increment_x;
                int grid_y = event.button.y / increment_y;

                if (grid_x >= 0 && grid_x < dimension_x &&
                    grid_y >= 0 && grid_y < dimension_y)
                {
                    int index = grid_x * dimension_y + grid_y;
                    Cell_vector[index].clicked = true;
                }
            }
        }



        // 5. Render
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        
        

        for (const auto& cell : Cell_vector){
              if (cell.clicked)
        SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
    else
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    SDL_RenderFillRect(renderer, &cell.rect);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderRect(renderer, &cell.rect);
        }


        
       

       


        SDL_RenderPresent(renderer);
    }

    // 6. Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}