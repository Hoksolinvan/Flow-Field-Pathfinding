#include <flow_field.hpp>
#include <arrow.hpp>
#include <terrain.hpp>
#include <SDL3/SDL.h>
#include <vector>
#include <SDL3_image/SDL_image.h>
#include <random>
#include <algorithm>
#include <cmath>
#include <thread>


// global/static variables
constexpr int terrain_count = 3;
constexpr int dimension_x = 20;
constexpr int dimension_y = 20;
constexpr int window_x = 1000;
constexpr int window_y = 1000;
constexpr float increment_x = 1000/dimension_x;
constexpr float increment_y = 1000/dimension_y;
constexpr int particle_increment = 10;
constexpr int particle_total_count = 5000;
constexpr int total_thread_count = 10;
bool running = true;
float goal_x;
float goal_y;
std::pair<int,int> previous_index{0,0};


// random device number generator
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> distX(0, window_x - increment_x);
std::uniform_real_distribution<float> distY(0, window_y - increment_y);


/**
 * @struct Grid_Cells
 * @brief Represents a single cell in the grid-based layout 
 * 
 * Stores the cell's grid rectangle utilized for pathfinding/obstacle logic and user interaction.
 * 
 * 
 * 
 */
struct Grid_Cells {

    Grid_Cells(float coordinate_x, float coordinate_y, float width, float height) : rect{coordinate_x,coordinate_y,width,height}, clicked(false), obstacle(false) {}
    SDL_FRect rect; ///< Primitive Object for defining cell's position and size within the grid
    bool obstacle; ///< True if the current cell is an obstacle.
    bool clicked; ///< True if the cell has been selected and/or clicked by the user

};

/**
 * @struct Particle_optimized
 * 
 * @brief Struct-of-Arrays to store particle/entity movements and properties
 */
struct Particle_optimized{

    std::vector<float> position_x = std::vector<float>(particle_total_count,0.0f); ///< Vector of particles' current x position
    std::vector<float> position_y = std::vector<float>(particle_total_count,0.0f); ///< Vector of particles' current y position
    float vx=25; ///< Particle's horizontal velocity
    float vy=25; ///< Particle's vertical velocity
    float size=10; ///< Radius of the particle

};


/**
 * @brief Reset function for respawning each particles after it reaches goal point
 * 
 * @param particles Struct containing information of all particles 
 * @param matrix Double nested vector containing grid-cells information
 * @param threadId The calling thread's threadID
 * 
 * @retval None
 */
void reset_optimized(Particle_optimized& particles, const std::vector<std::vector<Tile>>& matrix,int threadId){

    // Creates random device engine per thread for 
    std::mt19937 localGen(rd() + threadId);  
    std::uniform_real_distribution<float> localDistX(0, window_x - increment_x);
    std::uniform_real_distribution<float> localDistY(0, window_y - increment_y);

    // each thread handles its own updates for batch of particles to update thread position after it reached goal destination
    for(int i=threadId*(particle_total_count/total_thread_count); i< (threadId*(particle_total_count/total_thread_count)+(particle_total_count/total_thread_count)); i++){
        float cur_x = distX(gen), cur_y = distY(gen);

        // keeps finding new position to spawn into until the grid cell tile is not an obstacle
        while(matrix[floor(cur_y/increment_y)][floor(cur_x/increment_x)].is_obstacle){
            cur_x = distX(gen); cur_y = distY(gen);
        }
        particles.position_x[i]=cur_x;
        particles.position_y[i]=cur_y;
    }

    return;
}


/**
 * @brief Updates the position of a subset of particles based on flow-field steering.
 *
 * Each particle moves toward the center of the next tile indicated by the flow
 * field. If a particle goes out of window bounds or reaches the goal cell
 * (given by previous_index), it is respawned at a random, non-obstacle
 * position within the window.
 *
 * @param particles Struct-of-arrays container holding position/velocity data for all particles.
 * @param matrix Double-nested vector of grid Tiles, used to check whether a respawn point lands on an obstacle.
 * @param flow_field Flow field graph providing the next tile a particle should move toward, given its current cell.
 * @param previous_index Grid cell coordinates (x, y) treated as the goal cell particles that reach it are respawned.
 * @param dt Time delta used to scale movement speed this frame.
 * @param start Index (inclusive) of the first particle in this thread's assigned range.
 * @param end Index (exclusive) of the last particle in this thread's assigned range.
 * @param threadId Identifier of the calling thread, used to seed a thread-local RNG uniquely.
 */
void updateParticles(Particle_optimized& particles, const std::vector<std::vector<Tile>>& matrix,
                      GenerateFlowField& flow_field, std::pair<int,int> previous_index,
                      float dt, int start, int end, int threadId)
{  
    // Thread local RNG
    std::mt19937 localGen(rd() + threadId);
    std::uniform_real_distribution<float> localDistX(0, window_x - increment_x);
    std::uniform_real_distribution<float> localDistY(0, window_y - increment_y);

    for (int i = start; i < end; i++) {

        // Determine which grid cell this particle currently occupies.
        int particle_cell_x = static_cast<int>(particles.position_x[i] / increment_x);
        int particle_cell_y = static_cast<int>(particles.position_y[i] / increment_y);
        particle_cell_x = std::clamp(particle_cell_x, 0, dimension_x - 1);
        particle_cell_y = std::clamp(particle_cell_y, 0, dimension_y - 1);


        // Determines the next tile to move forward too
        auto [cur_x, cur_y] = flow_field.nextTile(particle_cell_x, particle_cell_y);
        float target_x = cur_x * increment_x + increment_x / 2.0f;
        float target_y = cur_y * increment_y + increment_y / 2.0f;


        // moves the particle towards target using normalized vector
        float dx = target_x - particles.position_x[i];
        float dy = target_y - particles.position_y[i];
        float dist = std::sqrt(dx * dx + dy * dy);
        if (dist > 0) {
            float dir_x = dx / dist;
            float dir_y = dy / dist;
            particles.position_x[i] += dir_x * particles.vx * dt;
            particles.position_y[i] += dir_y * particles.vx * dt;
        }

        int particle_x_position = floor(particles.position_x[i]);
        int particle_y_position = floor(particles.position_y[i]);

        // Respawn particles if it goes out of bounds or reaches goal grid cell
        if (particle_x_position < 0 || particle_x_position > window_x ||
            particle_y_position < 0 || particle_y_position > window_y ||
            (particle_cell_x == previous_index.first && particle_cell_y == previous_index.second)) {

            float cx = localDistX(localGen), cy = localDistY(localGen);
            while (matrix[floor(cy / increment_y)][floor(cx / increment_x)].is_obstacle) {
                cx = localDistX(localGen); cy = localDistY(localGen);
            }
            particles.position_x[i] = cx;
            particles.position_y[i] = cy;
        }
    }
}



int main(int argc, char* argv[])
{   

    // seeds the RNG
    srand(time(0));

    // setting up SDL Window and Graphics context
    SDL_Event event;
    SDL_Surface* icon = IMG_Load("assets/Flow_Field_Logo.png");


    if (!SDL_Init(SDL_INIT_VIDEO)) {
      std::cerr << "issues with initializing SDL3: "
          << SDL_GetError()
          << '\n';
       return -1;
       
    }

    

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

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

    if (!renderer) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        std::cerr << "issues with initializing SDL3: "
          << SDL_GetError()
          << '\n';
        return -1;
    }

    // generating terrain
    auto first_obstacle = terrainGenerator(rand()%dimension_x,rand()%dimension_y,dimension_x,dimension_y,8);
    auto second_obstacle = terrainGenerator(rand()%dimension_x,rand()%dimension_y,dimension_x,dimension_y,13);
    auto third_obstacle = terrainGenerator(rand()%dimension_x,rand()%dimension_y,dimension_x,dimension_y,9);

   
    
    // Local variable for storing grid cell states
    std::vector<std::vector<Grid_Cells>> Cell_vector;
    std::vector<std::vector<Arrow>> Arrows;
    Particle_optimized current_particles;

    Cell_vector.resize(dimension_y);
    Arrows.resize(dimension_y);


    // initializing the cell grids and arrows 
    for(int i =0; i< dimension_x; i++){
            for(int j=0; j < dimension_y; j++){
               
                Cell_vector[i].push_back(Grid_Cells(i*increment_x,j*increment_y,increment_x,increment_y));
                Arrows[i].push_back(renderer);

            }

    }
    

    // initializes flow field based on obstacle
    GenerateFlowField flow_field = GenerateFlowField(previous_index.first,previous_index.second,dimension_x,dimension_y);
    

    // updating and setting up flow field cells obstacle parameter 
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

    // generates flow field
    auto matrix = flow_field.Generate();
    

    // initial 
    for(int i =0; i< particle_total_count; i++){
        float cur_x = distX(gen), cur_y = distY(gen);
        while(matrix[floor(cur_x/increment_x)][floor(cur_y/increment_y)].is_obstacle)
        {
            cur_x = distX(gen); cur_y = distY(gen);
        }

        particles.emplace_back(cur_x,cur_y);
    }


    

    // initializes current clicked cell
    Cell_vector[previous_index.first][previous_index.second].clicked = true;

    // tick counter
    Uint64 last_time = SDL_GetTicks();



    // Main Loop
    while (running) {
        Uint64 now = SDL_GetTicks();
        float dt = (now - last_time) / 1000.0f;
        last_time = now;


        // event handler
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

                    std::vector<std::thread> threadPool1;
                    
                    for(int i=0; i<total_thread_count; i++){
                        threadPool1.emplace_back(reset_optimized,std::ref(current_particles),matrix,i);
                    }

                    for(auto & t : threadPool1){
                        t.join();
                    }
                    
                 
                }
            }
        }



    
    // render the grid cells and the arrows
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



// moves the particle based on the grid information
{
    std::vector<std::thread> updateThreads;
    int chunk = particle_total_count / total_thread_count;
    for (int i = 0; i < total_thread_count; i++) {
        int start = i * chunk;
        int end = (i == total_thread_count - 1) ? particle_total_count : start + chunk;
        updateThreads.emplace_back(updateParticles, std::ref(current_particles), std::cref(matrix),
                                    std::ref(flow_field), previous_index, dt, start, end, i);
    }
    for (auto& t : updateThreads) t.join();
}


// renders particles
for (size_t i = 0; i < current_particles.position_x.size(); i++) {
    SDL_FRect rect = {
        current_particles.position_x[i],
        current_particles.position_y[i],
        current_particles.size,
        current_particles.size
    };
    SDL_RenderFillRect(renderer, &rect);
}

        SDL_RenderPresent(renderer);
    }

    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}