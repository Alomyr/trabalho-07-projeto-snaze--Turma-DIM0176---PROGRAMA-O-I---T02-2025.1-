#include "SnazeSimulation.hpp"
#include "level.hpp"
#include "snake.hpp"
#include <iostream>
/**
 * @brief Processes user input to control the game state transitions.
 * 
 * Reads a line of input from the user and changes the game state accordingly:
 * - If on the START_SCREEN or LEVEL_UP state and the input is empty (Enter pressed),
 *   the state changes to SNAKE_THINKING to start or continue the game.
 * - If on the SNAKE_CRASHED state and Enter is pressed, respawns the snake and
 *   updates the game state depending on remaining lives.
 * 
 * @note This function only reacts to empty input lines (pressing Enter).
 */
void SnazeSimulation::input_process(){
    std::string input;
    std::getline(std::cin, input);
    
    if(current_state == states::START_SCREEN and input.empty()){
        //enter do state screen
        current_state=states::SNAKE_THINKING;
        return;

    }
    if(current_state == states::LEVEL_UP and input.empty()){
        // enter do level up
        current_state=states::SNAKE_THINKING;
        return;

    }
     if(current_state == states::SNAKE_CRASHED and input.empty()){
        respawn();
        if (current_life == 0) {
            current_state = states::GAME_OVER;
        } else{
            current_state = states::SNAKE_THINKING;
        }
    }
}

/**
 * @brief Handles collision and food consumption events in the simulation.
 * 
 * Updates the game state based on whether food was eaten or a collision occurred.
 * Increments the food count and updates the score when food is consumed.
 * Decrements lives and changes the game state on collision; ends the game if no lives remain.
 * 
 * @param food Indicates whether food was consumed (true) or not (false).
 * @param colision Indicates whether a collision occurred (true) or not (false).
 * 
 * @note If the collected food count reaches the target (`n_food`), the function
 *       triggers a level up. If a collision occurs and lives remain, it sets the
 *       state to SNAKE_CRASHED; otherwise, it ends the game.
 */
void SnazeSimulation::input_colision(bool food, bool colision){
    if (food==true){
        ++current_food;
        update_score();
        snake_obj.found_foods = false;

        if(current_food == n_food){
            level_up();
            return; 
        }
    }
    else if(colision==true and food == false){
        --current_life;
        if(current_life>0){
        current_state = states::SNAKE_CRASHED; 
        }
        if(current_life==0){
            current_state = states::GAME_OVER;
        }
    }
}

/**
* @brief Repositions the snake at the current level's spawn point.
* 
* The function removes the snake from the current level, resets its state, resets collision,
* and updates the head positions and next movement for the spawn point.
* 
* @note This function is typically used after death or a restart to place the snake
* at the level's starting position.
*/
void SnazeSimulation::respawn(){
    
        levels[current_level_index].remove_snake();
        snake_obj.reset(levels[current_level_index]);
        snake_obj.collision =false;
        TilePos spawn = levels[current_level_index].get_spawn_loc();
        head_pos = spawn;
        next_pos = spawn;

}
/**
* @brief Executes the snake's thought, calculating its next move.
* 
* The function calls the snake's breadth-first search method (`breadthFirst_search`),
* which determines the next step based on the current maze and its head position.
* 
* @note The next move calculation is done for the current level stored in `levels`.
*/
void SnazeSimulation::snake_thinking(){
    if (player_type == player_type_e::RANDOM) {
        SnazeSimulation& sin = SnazeSimulation::getInstance();
        sin.troca();
    } else {
        snake_obj.breadthFirst_search(levels[current_level_index], head_pos, next_pos);
    }
}