#include "SnazeSimulation.hpp"
#include "level.hpp"
#include "snake.hpp"

/**
 * @brief Advances to the next level or ends the game if no more levels remain.
 * 
 * If there is a next available level, the function updates the current level index,
 * resets the snake, resets the food, redraws the maze, and respawns objects.
 * Otherwise, it sets the game state to GAME_WON.
 * 
 * @note Assumes the `levels` vector is pre-loaded and that the called methods (`reset`, 
 *       `reset_food`, `respawn`) correctly reset the new level's elements.
 */
void SnazeSimulation::level_up() {
    if (current_level_index + 1 < static_cast<int>(levels.size())) {
        ++current_level_index;

        snake_obj.reset(levels[current_level_index]); // Reset the snake on the new level
        reset_food();
        print_maze_in_lv();
        respawn();

        current_state = states::START_SCREEN;
    } else {
        current_state = states::GAME_WON;
    }
}

/**
 * @brief Returns the singleton instance of the Snaze simulation.
 * 
 * This function implements the singleton pattern, ensuring that there is only one
 * instance of `SnazeSimulation` throughout the program's execution.
 * 
 * @return SnazeSimulation& Reference to the single instance of the simulation.
 */
SnazeSimulation& SnazeSimulation::getInstance() {
    static SnazeSimulation instance;
    return instance;
}

/**
 * @brief Updates the game score based on collected food.
 * 
 * The function increments the current score by adding 20 points for each unit
 * of food collected so far (`current_food`).
 */
void SnazeSimulation::update_score() {
    score += 20 * current_food;
}

/**
 * @brief Resets the food collected counter in the simulation.
 * 
 * This function zeroes the variable tracking the current amount of collected food.
 */
void SnazeSimulation::reset_food() {
    current_food = 0;
}



/**
 * @brief Prints a visual representation of the maze to the console.
 * 
 * This function translates the current state of the maze into ASCII characters and prints
 * it line by line, allowing visualization of the map with symbols for walls, food,
 * snake body and head, empty spaces, etc.
 * 
 * @note This function was used for testing and visualizing maze creation.
 *       It is currently not used during the project's runtime.
 * The symbols used for each tile type are:
 *       - ' ' (space): empty tile (EMPTY)
 *       - '#'        : wall (WALL)
 *       - '.'        : invisible wall (INV_WALL)
 *       - '*'        : food (FOOD)
 *       - '@'        : snake head (SNAKE_HEAD)
 *       - 'o'        : snake body (SNAKE_BODY)
 */
// void Level::trad_maze() const {
//     std::cout << "--- Maze State ---" << '\n';
//     for (size_t i = 0; i < n_rows(); ++i) {
//         for (size_t j = 0; j < n_cols(); ++j) {
//             tile_type_e type = get_tile_type(TilePos(i, j));
//             switch (type) {
//                 case EMPTY:      std::cout << ' '; break;
//                 case WALL:       std::cout << '#'; break;
//                 case INV_WALL:   std::cout << '.'; break;
//                 case FOOD:       std::cout << '*'; break;
//                 case SNAKE_HEAD: std::cout << '@'; break;
//                 case SNAKE_BODY: std::cout << 'o'; break;
//                 default:         break;
//             }
//         }
//         std::cout << '\n';
//     }
//     std::cout << "------------------" << '\n';
// }
