#include <vector>        // std::vector
#include <stack>         // std::stack for DFS
#include <queue>         // std::queue for BFS
#include <cstdlib>       // rand(), srand()
#include <ctime>         // time(0)
#include <array>



std::vector<std::pair<int,int>>  terrainGenerator(int start_x, int start_y, int max_x, int max_y, int max=3);