// yaaccovkrawiec@gmail.com

#include "../include/Game.hpp"
#include "../include/Player.hpp"
#include "../include/Role.hpp"
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <limits>
#include <iomanip>

class ConsoleUI {
private:
    Game game;
    std::vector<std::shared_ptr<Player>> players;
    
    // Display separator line
    void print_line(char c = '-', int width = 50) {
        for (int i = 0; i < width; ++i) std::cout << c;
        std::cout << "\n";
    }
    
    // Clear screen (works on most terminals)
    void clear_screen() {
        // Use ANSI escape codes for clearing screen
        std::cout << "\033[2J\033[1;1H";
    }
    
    // Display colored text (ANSI codes)
    void print_colored(const std::string& text, const std::string& color) {
        if (color == "red") std::cout << "\033[31m";
        else if (color == "green") std::cout << "\033[32m";
        else if (color == "yellow") std::cout << "\033[33m";
        else if (color == "blue") std::cout << "\033[34m";
        else if (color == "magenta") std::cout << "\033[35m";
        else if (color == "cyan") std::cout << "\033[36m";
        
        std::cout << text << "\033[0m";  // Reset color
    }
    
    // Display game header
    void display_header() {
        clear_screen();
        print_line('=', 50);
        std::cout << std::setw(30) << "COUP GAME\n";
        print_line('=', 50);
    }
    
    // Display player status
    void display_players() {
        std::cout << "\n";
        print_colored("PLAYERS STATUS:\n", "cyan");
        print_line();
        
        for (size_t i = 0; i < players.size(); ++i) {
            std::cout << std::setw(2) << i + 1 << ". ";
            
            // Player name
            std::cout << std::setw(12) << std::left << players[i]->get_name();
            
            if (!players[i]->is_player_active()) {
                print_colored(" [ELIMINATED]", "red");
            } else {
                // Coins
                std::cout << " | Coins: ";
                std::string coin_color = "white";
                if (players[i]->get_coins() >= 10) coin_color = "red";
                else if (players[i]->get_coins() >= 7) coin_color = "yellow";
                else if (players[i]->get_coins() >= 4) coin_color = "green";
                
                print_colored(std::to_string(players[i]->get_coins()), coin_color);
                std::cout << std::setw(3 - std::to_string(players[i]->get_coins()).length()) << "";
                
                // Role
                if (players[i]->get_role()) {
                    std::cout << " | ";
                    print_colored(players[i]->get_role()->get_name(), "magenta");
                }
                
                // Status
                if (players[i]->is_player_sanctioned()) {
                    print_colored(" [SANCTIONED]", "yellow");
                }
                
                // Current player
                if (game.is_game_active() && players[i]->get_name() == game.turn()) {
                    print_colored(" <- PLAYING", "green");
                }
            }
            std::cout << "\n";
        }
        print_line();
    }
    
    // Get integer input with validation
    int get_input(int min, int max) {
        int choice;
        while (true) {
            std::cin >> choice;
            if (std::cin.fail() || choice < min || choice > max) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                print_colored("Invalid input. Enter a number between " + 
                            std::to_string(min) + " and " + std::to_string(max) + ": ", "red");
            } else {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return choice;
            }
        }
    }
    
    // Select target player
    Player* select_target(Player* current) {
        std::cout << "\n";
        print_colored("SELECT TARGET:\n", "yellow");
        
        std::vector<Player*> valid_targets;
        int index = 1;
        
        for (auto& player : players) {
            if (player->is_player_active() && player.get() != current) {
                std::cout << index << ". " << player->get_name() 
                         << " (" << player->get_coins() << " coins)\n";
                valid_targets.push_back(player.get());
                index++;
            }
        }
        
        if (valid_targets.empty()) {
            print_colored("No valid targets available!\n", "red");
            return nullptr;
        }
        
        std::cout << "Choice: ";
        int choice = get_input(1, valid_targets.size());
        return valid_targets[choice - 1];
    }
    
    // Display action menu
    void display_menu() {
        auto current = game.get_current_player();
        if (!current) return;
        
        std::cout << "\n";
        print_colored("AVAILABLE ACTIONS:\n", "cyan");
        print_line();
        
        // Check if must coup
        if (current->get_coins() >= 10) {
            print_colored("*** YOU HAVE 10+ COINS - YOU MUST COUP! ***\n", "red");
            print_line();
        }
        
        int option = 1;
        
        // Basic actions
        std::cout << option++ << ". Gather (Take 1 coin)\n";
        std::cout << option++ << ". Tax (Take ";
        if (current->get_role() && current->get_role()->get_type() == RoleType::GOVERNOR) {
            print_colored("3", "green");
            std::cout << " coins as Governor)\n";
        } else {
            std::cout << "2 coins)\n";
        }
        
        std::cout << option++ << ". Arrest (Take 1 coin from another player)\n";
        
        // Actions requiring coins
        if (current->get_coins() >= 3) {
            std::cout << option++ << ". Sanction (3 coins - Block target's economic actions)\n";
        }
        
        if (current->get_coins() >= 4) {
            std::cout << option++ << ". Bribe (4 coins - Get extra turn)\n";
        }
        
        if (current->get_coins() >= 7) {
            print_colored(std::to_string(option++) + ". Coup (7 coins - Eliminate target)\n", "yellow");
        }
        
        // Special abilities
        if (current->get_role() && current->get_role()->get_type() == RoleType::BARON) {
            if (current->get_coins() >= 3) {
                print_colored(std::to_string(option++) + ". Invest (Baron: 3 coins -> 6 coins)\n", "magenta");
            }
        }
        
        print_line();
        std::cout << "0. Exit Game\n";
        std::cout << "\nYour choice: ";
    }
    
    // Process action
    bool process_action(int choice) {
        auto current = game.get_current_player();
        if (!current) return false;
        
        try {
            bool action_performed = false;
            
            // Map choice to actual action based on available options
            int actual_choice = 1;
            
            if (choice == 0) return false;
            
            if (choice == actual_choice++) { // Gather
                current->gather(game);
                print_colored("\n✓ " + current->get_name() + " gathered 1 coin.\n", "green");
                action_performed = true;
            }
            else if (choice == actual_choice++) { // Tax
                current->tax(game);
                print_colored("\n✓ " + current->get_name() + " used tax.\n", "green");
                action_performed = true;
            }
            else if (choice == actual_choice++) { // Arrest
                Player* target = select_target(current.get());
                if (target) {
                    current->arrest(*target, game);
                    print_colored("\n✓ " + current->get_name() + " arrested " + 
                                target->get_name() + ".\n", "green");
                    action_performed = true;
                }
            }
            else if (current->get_coins() >= 3 && choice == actual_choice++) { // Sanction
                Player* target = select_target(current.get());
                if (target) {
                    current->sanction(*target, game);
                    print_colored("\n✓ " + current->get_name() + " sanctioned " + 
                                target->get_name() + ".\n", "green");
                    action_performed = true;
                }
            }
            else if (current->get_coins() >= 4 && choice == actual_choice++) { // Bribe
                current->bribe(game);
                print_colored("\n✓ " + current->get_name() + " paid bribe for extra turn.\n", "green");
                return true; // Don't advance turn
            }
            else if (current->get_coins() >= 7 && choice == actual_choice++) { // Coup
                Player* target = select_target(current.get());
                if (target) {
                    current->coup(*target, game);
                    game.eliminate_player(target);
                    print_colored("\n✓ " + current->get_name() + " performed COUP on " + 
                                target->get_name() + "!\n", "yellow");
                    action_performed = true;
                }
            }
            else if (current->get_role() && current->get_role()->get_type() == RoleType::BARON &&
                    current->get_coins() >= 3 && choice == actual_choice++) { // Baron invest
                auto baron = std::dynamic_pointer_cast<Baron>(current->get_role());
                if (baron) {
                    baron->invest(*current);
                    print_colored("\n✓ " + current->get_name() + " (Baron) invested 3 coins and got 6 back.\n", "green");
                    action_performed = true;
                }
            }
            else {
                print_colored("\nInvalid choice!\n", "red");
                return true;
            }
            
            if (action_performed) {
                game.next_turn();
            }
            
            std::cout << "\nPress Enter to continue...";
            std::cin.get();
            
        } catch (const std::exception& e) {
            print_colored("\n✗ Error: " + std::string(e.what()) + "\n", "red");
            std::cout << "Press Enter to continue...";
            std::cin.get();
        }
        
        return true;
    }
    
public:
    void run() {
        // Initialize game
        display_header();
        
        std::cout << "\n";
        print_colored("GAME SETUP\n", "cyan");
        print_line();
        
        std::cout << "Number of players (2-6): ";
        int num_players = get_input(2, 6);
        
        std::cout << "\n";
        print_colored("Available Roles:\n", "yellow");
        std::cout << "1. Governor - Tax gives 3 coins, can block tax\n";
        std::cout << "2. Baron - Can invest 3 coins to get 6\n";
        std::cout << "3. Merchant - Bonus coin at turn start with 3+ coins\n";
        std::cout << "4. General - Can block coup for 5 coins\n";
        std::cout << "5. Judge - Can block bribe\n";
        std::cout << "6. Spy - Can see other players' coins\n";
        
        // Create players
        for (int i = 0; i < num_players; ++i) {
            std::cout << "\n";
            print_colored("Player " + std::to_string(i + 1) + " Setup:\n", "cyan");
            
            std::string name;
            std::cout << "Enter name: ";
            std::getline(std::cin, name);
            
            auto player = std::make_shared<Player>(name);
            
            std::cout << "Choose role (1-6): ";
            int role_choice = get_input(1, 6);
            
            switch (role_choice) {
                case 1: player->set_role(std::make_shared<Governor>()); break;
                case 2: player->set_role(std::make_shared<Baron>()); break;
                case 3: player->set_role(std::make_shared<Merchant>()); break;
                case 4: player->set_role(std::make_shared<General>()); break;
                case 5: player->set_role(std::make_shared<Judge>()); break;
                case 6: player->set_role(std::make_shared<Spy>()); break;
            }
            
            players.push_back(player);
            game.add_player(player);
        }
        
        game.start_game();
        
        // Game loop
        bool running = true;
        while (running && game.is_game_active()) {
            display_header();
            display_players();
            display_menu();
            
            int max_choice = 7; // Adjust based on available actions
            int choice = get_input(0, max_choice);
            running = process_action(choice);
        }
        
        // Game over
        if (!game.is_game_active()) {
            display_header();
            display_players();
            
            std::cout << "\n";
            print_line('=', 50);
            print_colored("          GAME OVER!\n", "yellow");
            try {
                print_colored("        Winner: " + game.winner() + "\n", "green");
            } catch (...) {
                std::cout << "Game ended.\n";
            }
            print_line('=', 50);
        }
    }
};

int main() {
    ConsoleUI ui;
    ui.run();
    return 0;
}