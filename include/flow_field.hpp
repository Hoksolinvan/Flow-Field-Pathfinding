#pragma once 

#include <iostream>
#include <stdio.h>
#include <utility>
#include <stdexcept>
#include <vector>
#include <limits>
#include <queue>




struct Tile {
    int cost_field;
    float integration_field = std::numeric_limits<float>::infinity();
    std::pair<int,int> next_Tile;
    bool is_obstacle=false;
};

struct Node {

    Node(int first_x, int first_y, int first_cost): x(first_x),y(first_y),cost(first_cost) {}
    int x;
    int y;
    int cost;
};

struct CompareCost {
    bool operator()(const Node& a, const Node& b) const {
        return a.cost > b.cost;
    }
};


class GenerateFlowField {

    public:
    GenerateFlowField(int goal_x, int goal_y, int tile_size_x, int tile_size_y);

    std::vector<std::vector<Tile>>& Generate();

    ~GenerateFlowField()=default;

   


    // prints out the cost field
    void Print_Cost_Field() const;


    // prints out the integration field
   void Print_Integration_Field() const;

    // prints out the flow field
    void Print_Flow_Field() const;
    

    private:
    int goal_x=0;
    int goal_y=0;
    int tile_size_x=8;
    int tile_size_y=8;
    std::vector<std::vector<Tile>> matrix;


    // generate the current Cost Field
    void Generate_Cost_Field();
    

    // generate the current integration Field
    void Generate_Integration_Field();
    


    // generate the finalized flow field
    void Generate_Flow_Field();
   
};