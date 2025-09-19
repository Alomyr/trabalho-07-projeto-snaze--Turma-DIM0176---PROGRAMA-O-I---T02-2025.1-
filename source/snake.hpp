#ifndef SNAKE_HPP
#define SNAKE_HPP

#include "tile_pos.hpp"

#include <deque>
#include <optional>
#include <unordered_map>

// Forward declaration for the Level class. This informs the compiler that 'Level' is a class.
/**
 * @brief Forward declaration of the Level class.
 * 
 * Allows referencing the `Level` class before its full definition.
 */
class Level;

/**
 * @brief Enumeration representing the four orthogonal directions.
 * 
 * Defines possible movement directions: up, right, down, and left.
 */
enum class direction {
    up = 0,    /**< Up direction */
    right = 1, /**< Right direction */
    down = 2,  /**< Down direction */
    left = 3   /**< Left direction */
};


/**
 * @brief Calculates the new position by moving one unit in the specified direction.
 * 
 * @note This is the function declaration; the definition should implement the movement
 *       using the corresponding displacement arrays.
 */
TilePos move(TilePos current_pos, direction dir);


/**
 * @brief Class representing the snake in the game.
 */
class Snake {
public:
    std::deque<TilePos> body;            ///< Snake body represented as sequential positions

public:
    bool found_foods = false;            ///< Flag indicating if the snake found food
    bool collision = false;              ///< Flag indicating a collision occurred
    bool wall_collision = false;         ///< Flag indicating a collision with a wall

public:
    /// @Snake_actions
    ///@{

    void init(TilePos start_pos);                                                       ///< Initializes the snake at the starting position
    void breadthFirst_search(Level& level, TilePos start, TilePos& next_move);          ///< Breadth-first search to find path
    bool is_valid_position(const TilePos& pos, Level& level);                           ///< Checks if a position is valid to move to
    std::optional<direction> search_random(TilePos head_pos, Level& level);             ///< Searches for a valid random direction
    void found_food(TilePos& next_move, bool& found, TilePos& food_pos, std::unordered_map<size_t, TilePos>& predecessor_map, Level& level, TilePos& start); ///< Finds path to food

    ///@}
public:
    /// @Snake_in_Maze_actions
    ///@{
    
    void reset(Level& level);  ///< Resets the snake in the current level
    
    ///@}
};

#endif
