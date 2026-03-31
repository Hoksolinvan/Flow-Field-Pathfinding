<img src="./assets/Flow_Field_Logo.png" width="50px" height="50px">

# Flow Field PathFinding

**Description:**

This visualization illustrates the implementation of the flow field pathfinding technique commonly used in computer games. In most game implementations, NPC agents would follow a shortest distance path to get to a specific goal (e.g., players) either through the use of Dijkstra or A* shortest path algorithm. However, when there are thousands of agents with the same (or different) goals this can dramatically increase overhead as thousands of shortest path algorithms are being run at the same time.

This is where the flow field algorithm comes in, it aims to partition a plane into equally-sized grids where each tile of the grid stores summary information to the next adjacent tile that is closest towards the goal.

The flow field algorithm achieves this by making use of intermediary graphs:
- Cost Field
- Integration Field


## Cost Field

Each cell holds a **movement cost** to enter it. Low = easy terrain, high = difficult, `255` = impassable.
```
Cost Field (5×5):
+----+----+----+----+----+
|  1 |  1 | 255|  1 |  1 |
+----+----+----+----+----+
|  1 |  5 | 255|  5 |  1 |
+----+----+----+----+----+
|  1 |  1 | 255|  1 |  1 |
+----+----+----+----+----+
|  1 |  2 |  2 |  2 |  1 |
+----+----+----+----+----+
|  1 |  1 |  1 |  1 |  G |
+----+----+----+----+----+

Legend: 1 = open ground, 5 = rough terrain, 255 = wall, G = goal
```
```python
cost_field = [
    [  1,   1, 255,   1,   1],
    [  1,   5, 255,   5,   1],
    [  1,   1, 255,   1,   1],
    [  1,   2,   2,   2,   1],
    [  1,   1,   1,   1,   0],  # 0 = goal
]
```

## Integration Field

Starting from a goal it navigates through all the cell using **Dijkstra Algorithm** to iteratively/recursively propagates the accumulated weights (based off the cost-field) outwards.
```
Integration Field (5×5):
+----+----+----+----+----+
| 10 |  9 |  ∞ |  7 |  6 |
+----+----+----+----+----+
|  9 | 13 |  ∞ | 11 |  5 |
+----+----+----+----+----+
|  8 |  7 |  ∞ |  4 |  4 |
+----+----+----+----+----+
|  7 |  5 |  4 |  3 |  3 |
+----+----+----+----+----+
|  6 |  5 |  4 |  3 |  0 |
+----+----+----+----+----+

Legend: 0 = goal, ∞ = impassable (wall)
```
```python
integration_field = [
    [ 10,  9, float('inf'),  7,  6],
    [  9, 13, float('inf'), 11,  5],
    [  8,  7, float('inf'),  4,  4],
    [  7,  5,            4,  3,  3],
    [  6,  5,            4,  3,  0],  # 0 = goal
]
```

Once these intermediary graphs are pre-computed, the flow-field algorithm make use of this to find the `next_closest_tile()` for each tiles based off the computed integration field. Therefore, agents would be able to query the `next_closest_tile()` and then proceed with their course of action, rather than running it's own shortest path algorithm.


## Built With

![C++](https://img.shields.io/badge/C++-00599C?style=flat&logo=cplusplus&logoColor=white)
![GCC](https://img.shields.io/badge/GCC-A42E2B?style=flat&logo=gnu&logoColor=white)
![SDL3](https://img.shields.io/badge/SDL3-1C1C1C?style=flat&logo=sdl&logoColor=white)

## How to Build

### Prerequisites
- CMake 3.x or higher
- GCC (g++ 11 or higher)
- SDL3

### Steps
```bash
# Clone the repository
git clone https://github.com/Hoksolinvan/flow_field_pathfinding.git
cd flow_field_pathfinding

# Create build directory and compile
mkdir build && cd build
cmake ..
make

# Run
./flow_field_pathfinding
```


# Demonstration:

![video](https://www.youtube.com/watch?v=fXjrIJ7fmHI)

This implementation can support up to 100_000 different agents in a 2D plane. Automatically generates and handles obstacle during the traversal for each agents.
