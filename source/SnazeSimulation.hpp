#ifndef SIMULATION_HPP
#define SIMULATION_HPP  

#include "level.hpp"
#include "snake.hpp"
#include "tile_pos.hpp"

/// @brief Enumerates the possible states of the Snaze game simulation.
enum class states {
    START = 0,      ///< Initial state of the simulation.
    WELCOME,        ///< Displays the welcome screen.
    START_SCREEN,   ///< Displays the start game screen, waiting for user input.
    SNAKE_THINKING, ///< The snake AI is calculating its next move.
    GAME_RUNNING,   ///< The game is actively running.
    SNAKE_CRASHED,  ///< The snake has crashed into a wall or itself.
    LEVEL_UP,       ///< The current level has been completed.
    GAME_WON,       ///< All levels have been completed, and the game is won.
    GAME_OVER       ///< The game has ended (the snake ran out of lives).
};

/// @brief Enumerates the types of AI players available for the snake.
enum class player_type_e {
    RANDOM = 0,
    BACKTRACKING
};

/**
 * @brief The main simulation class for the Snaze game.
 *
 * This class implements the Singleton design pattern to ensure only one instance
 * of the game simulation exists. It manages the game states, levels, snake behavior,
 * score, lives, and overall game flow.
 */
class SnazeSimulation{
private:
    /**
     * @brief Private constructor to enforce the Singleton pattern.
     *
     * @details Initializes the current_state to START.
     */
    SnazeSimulation();

    // Prevent copying and assignment to ensure a single instance.
    SnazeSimulation(const SnazeSimulation&) = delete;             ///< Deleted copy constructor.
    SnazeSimulation& operator=(const SnazeSimulation&) = delete;  ///< Deleted assignment operator.

    states current_state;          ///< The current state of the game simulation.
    std::vector<Level> levels;     ///< Collection of game levels.
    Snake snake_obj;               ///< The snake object controlled by the simulation.
    TilePos head_pos;              ///< The current position of the snake's head.
    direction dir;                 ///< The current direction of the snake.
    direction next_dir=dir;        ///< The next intended direction of the snake.
    TilePos next_pos = head_pos;   ///< The next calculated position for the snake's head.
    int score = 0;                 ///< The player's current score.

    // Initialization variables with default values
    int fps = 10;        ///< Frames per second for the simulation. Defaults to 10.
    int n_lives = 5;     ///< Total number of lives for the player. Defaults to 5.
    int n_food = 10;     ///< Total amount of food to be collected per level. Defaults to 10.
    player_type_e player_type = player_type_e::BACKTRACKING;  ///< The type of AI controlling the snake. Defaults to BACKTRACKING.

    int current_level_index = 0;   ///< The index of the current active level.
    int current_life = n_lives;    ///< The current number of remaining lives.
    int current_food = 0;          ///< The amount of food collected in the current level.

public:
    /**
     * @brief Gets the single instance of the SnazeSimulation class.
     *
     * @return A reference to the single SnazeSimulation instance.
     */
    static SnazeSimulation& getInstance();

    /**
     * @brief Initializes the simulation based on command line arguments.
     *
     * @param argc The number of command-line arguments.
     * @param argv An array of command-line argument strings.
     */
    void initialize(int argc, char* argv[]);

    /**
     * @brief Checks if the game simulation is over.
     *
     * @details The game is considered over when the current state is GAME_OVER.
     *
     * @return True if the game is over, false otherwise.
     */
    bool is_over();

    /**
     * @brief Processes events based on the current game state.
     */
    void process_events();

    /**
     * @brief Updates the game state based on the current game state.
     */
    void update();

    /**
     * @brief Renders the game elements to the screen.
     */
    void render();

    /**
     * @brief Updates the player's score.
     *
     * Increments the score, typically when the snake eats food.
     */
    void update_score();

    /**
     * @brief Resets the food count for the current level.
     *
     * Called when a new level starts or after certain game events.
     */
    void reset_food();

    /**
     * @brief Updates the snake's position and state.
     *
     * This method is responsible for moving the snake and handling its internal logic.
     */
    void snake_update();

    /**
     * @brief Parses a file to load level data.
     *
     * Reads maze layouts and other level-specific information from the specified file.
     *
     * @param file_path The path to the level file.
     */
    void parse_file(const char* file_path);

    /**
     * @brief Advances the game to the next level.
     *
     * Handles the logic for transitioning between levels, such as resetting the snake
     * and loading the next maze.
     */
    void level_up();

    /**
     * @brief Processes user input.
     *
     * Handles keyboard input for navigation or game actions depending on the state.
     */
    void input_process();

    /**
     * @brief Processes the outcome of collisions.
     *
     * Determines the next game state based on whether the snake found food or collided.
     *
     * @param food True if the snake found food.
     * @param collision True if the snake collided with an obstacle or itself.
     */
    void input_colision(bool food, bool colision);

    /**
     * @brief Prints the welcome screen to the console.
     */
    void print_welcome();

    /**
     * @brief Prints the current maze to the console.
     */
    void print_maze();

    /**
     * @brief Prints the maze, likely for a specific level.
     *
     * This method might be a variation of `print_maze()` specific to `level` objects.
     */
    void print_maze_in_lv();

    /**
     * @brief Prints the "snake crashed" message.
     */
    void print_snake_crashed();

    /**
     * @brief Prints the "level up" message.
     */
    void print_level_up();

    /**
     * @brief Prints the "game won" message.
     */
    void print_game_won();

    /**
     * @brief Prints the "game over" message.
     */
    void print_game_over();

    /**
     * @brief Respawns the snake after a crash or at the start of a level.
     */
    void respawn();

    /**
     * @brief Simulates the snake's AI thinking process.
     *
     * This method encapsulates the logic for the snake to determine its next move.
     */
    void snake_thinking();
    
    /**
     * @brief Gets the current state of the simulation.
     *
     * @return The current `states` enum value.
     */
    states get_states() const;

    /**
    * @brief Sets the snake's next direction based on a random search.
    */
    void troca();
};

#endif