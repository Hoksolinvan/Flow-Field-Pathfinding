#pragma once 
#include <iostream>
#include <stdio.h>
#include <utility>
#include <stdexcept>
#include <vector>
#include <limits>
#include <queue>

/**
 * @struct Tile
 * @brief Represents a single cell in the flow field grid.
 *
 * Stores the data needed across all three flow-field generation passes:
 * the cost field, integration field, and resulting direction to the next tile.
 */
struct Tile {
    int cost_field;                                                    ///< Movement cost of entering this tile (e.g. terrain cost).
    float integration_field = std::numeric_limits<float>::infinity();  ///< Accumulated cost from this tile to the goal; used to derive flow direction.
    std::pair<int,int> next_Tile;                                      ///< Grid coordinates of the neighboring tile to move to next (the computed flow direction).
    bool is_obstacle = false;                                          ///< True if this tile is impassable.
};

/**
 * @struct Node
 * @brief Represents a grid cell entry used during cost-based pathfinding traversal.
 *
 * Used as the element type for the priority queue in Dijkstra-style
 * integration field generation.
 */
struct Node {
    /**
     * @brief Constructs a node for a given grid position and accumulated cost.
     * @param first_x X coordinate of the tile.
     * @param first_y Y coordinate of the tile.
     * @param first_cost Accumulated cost associated with reaching this tile.
     */
    Node(int first_x, int first_y, int first_cost): x(first_x), y(first_y), cost(first_cost) {}

    int x;     ///< X coordinate of the tile in the grid.
    int y;     ///< Y coordinate of the tile in the grid.
    int cost;  ///< Accumulated cost of reaching this tile.
};

/**
 * @struct CompareCost
 * @brief Comparator used to order Nodes in a min-priority queue by cost.
 *
 * Ensures the priority queue pops the lowest-cost Node first
 */
struct CompareCost {
    
    bool operator()(const Node& a, const Node& b) const {
        return a.cost > b.cost;
    }
};

/**
 * @class GenerateFlowField
 * @brief Builds and manages a flow field grid used to steer particles/agents toward a goal.
 *
 * Generation happens in three passes: a cost field (per-tile movement cost
 * and obstacles), an integration field (accumulated cost-to-goal via
 * Dijkstra-style traversal), and a flow field (per-tile direction toward
 * the goal, derived from the integration field). Once generated, callers
 * can query the next tile to move toward from any given position via
 * nextTile().
 */
class GenerateFlowField {
    public:

    /**
     * @brief Constructs a flow field for a given goal position and grid tile size.
     * @param goal_x X coordinate of the goal tile.
     * @param goal_y Y coordinate of the goal tile.
     * @param tile_size_x Width of each tile in the grid.
     * @param tile_size_y Height of each tile in the grid.
     */
    GenerateFlowField(int goal_x, int goal_y, int tile_size_x, int tile_size_y);

    /**
     * @brief Runs all three generation passes (cost, integration, flow) and returns the resulting grid.
     * @return Reference to the internal grid of Tiles, now fully populated.
     */
    std::vector<std::vector<Tile>>& Generate();

    /**
     * @brief Recomputes the flow field, e.g. after obstacles or the goal have changed.
     */
    void regenerate();

    /**
     * @brief Updates the goal position and marks the field for regeneration.
     * @param new_goal_x New X coordinate of the goal tile.
     * @param new_goal_y New Y coordinate of the goal tile.
     */
    void setGoal(int new_goal_x, int new_goal_y);

    ~GenerateFlowField() = default;

    /**
     * @brief Prints the cost field to stdout, for debugging.
     */
    void Print_Cost_Field() const;

    /**
     * @brief Prints the integration field to stdout, for debugging.
     */
    void Print_Integration_Field() const;

    /**
     * @brief Prints the flow field to stdout, for debugging.
     */
    void Print_Flow_Field() const;

    /**
     * @brief Marks the tile at the given grid coordinates as an obstacle.
     * @param x X coordinate of the tile.
     * @param y Y coordinate of the tile.
     */
    void setMatrix_obstacle(int x, int y);

    /**
     * @brief Returns the next tile to move toward from the given grid position.
     * @param x X coordinate of the current tile.
     * @param y Y coordinate of the current tile.
     * @return Grid coordinates (x, y) of the next tile along the flow field.
     */
    std::pair<int,int> nextTile(int x, int y);
    
    private:
    int goal_x = 0;         ///< X coordinate of the current goal tile.
    int goal_y = 0;         ///< Y coordinate of the current goal tile.
    int tile_size_x = 8;    ///< Width of each tile in the grid.
    int tile_size_y = 8;    ///< Height of each tile in the grid.
    std::vector<std::vector<Tile>> matrix; ///< Grid of Tiles holding cost, integration, and flow data.

    /**
     * @brief Computes the cost field, assigning movement costs and marking obstacles.
     */
    void Generate_Cost_Field();
    
    /**
     * @brief Computes the integration field via cost-based traversal from the goal outward.
     */
    void Generate_Integration_Field();
    
    /**
     * @brief Derives the final flow field by choosing, per tile, the lowest-cost neighbor.
     */
    void Generate_Flow_Field();
   
};