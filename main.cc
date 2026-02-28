#include "flow_field.hpp"
#include <SDL3/SDL.h>



constexpr int dimension_x = 8;
constexpr int dimension_y = 8;


// int main(){

//     // Create a Matrix
//     GenerateFlowField main_template(0,0,8,8);

//     main_template.Generate();


//     main_template.Print_Cost_Field();
//     std::cout << std::endl;
//     main_template.Print_Integration_Field();
//         std::cout << std::endl;
//     main_template.Print_Flow_Field();


    

//     return 0;
// }

/* primitives.c ... */

/*
 * This example creates an SDL window and renderer, and then draws some lines,
 * rectangles and points to it every frame.
 *
 * This code is public domain. Feel free to use it for any purpose!
 */
int main(int argc, char* argv[])
{
    // 1. Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
      std::cerr << "issues with initializing SDL3: "
          << SDL_GetError()
          << '\n';
       return -1;
       
    }

    int window_x = 1000;
    int window_y = 1000;

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

    int increment_x = 1000 / dimension_x;
    int increment_y = 1000 / dimension_y;

    bool running = true;
    SDL_Event event;

    // 4. Main Loop
    while (running) {
        // Quitting Handler
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }



        // 5. Render
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        
        
        for(int i =0; i< dimension_x; i++){
            for(int j=0; j < dimension_y; j++){
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                SDL_FRect rect = { static_cast<float>(i*increment_x),
                    static_cast<float>(j*increment_y), static_cast<float>(increment_x),
                    static_cast<float>(increment_y) };
                SDL_RenderFillRect(renderer,&rect);
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderRect(renderer, &rect);

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