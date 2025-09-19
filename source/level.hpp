#ifndef LEVEL_HPP
#define LEVEL_HPP

#include "tile_pos.hpp"
#include "snake.hpp"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

// Forward declaration for the Snake class.
// This informs the compiler that 'Snake' is a class, without needing its
// complete definition at this point, which helps resolve circular dependencies.
class Snake;

/**
 * @brief Represents a single level within the Snaze game.
 *
 * This class manages the maze layout, tile types, food placement,
 * and interactions with the snake within a specific game level.
 */
class Level {
private:
    std::vector<std::vector<int>> m_maze; ///< Internal representation of the maze grid.
    TilePos m_spawn_loc;                  ///< The initial spawn location for the snake in this level.
    TilePos m_food_loc;                   ///< The current location of the food in the maze.

public:
    /**
     * @brief Constructor that initializes the maze with the given input.
     *
     * The `input_maze` is a vector of strings where each string represents a row
     * of the maze. Characters like ' ', '#', '.', and '&' are converted to
     * their corresponding `tile_type_e` values.
     *
     * @param input_maze A constant reference to a vector of strings representing the initial maze layout.
     */
    Level(const std::vector<std::string> &input_maze);

    /**
     * @brief Enumerates the different types of tiles that can exist in the maze.
     */
    enum tile_type_e : std::uint8_t {
        EMPTY = 0,  ///< Represents an empty, walkable space in the maze.
        WALL,       ///< Represents an impassable wall.
        INV_WALL,   ///< Represents an invisible wall: an unreachable cell.
        FOOD,       ///< Represents the food for the snake to consume.
        SNAKE_HEAD, ///< Represents the head of the snake.
        SNAKE_BODY  ///< Represents a segment of the snake's body.
    };

    /**
     * @brief Gets the number of rows in the maze.
     *
     * @return The number of rows in the maze grid.
     */
    size_t n_rows() const;

    /**
     * @brief Gets the number of columns in the maze.
     *
     * @return The number of columns in the maze grid.
     */
    size_t n_cols() const;

    /**
     * @brief Gets the type of tile at a specified position.
     *
     * @param t_pos The `TilePos` object representing the coordinates (row, col) to check.
     * @return The `tile_type_e` enum value corresponding to the tile at `t_pos`.
     */
    tile_type_e get_tile_type(TilePos t_pos) const;

    /**
     * @brief Sets the type of tile at a specified position.
     *
     * @param t_type The `tile_type_e` enum value to set the tile to.
     * @param t_pos The `TilePos` object representing the coordinates (row, col) where the tile type will be set.
     */
    void set_tile_type(tile_type_e t_type, TilePos t_pos);

    /**
     * @brief Gets the current location of the food in the maze.
     *
     * @return The `TilePos` object indicating the food's coordinates.
     */
    TilePos get_food_loc() const;
    
    /**
     * @brief Gets the initial spawn location for the snake in this level.
     *
     * @return The `TilePos` object indicating the snake's spawn coordinates.
     */
    TilePos get_spawn_loc() const { return m_spawn_loc; }

    /**
     * @brief Removes the snake's body and head from the maze grid.
     *
     * Iterates through the entire maze and resets any `SNAKE_HEAD` or `SNAKE_BODY`
     * tiles back to `EMPTY`.
     */
    void remove_snake();

    /**
     * @brief Returns a vector of all empty spaces in the maze.
     *
     * @return A `std::vector` of `TilePos` objects representing all empty locations.
     */
    std::vector<TilePos> empty_spaces() const;

    /**
     * @brief Places food at a random empty location in the maze.
     *
     * If there are no empty spaces, no food is placed. This method also
     * updates `m_food_loc` to the new food position.
     */
    void place_food();

    /**
     * @brief Checks if a given position in the maze would result in a crash.
     *
     * @param t_pos The `TilePos` object representing the coordinates to check for a crash.
     * @return True if the position is occupied by a wall or snake body, or out of bounds; false otherwise.
     */
    bool crashed(TilePos t_pos) const;
};

#endif