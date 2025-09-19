#include "SnazeSimulation.hpp"
#include "level.hpp"
#include "snake.hpp"

#include <iostream>
#include <thread>
#include <chrono>
#include <vector>

/// @brief Private constructor to enforce the Singleton pattern.
SnazeSimulation::SnazeSimulation() : current_state(states::START) { }

/// @brief Processes events based on the current game state.
void SnazeSimulation::process_events(){
    std::this_thread::sleep_for(std::chrono::milliseconds(1000/fps));
    
    switch (current_state) {
        case states::START_SCREEN:
            input_process();
            break;
        case states::SNAKE_THINKING:
            snake_thinking();
            break;
        case states::GAME_RUNNING:
            input_colision(snake_obj.found_foods, snake_obj.collision);
            break;
        case states::SNAKE_CRASHED:
            input_process();
            break;
        case states::LEVEL_UP:
            SnazeSimulation::input_process();       
            break;
        default:
            break;
    }
}

/// @brief Updates the game state based on the current game state.
void SnazeSimulation::update(){
    switch (current_state) {
        case states::START:
            current_state = states::WELCOME;
            break;
        case states::WELCOME:
            current_state = states::START_SCREEN;
            break;
        case states::SNAKE_THINKING:
            snake_update();
            current_state = states::GAME_RUNNING;
        case states::GAME_RUNNING:
            break;
        case states::LEVEL_UP:
            level_up();
            break;
        default:
            break;
    }
}

/// @brief Renders the game elements to the screen.
void SnazeSimulation::render(){
    switch (current_state) {
        case states::WELCOME:
            print_welcome();
            break;
        case states::GAME_RUNNING:
            print_maze();
            break;
        case states::SNAKE_CRASHED:
            print_snake_crashed();
            break;
        case states::LEVEL_UP:
            print_level_up();
            break;
        case states::GAME_WON:
            print_game_won();
            break;
        case states::GAME_OVER:
            print_game_over();
            break;
        default:
            break;
    }
}

/// @brief Checks if the game simulation is over.
bool SnazeSimulation::is_over(){ return current_state == states::GAME_OVER; }