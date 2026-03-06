#include "flow_field.hpp"
#include "arrow.hpp"
#include <terrain.hpp>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>



constexpr int terrain_count = 3;
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
    srand(time(0));

    auto first_obstacle = terrainGenerator(rand()%dimension_x,rand()%dimension_y,dimension_x,dimension_y,8);
    auto second_obstacle = terrainGenerator(rand()%dimension_x,rand()%dimension_y,dimension_x,dimension_y,13);
    auto third_obstacle = terrainGenerator(rand()%dimension_x,rand()%dimension_y,dimension_x,dimension_y,9);

    
    std::pair<int,int> previous_index{0,0};
    SDL_Event event;
   
    std::vector<std::vector<Grid_Cells>> Cell_vector;
    Cell_vector.resize(dimension_y);
    std::vector<std::vector<Arrow>> Arrows;
    Arrows.resize(dimension_y);
    SDL_Surface* icon = IMG_Load("assets/Flow_Field_Logo.png");
    
    GenerateFlowField flow_field = GenerateFlowField(previous_index.first,previous_index.second,dimension_x,dimension_y);
    auto matrix = flow_field.Generate();
    
    

    


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
               
                Cell_vector[i].push_back(Grid_Cells(i*increment_x,j*increment_y,increment_x,increment_y));
                Arrows[i].push_back(renderer);

            }

        }

    
    for(const auto& [x,y]: first_obstacle){
        Cell_vector[x][y].obstacle=true;
       flow_field.setMatrix_obstacle(x,y);
    }

    for(const auto& [x,y]: second_obstacle){
        Cell_vector[x][y].obstacle=true;
       flow_field.setMatrix_obstacle(x,y);
    }

    for(const auto& [x,y]: third_obstacle){
        Cell_vector[x][y].obstacle=true;
        flow_field.setMatrix_obstacle(x,y);
    }




    
     
        Cell_vector[0][0].clicked = true;

   

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
                    grid_y >= 0 && grid_y < dimension_y && !Cell_vector[grid_x][grid_y].obstacle)
                {
                    int index = grid_x * dimension_y + grid_y;
                    

                    Cell_vector[previous_index.first][previous_index.second].clicked = false;
                    
                    flow_field.setGoal(grid_x, grid_y);
                    flow_field.regenerate();

                   
                                    
                    Cell_vector[grid_x][grid_y].clicked = true;
                    previous_index = {grid_x, grid_y};
                 
                }
            }
        }



        // 5. Render
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        
    

    for(int x =0; x<dimension_x;x++){
        for(int y =0; y<dimension_y;y++){

            auto current_cell = Cell_vector[x][y]; 
            auto current_arrow = Arrows[x][y];

            if(current_cell.clicked && !current_cell.obstacle) SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
            else if(current_cell.obstacle) SDL_SetRenderDrawColor(renderer,0,0,0,255);
            else SDL_SetRenderDrawColor(renderer, 255, 200, 0, 255);

            SDL_RenderFillRect(renderer, &current_cell.rect);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderRect(renderer, &current_cell.rect);


            SDL_SetRenderDrawColor(renderer,0,0,0,255);


            if(x==previous_index.first && y==previous_index.second) continue;


            auto [flow_x, flow_y] = flow_field.nextTile(x,y);

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