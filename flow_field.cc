#include "flow_field.hpp"
#include <stdio.h>
#include <utility>
#include <stdexcept>
#include <vector>
#include <limits>
#include <queue>


GenerateFlowField::GenerateFlowField(int goal_x, int goal_y, int tile_size_x, int tile_size_y) 
        : goal_x(goal_x), goal_y(goal_y), tile_size_x(tile_size_x), tile_size_y(tile_size_y),
          matrix(tile_size_y, std::vector<Tile>(tile_size_x)) {
        
        if(tile_size_x <= 0 || tile_size_y <=0 || goal_x < 0 || goal_y < 0) throw std::invalid_argument("Tile size must be greater than 0");

    }


std::vector<std::vector<Tile>>& GenerateFlowField::Generate() {
        Generate_Cost_Field();
        Generate_Integration_Field();

        std::vector<std::vector<Tile>>* p = nullptr; 
        return p;
    }


void GenerateFlowField::Generate_Cost_Field(){
        
        for(int x =0; x < tile_size_x; x++){
            for(int y =0; y< tile_size_y; y++){
                matrix[y][x].cost_field=1;
            }
        }
    }


void GenerateFlowField::Generate_Integration_Field(){
    
    matrix[goal_y][goal_x].integration_field=0;
    std::pair<int,int> arr[4] = {{-1,0},{1,0},{0,1},{0,-1}}; 

    std::priority_queue<Node,std::vector<Node>,CompareCost> pq;
    pq.emplace(goal_x,goal_y,0);


    while(!(pq.empty())){

        auto current = pq.top();
        pq.pop();

        for(int i = 0; i < 4; i++){

            int temp_x = current.x+arr[i].first;
            int temp_y = current.y+arr[i].second;

            if((temp_x>=0 && temp_x<tile_size_x) && (temp_y>=0 && temp_y<tile_size_y)){
                auto new_cost = current.cost + matrix[temp_y][temp_x].cost_field;
                
                if(new_cost < matrix[temp_y][temp_x].integration_field){
                    matrix[temp_y][temp_x].integration_field = new_cost;
                    pq.emplace(temp_x,temp_y,new_cost);
                }
            }
            else{
                continue;
            }
        }


    }

}