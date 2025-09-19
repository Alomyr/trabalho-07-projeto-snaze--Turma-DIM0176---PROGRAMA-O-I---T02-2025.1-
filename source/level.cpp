#include "level.hpp"

#include <random>

/// @brief Constructor that initializes the maze with the given input.
Level::Level(const std::vector<std::string> &input_maze) {
    // Maze sizing
    m_maze.resize(input_maze.size()); //
    for (size_t i = 0; i < input_maze.size(); ++i) { //
        m_maze[i].resize(input_maze[0].size()); //
    }

    // Filling the maze
    for (size_t i{0}; i < n_rows(); ++i) {
        for (size_t j{0}; j < n_cols(); ++j) {
            switch (input_maze[i][j]) {
                case ' ':
                    m_maze[i][j] = tile_type_e::EMPTY;
                    break;
                case '#':
                    m_maze[i][j] = tile_type_e::WALL;
                    break;
                case '.':
                    m_maze[i][j] = tile_type_e::INV_WALL;
                    break;
                case '&':
                    m_spawn_loc = TilePos(i, j);
                    m_maze[i][j]=tile_type_e::SNAKE_HEAD;
                    break;
                default:
                    break;
            }
        }
    }

    // Food generation
    place_food();
}

/// @brief Gets the number of rows in the maze.
size_t Level::n_rows() const { return m_maze.size(); }

/// @brief Gets the number of columns in the maze.
size_t Level::n_cols() const { return m_maze[0].size(); }

/// @brief Gets the type of tile at a specified position.
Level::tile_type_e Level::get_tile_type(TilePos t_pos) const {
    return static_cast<tile_type_e>(m_maze[t_pos.row][t_pos.col]);
}

/// @brief Sets the type of tile at a specified position.
void Level::set_tile_type(tile_type_e t_type, TilePos t_pos) {
    m_maze[t_pos.row][t_pos.col] = t_type;
}

/// @brief Gets the current location of the food in the maze.
TilePos Level::get_food_loc() const { return m_food_loc; }

/// @brief Removes the snake's body and head from the maze grid.
void Level::remove_snake() {
    for (size_t i{0}; i < n_rows(); ++i) {
        for (size_t j{0}; j < n_cols(); ++j) {
            if (m_maze[i][j] == tile_type_e::SNAKE_HEAD 
                    or m_maze[i][j] == tile_type_e::SNAKE_BODY) {
                m_maze[i][j] = tile_type_e::EMPTY;
            }
        }
    }
}

/// @brief Returns a vector of all empty spaces in the maze.
std::vector<TilePos> Level::empty_spaces() const {
    std::vector<TilePos> vec_tiles;

    for (size_t i{0}; i < n_rows(); ++i) {
        for (size_t j{0}; j < n_cols(); ++j) {
            if (m_maze[i][j] == tile_type_e::EMPTY) {
                vec_tiles.push_back(TilePos(i,j));
            }
        }
    }

    return vec_tiles;
}

/// @brief Places food at a random empty location in the maze.
void Level::place_food() {
    auto vec_empty_spaces = empty_spaces();

    if (vec_empty_spaces.empty()) return;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, vec_empty_spaces.size() - 1);

    m_food_loc = vec_empty_spaces[dist(gen)];
    set_tile_type(tile_type_e::FOOD, m_food_loc);
}

/// @brief Checks if a given position in the maze would result in a crash.
bool Level::crashed(TilePos t_pos) const {
    if (t_pos.row >= n_rows() or t_pos.col >= n_cols()) {
        return true;
    }

    auto t_type = get_tile_type(t_pos);

    return (t_type != tile_type_e::EMPTY and t_type != tile_type_e::FOOD);
}