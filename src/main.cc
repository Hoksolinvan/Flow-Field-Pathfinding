#include <flow_field.hpp>
#include <arrow.hpp>
#include <terrain.hpp>
#include <SDL3/SDL.h>
#include <vector>
#include <SDL3_image/SDL_image.h>
#include <random>
#include <algorithm>
#include <cmath>



constexpr int terrain_count = 3;
constexpr int dimension_x = 20;
constexpr int dimension_y = 20;
constexpr int window_x = 1000;
constexpr int window_y = 1000;
constexpr float increment_x = 1000/dimension_x;
constexpr float increment_y = 1000/dimension_y;
constexpr int particle_increment = 10;
constexpr int particle_total_count = 200;
bool running = true;
float goal_x;
float goal_y;
std::pair<int,int> previous_index{0,0};

enum class Color{Red};

struct Grid_Cells {

    Grid_Cells(float coordinate_x, float coordinate_y, float width, float height) : rect{coordinate_x,coordinate_y,width,height}, clicked(false), obstacle(false) {}
    SDL_FRect rect;
    bool obstacle;
    bool clicked;

};


struct Particle {
    Particle(float position_x,float position_y, Color color): position_x(position_x), position_y(position_y), color(color){}
    float position_x;
    float position_y;
    float vx=25;
    float vy=25;
    
    Color color;
    float size=10;
};


void reset(std::vector<Particle>& particles,const std::vector<std::vector<Tile>>& matrix){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distX(0, window_x - increment_x);
    std::uniform_real_distribution<float> distY(0, window_y - increment_y);

    particles.clear();

    
    for(int i =0; i< particle_total_count; i++){
        float cur_x = distX(gen), cur_y = distY(gen);
        while(matrix[floor(cur_y/increment_y)][floor(cur_x/increment_x)].is_obstacle)
        {
            cur_x = distX(gen); cur_y = distY(gen);
        }

        particles.emplace_back(cur_x,cur_y,Color::Red);

    }
    

    return;
}


int main(int argc, char* argv[])
{   
    srand(time(0));

     
    SDL_Event event;
    SDL_Surface* icon = IMG_Load("assets/Flow_Field_Logo.png");


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


    auto first_obstacle = terrainGenerator(rand()%dimension_x,rand()%dimension_y,dimension_x,dimension_y,8);
    auto second_obstacle = terrainGenerator(rand()%dimension_x,rand()%dimension_y,dimension_x,dimension_y,13);
    auto third_obstacle = terrainGenerator(rand()%dimension_x,rand()%dimension_y,dimension_x,dimension_y,9);

   
    
   
    std::vector<std::vector<Grid_Cells>> Cell_vector;
    std::vector<Particle> particles;
    Cell_vector.resize(dimension_y);
    std::vector<std::vector<Arrow>> Arrows;
    Arrows.resize(dimension_y);

    for(int i =0; i< dimension_x; i++){
            for(int j=0; j < dimension_y; j++){
               
                Cell_vector[i].push_back(Grid_Cells(i*increment_x,j*increment_y,increment_x,increment_y));
                Arrows[i].push_back(renderer);

            }

        }
    
    GenerateFlowField flow_field = GenerateFlowField(previous_index.first,previous_index.second,dimension_x,dimension_y);
    
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

    auto matrix = flow_field.Generate();
    
    

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distX(0, window_x - increment_x);
    std::uniform_real_distribution<float> distY(0, window_y - increment_y);

    for(int i =0; i< particle_total_count; i++){
        float cur_x = distX(gen), cur_y = distY(gen);
        while(matrix[floor(cur_x/increment_x)][floor(cur_y/increment_y)].is_obstacle)
        {
            cur_x = distX(gen); cur_y = distY(gen);
        }

        particles.emplace_back(cur_x,cur_y,Color::Red);
    }
    
    
     
    Cell_vector[previous_index.first][previous_index.second].clicked = true;

    
    Uint64 last_time = SDL_GetTicks();

    // 4. Main Loop
    while (running) {
        Uint64 now = SDL_GetTicks();
        float dt = (now - last_time) / 1000.0f;
        last_time = now;


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
                   
                    Cell_vector[previous_index.first][previous_index.second].clicked = false;
                    
                    flow_field.setGoal(grid_x, grid_y);
                    flow_field.regenerate();
                   
                                    
                    Cell_vector[grid_x][grid_y].clicked = true;
                    previous_index = {grid_x, grid_y};
                    reset(particles,matrix);
                 
                }
            }
        }



    

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

        SDL_SetRenderDrawColor(renderer,255,0,0,255);
        goal_x = previous_index.first * increment_x + increment_x/2;
        goal_y = previous_index.second * increment_y + increment_y/2;


        for(size_t x = 0; x < particles.size();x++){
        

        SDL_FRect rect = {particles[x].position_x, particles[x].position_y, particles[x].size, particles[x].size};
        SDL_RenderFillRect(renderer,&rect);

        int particle_cell_x = static_cast<int>(particles[x].position_x / increment_x);
        int particle_cell_y = static_cast<int>(particles[x].position_y / increment_y);
        
        particle_cell_x = std::clamp(particle_cell_x, 0, dimension_x - 1);
        particle_cell_y = std::clamp(particle_cell_y, 0, dimension_y - 1);
        
        auto [cur_x,cur_y]=  flow_field.nextTile(particle_cell_x,particle_cell_y);

        float target_x = cur_x * increment_x + increment_x / 2.0f;
        float target_y = cur_y * increment_y + increment_y / 2.0f;

        float dx = target_x - particles[x].position_x;
        float dy = target_y - particles[x].position_y;

        float dist = std::sqrt(dx * dx + dy * dy);

        if (dist > 0) {
            float dir_x = dx / dist;
            float dir_y = dy / dist;

            particles[x].position_x += dir_x * particles[x].vx * dt;
            particles[x].position_y += dir_y * particles[x].vx * dt;
        }


        


        

        int particle_x_position = floor(particles[x].position_x);
        int particle_y_position = floor(particles[x].position_y);


        if (particle_x_position < 0 || particle_x_position > window_x || particle_y_position < 0 || particle_y_position > window_y
            || (particle_cell_x == previous_index.first && particle_cell_y == previous_index.second)) {

            float cur_x = distX(gen), cur_y = distY(gen);
            while(matrix[floor(cur_y/increment_y)][floor(cur_x/increment_x)].is_obstacle)
            {
            cur_x = distX(gen); cur_y = distY(gen);
            }   


            particles[x].position_x = cur_x;
            particles[x].position_y = cur_y;
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