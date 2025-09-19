
#include "snake.hpp"
#include "level.hpp"
#include "SnazeSimulation.hpp"
#include "tile_pos.hpp"

#include <algorithm>
#include <deque> 
#include <optional>
#include <queue>
#include <random>
#include <unordered_map>
#include <vector>




/**
* @brief Initializes the snake with the starting head position.
* 
* This function clears any previous body of the snake and positions the head
* at the provided starting position.
* 
* @param start_pos Starting position for the snake's head.
*/
void Snake::init(TilePos start_pos) {
    body.clear();
    body.push_front(start_pos);
}
/**
* @brief Reconstructs the food's path to the start to define the next move.
* 
* Given the search result (the `main` map that maps each position to the previous one),
* this function traverses the path from the food's position to the starting position (`start`)
* to determine the snake's next move.
* 
* @param next_move Reference to store the snake's next move.
* @param achei Flag indicating whether the food was found (true) or not (false).
* @param food_pos Position of the food found in the maze.
* @param main Map that relates each position to the previous position in the path.
* @param level Reference to the current level (used for map dimensions).
* @param start Starting position (snake's head) from which the search begins. * 
* @note If the food was not found (`found == false`), the next move will be the current position (start).
*/
void Snake::found_food(TilePos& next_move, bool& achei, TilePos& food_pos, std::unordered_map<size_t, TilePos>& main,Level& level, TilePos& start){
        if(achei){
        TilePos curr=food_pos;

        if (main.find(food_pos.row * level.n_cols() + food_pos.col) == main.end()) {
            next_move = food_pos;
            return;
        }

        while (true) {
            TilePos anterior = main[curr.row * level.n_cols() + curr.col];
            if (anterior.row == start.row and anterior.col == start.col) {
                next_move = curr;
                return;
            }
            curr = anterior;
        }
    }else {
        next_move = start;
    }
}

/**
* @brief Checks if a position is valid for the snake to move.
* 
* The function checks if the passed position is not a wall and also
* if it is not occupied by any segment of the snake's body.
* 
* @param pos Position to be checked.
* @param level Reference to the current level, to query the tile type.
* 
* @return true If the position is valid for movement (not a wall and not a body).
* @return false Otherwise.
* 
* @note The function assumes that the position is within the level's boundaries.
*/
bool Snake::is_valid_position(const TilePos& pos, Level& level) {
    if (level.get_tile_type(pos) == Level::WALL) {
        return false;
    }

    for (const TilePos& segment : body) {
        if (segment == pos) {
            return false;
        }
    }

    return true;
}
/**
* @brief Randomly searches for a valid direction to move the snake.
* 
* The function shuffles the four possible directions (up, right, down, left)
* and returns the first direction that leads to a valid position within the level.
* If no valid direction is found, it returns std::nullopt.
* 
* @param head_pos Current position of the snake's head.
* @param level Reference to the current level, used to validate positions.
* 
* @return std::optional<direction> The valid direction found, or std::nullopt if none.
* 
* @note The function uses a static random number generator to ensure
* variation in attempts between calls.
*/
std::optional<direction> Snake::search_random(TilePos head_pos, Level& level) {
    // Vector with possible directions
    std::vector<direction> directions = {
        direction::up, direction::right, direction::down, direction::left
    };

     ///@{ Static generators for consistent randomness
    static std::random_device rd;
    static std::mt19937 gen(rd());
    ///}@

    std::shuffle(directions.begin(), directions.end(), gen);

    for (direction dir : directions) {
        TilePos next = move(head_pos, dir);
        if (is_valid_position(next, level)) {
            return dir; //Valid direction found
        }
        
    }

    return std::nullopt; // No valid address found
}

/**
* @brief Returns the current state of the game simulation.
* 
* This function provides the current internal state, which can indicate whether the game is in progress,
* whether the snake is thinking/moving, or whether the game is over, among other states defined
* in the `states` enumeration.
* 
* @return states Current state of the simulation.
*/
states SnazeSimulation::get_states()const{
return current_state;
}

int drow[] = {-1,0,1,0}; ///Values used to navigate up, right, down, and left, respectively, for rows in the four orthogonal directions.
int dcol[] = {0,1,0,-1}; ///Values used to navigate up, right, down, and left, respectively, for columns in the four orthogonal directions.

/**
* @brief Calculates the new position from the current position and given direction.
* 
* Uses the `drow` and `dcol` offset arrays to move one position
* in the specified direction.
* 
* @param current_pos Current position (row and column).
* @param dir Direction of movement (up, right, down, left).
* 
* @return TilePos New position after movement.
* 
* @note The `dir` values must be within the valid range to index
* the `drow` and `dcol` arrays.
*/
TilePos move(TilePos current_pos, direction dir){
    return TilePos(current_pos.row + drow[static_cast<int>(dir)], current_pos.col + dcol[static_cast<int>(dir)]);
}


/**
* @brief Performs a breadth-first search (BFS) to find the path to the food.
* 
* The function performs a breadth-first search from the starting position (`start`)
* to try to find the food's position in the maze (`level`). If the food
* is found, it defines the next move (`next_move`) based on the shortest route.
* If no route is found, it delegates control to an alternative strategy
* (via `SnazeSimulation::switch()`).
* 
* @param level Reference to the current game level, containing the maze.
* @param start Current position of the snake's head.
* @param next_move Reference to where the next calculated move will be stored.
* 
* @note The function modifies the internal state of `found_foods` and `collision`. If the food
* is reached directly, it also signals this to `SnazeSimulation` with
* `input_colision(true, ...)`.
*/
void Snake::breadthFirst_search(Level& level, TilePos start,TilePos& next_move) {
    std::queue<TilePos> fila;
    std::vector<std::vector<bool>> visit(level.n_rows(), std::vector<bool>(level.n_cols(), false)); /// Marks visited positions in the maze
    std::unordered_map<size_t, TilePos> main; // Maps each position to the previous position, used to reconstruct the path
    TilePos food_pos;

    SnazeSimulation& sin = SnazeSimulation::getInstance();
    if (sin.get_states() != states::SNAKE_THINKING) return;

    found_foods = false;
    collision = false;

    /// Starts the BFS at the current position
    fila.push(start);
    visit[start.row][start.col]=true;

    while(not fila.empty()){
        TilePos curr = fila.front();
        fila.pop();

        /// Only perform the search if the snake is in the appropriate state
        if(level.get_tile_type(curr)==Level::FOOD){
            food_pos = curr;
            found_foods =true;
            break;
        }

        for(int i=0;i<4;++i){
            TilePos V(curr.row+drow[i], curr.col+dcol[i]);

            if(V.row<level.n_rows() and V.col<level.n_cols() and not visit[V.row][V.col] and not level.crashed(V)){// Check if it is within bounds, unvisited and collision-free
                visit[V.row][V.col]=true;
                fila.push(V);

                main[V.row*level.n_cols()+V.col]=curr;

                if (level.crashed(V)) {
                    sin.input_colision(false, collision);
                    return;
                }
            }
        }
    }
    if(fila.empty()){   /// If it didn't find a path, change it to random.
        sin.troca();
    }
    if (found_foods) {
        found_food(next_move, found_foods, food_pos, main, level, start);
        if (next_move == level.get_food_loc()) {
            SnazeSimulation& sin = SnazeSimulation::getInstance();
            sin.input_colision(true, collision); // found food
        }   
    }
} 

/**
* @brief Sets the snake's next direction based on a random search.
* 
* The function attempts to find a valid direction using the snake's search function
* (`search_random`). If a valid direction is found, it is used to
* calculate the snake's next position. Otherwise, it is treated as if the food
* were unreachable (or blocked).
* 
* @note If no direction is found, the function triggers a "logical collision"
* with the food via `input_colision(false, true)`. This does not represent a physical collision
* but rather the end of valid movement logic.
*/
void SnazeSimulation::troca(){
    std::optional<direction> dir_opt = snake_obj.search_random(head_pos, levels[current_level_index]);/// Search for a random feasible direction from the current position
        if (dir_opt.has_value()) {
            next_dir = dir_opt.value();
            next_pos = move(head_pos, next_dir);
        } else {
            input_colision(false, true); // not found food
        }
}

/**
* @brief Updates the snake's position and state within the simulation.
* 
* This function moves the snake toward the next previously calculated position.
* Checks if the snake collided with something or ate food. If it ate, the snake grows;
* otherwise, its tail is removed. The function also updates the game state,
* marking GAME_OVER if there was a collision.
* 
* @note This function assumes that `next_pos` and `next_dir` have already been set
* correctly before calling. It only executes the movement if the current
* state is SNAKE_THINKING.
*/
void SnazeSimulation::snake_update(){
   
    bool comeu = (levels[current_level_index].get_tile_type(next_pos) == Level::tile_type_e::FOOD);
    
    if (current_state != states::SNAKE_THINKING) return;

    if (levels[current_level_index].crashed(next_pos)) {
        current_state = states::GAME_OVER;
        return;
    }

        // Move the snake's head to the new position
        levels[current_level_index].set_tile_type(Level::tile_type_e::SNAKE_HEAD, next_pos);
        snake_obj.body.push_front(next_pos);

        if(not comeu){
            TilePos tail = snake_obj.body.back();
            snake_obj.body.pop_back();
            levels[current_level_index].set_tile_type(Level::tile_type_e::EMPTY, tail);

        }else {
            levels[current_level_index].place_food();
        }

        if (snake_obj.body.size() > 1) {
            levels[current_level_index].set_tile_type(Level::tile_type_e::SNAKE_BODY, snake_obj.body[1]);
        }

    head_pos = next_pos;
    dir = next_dir;
}

/**
* @brief Resets the snake's state at the given level.
* 
* This function resets the snake's position to the starting position defined
* by the level, cleans the snake's body, resets the collision and food
* retrieval states, and marks the new head position in the maze.
* 
* @param level Reference to the Level object containing the current maze.
* 
* @note This function assumes that the level is already loaded correctly
* and that the spawn position returned by level.get_spawn_loc() is valid.
*/
void Snake::reset(Level& level){

    body.clear();    // Remove all body segments from the previous snake
    TilePos start_pos = level.get_spawn_loc();
    body.push_back(start_pos);
    level.set_tile_type(Level::SNAKE_HEAD, start_pos);
   
    // Resets food found and collision states
    found_foods = false;
    collision = false;

}