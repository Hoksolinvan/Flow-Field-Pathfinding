#include "flow_field.hpp"


constexpr int range_of_motion = 4;

GenerateFlowField::GenerateFlowField(int goal_x, int goal_y, int tile_size_x, int tile_size_y) 
        : goal_x(goal_x), goal_y(goal_y), tile_size_x(tile_size_x), tile_size_y(tile_size_y),
          matrix(tile_size_y, std::vector<Tile>(tile_size_x)) {
        
        if(tile_size_x <= 0 || tile_size_y <=0 || goal_x < 0 || goal_y < 0) throw std::invalid_argument("Tile size must be greater than 0");

    }


std::vector<std::vector<Tile>>& GenerateFlowField::Generate() {
        Generate_Cost_Field();
        Generate_Integration_Field();
        Generate_Flow_Field();
        return matrix;
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
    std::pair<int,int> arr[range_of_motion] = {{-1,0},{1,0},{0,1},{0,-1}}; 

    std::priority_queue<Node,std::vector<Node>,CompareCost> pq;
    pq.emplace(goal_x,goal_y,0);


    while(!(pq.empty())){

        auto current = pq.top();
        pq.pop();

        for(int i = 0; i < range_of_motion; i++){

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


void GenerateFlowField::Generate_Flow_Field(){

    std::pair<int,int> arr[range_of_motion] = {{-1,0},{1,0},{0,1},{0,-1}}; 


    for(int x =0; x< tile_size_x; x++){
        for(int y =0; y <tile_size_y; y++){

            if(x==goal_x && y==goal_y) {
                matrix[y][x].next_Tile.first = goal_x;
                matrix[y][x].next_Tile.second = goal_y;
                continue; 
            }
            
            float current_min = std::numeric_limits<float>::infinity();
            std::pair<int,int> current_position = {-1,-1};

            for(int i =0; i<range_of_motion; i++){
                
                int temp_x = x+arr[i].first;
                int temp_y = y+arr[i].second;
                

            if((temp_x>=0 && temp_x<tile_size_x) && (temp_y>=0 && temp_y<tile_size_y)){
                
                auto value = matrix[temp_y][temp_x].integration_field;
              if(current_min > value){

                current_min = value;
                current_position.first = temp_x;
                current_position.second = temp_y;

              }


            }


        }

        matrix[y][x].next_Tile.first = current_position.first;
        matrix[y][x].next_Tile.second = current_position.second;



        }
    }
}


void GenerateFlowField::Print_Cost_Field() const{

    for(int x = 0; x<tile_size_x; x++){
        for(int y =0; y < tile_size_y; y++){
            std::cout << matrix[y][x].cost_field << " ";
        }

        std::cout << "\n";
    }

}

void GenerateFlowField::Print_Integration_Field() const {

    for(int x = 0; x<tile_size_x; x++){
        for(int y =0; y < tile_size_y; y++){
            std::cout << matrix[y][x].integration_field << " ";
        }

        std::cout << "\n";
    }
}



void GenerateFlowField::Print_Flow_Field() const {

    for(int x = 0; x<tile_size_x; x++){
        for(int y =0; y < tile_size_y; y++){
            std::cout << "(" << matrix[y][x].next_Tile.first << " , " << matrix[y][x].next_Tile.second << ")" << " ";
        }

        std::cout << "\n";
    }
}


