#include "flow_field.hpp"
#include "arrow.hpp"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>



constexpr int dimension_x = 20;
constexpr int dimension_y = 20;
constexpr int window_x = 1000;
constexpr int window_y = 1000;
constexpr float increment_x = 1000/dimension_x;
constexpr float increment_y = 1000/dimension_y;
bool running = true;
std::pair<int,int> previous_index{0,0};

struct Grid_Cells {

    Grid_Cells(float coordinate_x, float coordinate_y, float width, float height) : rect{coordinate_x,coordinate_y,width,height}, clicked(false), obstacle(false) {}
    SDL_FRect rect;
    bool obstacle;
    bool clicked;

};



int main(int argc, char* argv[])
{

    
    std::pair<int,int> previous_index{0,0};
    SDL_Event event;
    std::vector<Grid_Cells> Cell_vector;
    std::vector<Arrow> Arrows;
    SDL_Surface* icon = IMG_Load("assets/Flow_Field_Logo.png");
    
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

    if (!icon) {
    SDL_Log("Failed to load icon: %s", SDL_GetError());
    return -1;
} else {
    SDL_SetWindowIcon(window, icon);
    SDL_DestroySurface(icon);
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

        Cell_vector[1].obstacle = true;
        Cell_vector[25].obstacle = true;
        Cell_vector[15].obstacle = true;
        Cell_vector[10].obstacle = true;
        Cell_vector[11].obstacle = true;
        Cell_vector[12].obstacle = true;
        Cell_vector[13].obstacle = true;
        Cell_vector[14].obstacle = true;
        Cell_vector[16].obstacle = true;

     
        Cell_vector[0].clicked = true;

   

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
            else if(current_cell.obstacle) SDL_SetRenderDrawColor(renderer,0,0,0,255);
            else SDL_SetRenderDrawColor(renderer, 255, 200, 0, 255);

            SDL_RenderFillRect(renderer, &current_cell.rect);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderRect(renderer, &current_cell.rect);


            SDL_SetRenderDrawColor(renderer,0,0,0,255);


            if(x==previous_index.first && y==previous_index.second) continue;


            auto [flow_x, flow_y] = flow_field[y][x].next_Tile;

            if(flow_y < y){ 
            current_arrow.draw(x*increment_x,y*increment_y,increment_x,Direction::Up);
            }
            else if(flow_y > y){ 
            current_arrow.draw(x*increment_x,y*increment_y,increment_x,Direction::Down);
            }
            else if(flow_x < x){ 
            current_arrow.draw(x*increment_x,y*increment_y,increment_x,Direction::Left);
            }
            else if(flow_x > x){    
            current_arrow.draw(x*increment_x,y*increment_y,increment_x,Direction::Right);
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