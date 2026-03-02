#include "flow_field.hpp"
#include "arrow.hpp"
#include <SDL3/SDL.h>



constexpr int dimension_x = 100;
constexpr int dimension_y = 100;
constexpr int window_x = 1000;
constexpr int window_y = 1000;
constexpr float increment_x = 1000/dimension_x;
constexpr float increment_y = 1000/dimension_y;
bool running = true;
std::pair<int,int> previous_index{0,0};

struct Grid_Cells {

    Grid_Cells(float coordinate_x, float coordinate_y, float width, float height) : rect{coordinate_x,coordinate_y,width,height}, clicked(false) {}
    SDL_FRect rect;
    bool clicked;

};



int main(int argc, char* argv[])
{

    
    std::pair<int,int> previous_index{0,0};
    SDL_Event event;
    std::vector<Grid_Cells> Cell_vector;
    std::vector<Arrow> Arrows;

    auto flow_field = GenerateFlowField(previous_index.first,previous_index.second,dimension_x,dimension_y).Generate();
    
    

    


    // 1. Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
      std::cerr << "issues with initializing SDL3: "
          << SDL_GetError()
          << '\n';
       return -1;
       
    }

    // 2. Create Window
    SDL_Window* window = SDL_CreateWindow(
        "Flow_Field Visualization",
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

    for(int i =0; i< dimension_x; i++){
            for(int j=0; j < dimension_y; j++){
               
                Cell_vector.emplace_back(i*increment_x, j*increment_y, increment_x, increment_y);
                Arrows.emplace_back(renderer);

            }

        }
    
        Cell_vector[0].clicked = true;

    GenerateFlowField current = GenerateFlowField(0,0,dimension_x,dimension_y);

    

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
                    

                    // if(previous_index.first != 0 && previous_index.second != 0){
                    int temp_index = previous_index.first * dimension_y + previous_index.second;
                    Cell_vector[temp_index].clicked = false;
                    
                    flow_field = GenerateFlowField(grid_x,grid_y,dimension_x,dimension_y).Generate();
                    
                    Cell_vector[index].clicked = true;
                    previous_index = {grid_x, grid_y};
                 
                }
            }
        }



        // 5. Render
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        
    

    for(int x =0; x<dimension_x;x++){
        for(int y =0; y<dimension_y;y++){

            auto current_cell = Cell_vector[x*dimension_y+y];
            auto current_arrow = Arrows[x*dimension_y+y];

            if(current_cell.clicked) SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
            else SDL_SetRenderDrawColor(renderer, 255, 200, 0, 255);

            SDL_RenderFillRect(renderer, &current_cell.rect);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderRect(renderer, &current_cell.rect);


            SDL_SetRenderDrawColor(renderer,0,0,0,255);


            if(x==previous_index.first && y==previous_index.second) continue;


            auto [flow_first, flow_second] = flow_field[x][y].next_Tile;

            if(x-1==flow_first){
            current_arrow.draw(x*increment_x,y*increment_y,increment_x,Direction::Left);
            }
            else if(x+1==flow_first){
            current_arrow.draw(x*increment_x,y*increment_y,increment_x,Direction::Right);
            }
            else if(y-1==flow_first){
            current_arrow.draw(x*increment_x,y*increment_y,increment_x,Direction::Down);
            }
            else if {

            }

        }
    }

    


        
       

       


        SDL_RenderPresent(renderer);
    }

    // 6. Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}