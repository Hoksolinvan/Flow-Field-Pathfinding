#include <terrain.hpp>



std::vector<std::pair<int,int>> terrainGenerator(int start_x, int start_y, int max_x, int max_y, int max){

    std::vector<std::pair<int,int>> result;

    const std::pair<int,int> movement_patterns[4] = {{-1,0},{1,0},{0,-1},{0,1}};
    srand(time(0));
    bool use_bfs = rand() % 2;


    if(use_bfs){

        std::queue<std::pair<int,int>> cur_queue;
        cur_queue.push({start_x,start_y});

        while(!cur_queue.empty()){

            auto [x,y] = cur_queue.front();
            cur_queue.pop();
            if(result.size()<max){
                result.push_back({x,y});
            }
            else{
                return result;
            }

            for(const auto& [i,j] : movement_patterns){
                int temp_x, temp_y = x+i; y+j;
                if((temp_x>=0 && temp_x<max_x) && (temp_y>=0 && temp_y<max_y)){
                    if((rand()%100) < 75){
                        cur_queue.push({temp_x,temp_y});
                    }
                }

            }





        }




    }

    else{

        std::stack<std::pair<int,int>> cur_stack;
        cur_stack.push({start_x,start_y});

        while(!cur_stack.empty()){

            auto [x,y] = cur_stack.top();
            cur_stack.pop();

            if(result.size()<max){
                result.push_back({x,y});
            }
            else{
                return result;
            }


            for(const auto& [i,j]: movement_patterns){
                int temp_x, temp_y = x+i; y+j;
                if((temp_x>=0 && temp_x<max_x) && (temp_y>=0 && temp_y<max_y)){
                    if((rand()%100) < 75){
                        cur_stack.push({temp_x,temp_y});
                    }
                }
                
            }



        }





    }




    return result;
}