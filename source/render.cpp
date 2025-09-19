#include "SnazeSimulation.hpp"
#include "level.hpp"
#include "snake.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string_view>
#include <unordered_map>
#include <vector>
namespace fs = std::filesystem;

/**
 * @brief A static `unordered_map` that maps `Level::tile_type_e` enum values
 * to their corresponding string representations (characters) for console rendering.
 */
std::unordered_map<Level::tile_type_e , std::string> tile_2_char = {
  {Level::tile_type_e::EMPTY, " "},
  {Level::tile_type_e::WALL, "█"},
  {Level::tile_type_e::INV_WALL, " "},
  {Level::tile_type_e::FOOD, "¤"},
  {Level::tile_type_e::SNAKE_BODY, "●"},
  {Level::tile_type_e::SNAKE_HEAD, "◎"},
};

/**
 * @brief Prints the usage information for the Snaze game or an error message and exits.
 *
 * If `msg` is empty, it prints the general usage instructions including available
 * command-line options. If `msg` contains text, it prints that message to `std::cerr`
 * as an error and then exits with `EXIT_FAILURE`.
 *
 * @param msg An optional message to display. Defaults to an empty string.
 */
void usage(std::string_view msg = "") {
  if (msg.empty()) {
    std::cout << R"(Usage: snaze [<options>] <input_level_file>
Game simulation options:
--help Print this help text.
--fps <num> Number of frames (board) presented per second.
--lives <num> Number of lives the snake shall have. Default = 5.
--food <num> Number of food pellets for the entire simulation. Default = 10.
--playertype <type> Type of snake intelligence: random, backtracking. Default = backtracking.
)";

    exit(EXIT_SUCCESS);
  } else {
    std::cerr << msg << '\n';

    exit(EXIT_FAILURE);
  }
}

/**
 * @brief Verifies if a given string represents a valid natural number (non-negative integer).
 *
 * @param arg The string view to check.
 * @return True if the string contains only digits and is not empty, false otherwise.
 */
bool verifies_natural_number(const std::string_view &arg) {
  if (arg.empty()) return false;

  for (char ch : arg)
    if (!std::isdigit(ch)) return false;

  return true;
}

/**
 * @brief Trims leading and trailing whitespace characters from a string.
 *
 * @param s The string to be trimmed.
 * @param t A C-style string containing characters to trim. Defaults to common whitespace characters.
 */
void trim(std::string &s, const char *t = " \t\n\r\f\v") {
  s.erase(0, s.find_first_not_of(t));
  s.erase(s.find_last_not_of(t) + 1);
};

/**
 * @brief Splits a string by a given delimiter and returns the parts as a vector.
 * @param str The input string. 
 * @param token The delimiter string.
 * @return A vector containing the non-empty parts.
 */
std::vector<std::string> tokenizer(const std::string &str,
                                   const std::string &token) {
  std::vector<std::string> output;

  size_t last_pos_piece = 0;
  size_t found_pos;

  while ((found_pos = str.find(token, last_pos_piece)) != std::string::npos) {
    std::string part = str.substr(last_pos_piece, found_pos - last_pos_piece);
    if (!part.empty()) {
      output.push_back(part);
    }
    last_pos_piece = found_pos + token.length();
  }

  // Adds the last part, if not empty
  std::string last_part = str.substr(last_pos_piece);
  if (!last_part.empty()) {
    output.push_back(last_part);
  }

  return output;
}

/// @brief Parses a file to load level data.
void SnazeSimulation::parse_file(const char *file_path) {
  std::ifstream file(file_path);
  std::string line;

  std::vector<std::string> lines;

  while (std::getline(file, line)) {
    lines.push_back(line);
  }

  for (long unsigned int i = 0; i < lines.size(); ++i) {
    if (lines[i].empty())
      continue;

    trim(lines[i]);

    std::vector<std::string> numbers = tokenizer(lines[i], " ");

    if (numbers.size() < 2 or not(verifies_natural_number(numbers[0])) or
        not(verifies_natural_number(numbers[1]))) {
      usage("Error: expected two natural numbers for level dimensions.");
    }

    int n_rows = std::stoi(numbers[0]);
    int n_cols = std::stoi(numbers[1]);

    if (n_rows <= 0 or n_cols <= 0 or n_rows > 100 or n_cols > 100) {
      usage("Invalid number of rows or columns.");
    }

    std::vector<std::string> maze_level;
    int count_spawn = 0;

    for (long unsigned int j = i + 1; j <= i + n_rows; ++j) {
      for (auto ch : lines[j]) {
        if (ch == '&')
          count_spawn++;
      }

      if (count_spawn > 1)
        break;

      maze_level.push_back(lines[j]);
    }

    if (count_spawn == 1) {
      Level lvl(maze_level);
      levels.push_back(lvl);
    }

    i += n_rows;
  }
}

/// @brief Initializes the simulation based on command line arguments.
void SnazeSimulation::initialize(int argc, char *argv[]) {
  if (argc == 1) usage();

  for (auto i = 1; i < argc; ++i) {
    std::string_view arg = argv[i];

    if (arg == "--help") usage();

    if (arg == "--fps" and i + 1 < argc) {
      std::string next_arg = argv[i + 1];

      if (!verifies_natural_number(next_arg) || std::stoi(next_arg) == 0) {
        usage("Error: invalid number of frames presented per second.");
      }

      fps = std::stoi(next_arg);
      std::cout << fps << '\n';

      ++i;
      continue;
    } else if (arg == "--lives" and i + 1 < argc) {
      std::string next_arg = argv[i + 1];

      if (!verifies_natural_number(next_arg) || std::stoi(next_arg) == 0) {
        usage("Error: invalid number of lives.");
      }

      n_lives = std::stoi(next_arg);
      std::cout << n_lives << '\n';

      ++i;
      continue;
    } else if (arg == "--food" and i + 1 < argc) {
      std::string next_arg = argv[i + 1];

      if (!verifies_natural_number(next_arg) || std::stoi(next_arg) == 0) {
        usage("Error: invalid number of food pellets.");
      }

      n_food = std::stoi(next_arg);
      std::cout << n_food << '\n';

      ++i;
      continue;
    } else if (arg == "--playertype" and i + 1 < argc) {
      std::string next_arg = argv[i + 1];

      if (next_arg == "random") {
        player_type = player_type_e::RANDOM;
      } else if (next_arg == "backtracking") {
        // Do nothing.
        // Using default inicialization.
      } else {
        usage("Error: invalid player type.");
      }

      ++i;
      continue;
    } else {
      // If it's not one of the valid options, let's consider it is a file.

      fs::path path = argv[i];

      if (not(fs::exists(path)) or not(fs::is_regular_file(path))) {
        std::ostringstream out;

        out << "Sorry, unable to read \"" << std::string(argv[i]) << "\".\n";

        std::cout << out.str();

        exit(EXIT_FAILURE);
      }

      parse_file(argv[i]);

      if (levels.empty()) {
        usage("Error: no valid levels were loaded.");
      }

      head_pos = levels[current_level_index].get_spawn_loc();
      dir = direction::right;
      levels[current_level_index].set_tile_type(Level::tile_type_e::SNAKE_HEAD, head_pos);
      snake_obj.init(head_pos);
    }
  }
}

/// @brief Prints the welcome screen to the console.
void SnazeSimulation::print_welcome() {
    std::ostringstream out;
  
    out << " --->  Welcome to the classic Snake Game  <--- \n"
        << "        copyright DIMAp/UFRN 2017-2025\n"
        << "--------------------------------------------------------\n"
        << " Levels loaded: " << levels.size() << " | Snake lives: " << n_lives << " | Apples to eat: " << n_food << "\n"
        << " Clear all levels to win the game. Good luck!!!\n"
        << "--------------------------------------------------------\n"
        << " >>> Press <ENTER> to start the game!\n\n"
        << " Lives: ";

    for (auto i = 0; i < n_lives; ++i) out << "♥";

    out << " | Score: 0     | Food eaten: 0  of " << n_food << "\n"; 
    out << "--------------------------------------------------------\n\n";

    for (size_t i = 0; i < levels[current_level_index].n_rows(); ++i) {
        for (size_t j = 0; j < levels[current_level_index].n_cols(); ++j) {
            if (TilePos(i, j) == levels[current_level_index].get_spawn_loc()) {
                out << "๑";
            } else {
                Level::tile_type_e type = levels[current_level_index].get_tile_type(TilePos(i, j));
                if (type == Level::tile_type_e::FOOD) {
                  out << " ";
                } else {
                  out << tile_2_char[type];
                }
            }
        }
        out << '\n';
    }

    out << "\n--------------------------------------------------------\n";  

    std::cout << out.str();
}

/// @brief Prints the maze, likely for a specific level.
void SnazeSimulation::print_maze_in_lv(){
    std::ostringstream out;
  
    std::cout << ">>> Level up! Press <ENTER> to try again.\n";
    out << "  \n";

    for (auto i = 0; i < current_life; ++i) out << "♥";
    for (auto i = 0; i < n_lives - current_life; ++i) out << "♡";

    out << " | Score: " << score << "     | Food eaten: 0  of " << n_food << "\n"; 
    out << "--------------------------------------------------------\n\n";

    for (size_t i = 0; i < levels[current_level_index].n_rows(); ++i) {
        for (size_t j = 0; j < levels[current_level_index].n_cols(); ++j) {
            if (TilePos(i, j) == levels[current_level_index].get_spawn_loc()) {
                out << "๑";
            } else {
                Level::tile_type_e type = levels[current_level_index].get_tile_type(TilePos(i, j));
                if (type == Level::tile_type_e::FOOD) {
                  out << " ";
                } else {
                  out << tile_2_char[type];
                }
            }
        }
        out << '\n';
    }

    out << "\n--------------------------------------------------------\n";  

    std::cout << out.str();
}

/// @brief Prints the current maze to the console.
void SnazeSimulation::print_maze() {
  std::ostringstream out;

  out << " Lives: ";

  for (auto i = 0; i < current_life; ++i) out << "♥";
  for (auto i = 0; i < n_lives - current_life; ++i) out << "♡";


  out << " | Score: " << score << "     | Food eaten: " << current_food << " of " << n_food << '\n'
      << "--------------------------------------------------------\n\n";

  for (size_t i = 0; i < levels[current_level_index].n_rows(); ++i) {
      for (size_t j = 0; j < levels[current_level_index].n_cols(); ++j) {
          Level::tile_type_e type = levels[current_level_index].get_tile_type(TilePos(i, j));
          out << tile_2_char[type];
      }
      out << '\n';
  }

  out << "\n--------------------------------------------------------\n";

  std::cout << out.str();
  current_state = states::SNAKE_THINKING; 

}

/// @brief Prints the "snake crashed" message.
void SnazeSimulation::print_snake_crashed() {
  std::ostringstream out;

  out << " Lives: ";

  for (auto i = 0; i < current_life; ++i) out << "♥";
  for (auto i = 0; i < n_lives - current_life; ++i) out << "♡";

  out << " | Score: " << score << "     | Food eaten: " << current_food << " of " << n_food << '\n'
      << "--------------------------------------------------------\n\n";

  for (size_t i = 0; i < levels[current_level_index].n_rows(); ++i) {
      for (size_t j = 0; j < levels[current_level_index].n_cols(); ++j) {
          Level::tile_type_e type = levels[current_level_index].get_tile_type(TilePos(i, j));
          if (type == Level::tile_type_e::SNAKE_HEAD) {
            out << "☠";
          } else if (type == Level::tile_type_e::SNAKE_BODY) {
            out << '.';
          } else {
            out << tile_2_char[type];
          }
      }
      out << '\n';
  }

  out << "\n--------------------------------------------------------\n"
      << ">>> Press <ENTER> to try again.\n";

  std::cout << out.str();
}

/// @brief Prints the "level up" message.
void SnazeSimulation::print_level_up() {
  std::cout << ">>> Press <ENTER> to try again.\n";
}

/// @brief Prints the "game won" message.
void SnazeSimulation::print_game_won() {
  std::cout << R"(+-------------------------------------+
|    CONGRATULATIONS anaconda WON!    |
|        Thanks for playing!          |
+-------------------------------------+
)";
  exit(EXIT_SUCCESS);
}

/// @brief Prints the "game over" message.
void SnazeSimulation::print_game_over() {
  std::cout << R"(+-------------------------------------+
|        Sorry, anaconda LOST :(      |
|        Thanks for playing!          |
+-------------------------------------+
)";
  exit(EXIT_FAILURE);
}