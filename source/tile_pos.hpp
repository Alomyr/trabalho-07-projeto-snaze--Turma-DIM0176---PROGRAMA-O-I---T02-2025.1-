#ifndef TILE_POS_HPP
#define TILE_POS_HPP

#include <cstddef>

/**
 * @brief Represents a location inside the maze.
 *
 * This struct is used to store the row and column coordinates of a tile within a maze.
 * It provides a constructor for initialization and an equality operator for comparison.
 */
struct TilePos {
    size_t row; ///< The row coordinate of the tile.
    size_t col; ///< The column coordinate of the tile.

    /**
     * @brief Constructs a new TilePos object.
     *
     * @param r The row coordinate. Defaults to 0.
     * @param c The column coordinate. Defaults to 0.
     */
    TilePos(size_t r = 0, size_t c = 0) : row{r}, col{c} {}

    /**
     * @brief Overloads the equality operator (==) for TilePos objects.
     *
     * Compares two TilePos objects to check if their row and column coordinates are equal.
     *
     * @param other The other TilePos object to compare with.
     * @return True if the row and column coordinates are equal, false otherwise.
     */
    bool operator==(const TilePos& other) const {
        return row == other.row && col == other.col;
    }
};

#endif