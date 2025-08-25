// yaacovkrawiec@gmail.com

#include "../include/Game.hpp"
#include "../include/Player.hpp"
#include "../include/Role.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <string>
#include <sstream>

class GameGUI {
private:
    sf::RenderWindow window;
    sf::Font font;
    Game& game;
    std::vector<std::shared_ptr<Player>> players;
    
    // UI Elements
    std::vector<sf::RectangleShape> action_buttons;
    std::vector<sf::Text> action_labels;
    std::vector<sf::Text> player_info;
    sf::Text current_turn_text;
    sf::Text message_text;
    
    int selected_target;
    std::string message;
    
public:
    GameGUI(Game& g, std::vector<std::shared_ptr<Player>>& p) 
        : window(sf::VideoMode(800, 600), "Coup Game"), game(g), players(p), selected_target(-1) {
        
        // Load font - try multiple locations
        if (!font.loadFromFile("assets/arial.ttf")) {
            // Try system fonts
            if (!font.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf")) {
                if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
                    // Last resort - Ubuntu font
                    font.loadFromFile("/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf");
                }
            }
        }
        
        initialize_ui();
    }
    
    void initialize_ui() {
        // Create action buttons
        std::vector<std::string> actions = {"Gather", "Tax", "Bribe", "Arrest", "Sanction", "Coup"};
        
        for (size_t i = 0; i < actions.size(); ++i) {
            sf::RectangleShape button(sf::Vector2f(120, 40));
            button.setPosition(50 + (i % 3) * 140, 400 + (i / 3) * 60);
            button.setFillColor(sf::Color(100, 100, 200));
            button.setOutlineColor(sf::Color::White);
            button.setOutlineThickness(2);
            action_buttons.push_back(button);
            
            sf::Text label(actions[i], font, 16);
            label.setPosition(button.getPosition().x + 10, button.getPosition().y + 10);
            label.setFillColor(sf::Color::White);
            action_labels.push_back(label);
        }
        
        // Initialize player info display
        for (size_t i = 0; i < players.size(); ++i) {
            sf::Text info("", font, 14);
            info.setPosition(50, 100 + i * 60);
            info.setFillColor(sf::Color::White);
            player_info.push_back(info);
        }
        
        // Current turn display
        current_turn_text.setFont(font);
        current_turn_text.setCharacterSize(20);
        current_turn_text.setPosition(50, 30);
        current_turn_text.setFillColor(sf::Color::Yellow);
        
        // Message display
        message_text.setFont(font);
        message_text.setCharacterSize(16);
        message_text.setPosition(50, 550);
        message_text.setFillColor(sf::Color::Green);
    }
    
    void update_player_info() {
        for (size_t i = 0; i < players.size(); ++i) {
            std::stringstream ss;
            ss << players[i]->get_name();
            
            if (!players[i]->is_player_active()) {
                ss << " [ELIMINATED]";
            } else {
                ss << " - Coins: " << players[i]->get_coins();
                
                if (players[i]->get_role()) {
                    ss << " - Role: " << players[i]->get_role()->get_name();
                }
                
                if (players[i]->is_player_sanctioned()) {
                    ss << " [SANCTIONED]";
                }
            }
            
            player_info[i].setString(ss.str());
            
            // Highlight current player
            if (game.is_game_active() && players[i]->get_name() == game.turn()) {
                player_info[i].setFillColor(sf::Color::Yellow);
            } else if (!players[i]->is_player_active()) {
                player_info[i].setFillColor(sf::Color::Red);
            } else {
                player_info[i].setFillColor(sf::Color::White);
            }
        }
        
        // Update current turn text
        if (game.is_game_active()) {
            current_turn_text.setString("Current Turn: " + game.turn());
        } else {
            try {
                current_turn_text.setString("Winner: " + game.winner());
            } catch (...) {
                current_turn_text.setString("Game Not Started");
            }
        }
        
        // Update message
        message_text.setString(message);
    }
    
    void handle_action(int action_index) {
        if (!game.is_game_active()) {
            message = "Game is not active!";
            return;
        }
        
        auto current_player = game.get_current_player();
        if (!current_player) return;
        
        try {
            switch (action_index) {
                case 0: // Gather
                    current_player->gather(game);
                    message = current_player->get_name() + " gathered 1 coin";
                    game.next_turn();
                    break;
                    
                case 1: // Tax
                    current_player->tax(game);
                    message = current_player->get_name() + " used tax";
                    game.next_turn();
                    break;
                    
                case 2: // Bribe
                    current_player->bribe(game);
                    message = current_player->get_name() + " paid bribe for extra turn";
                    break;
                    
                case 3: // Arrest
                    if (selected_target >= 0 && selected_target < (int)players.size()) {
                        current_player->arrest(*players[selected_target], game);
                        message = current_player->get_name() + " arrested " + players[selected_target]->get_name();
                        game.next_turn();
                    } else {
                        message = "Select a target first!";
                    }
                    break;
                    
                case 4: // Sanction
                    if (selected_target >= 0 && selected_target < (int)players.size()) {
                        current_player->sanction(*players[selected_target], game);
                        message = current_player->get_name() + " sanctioned " + players[selected_target]->get_name();
                        game.next_turn();
                    } else {
                        message = "Select a target first!";
                    }
                    break;
                    
                case 5: // Coup
                    if (selected_target >= 0 && selected_target < (int)players.size()) {
                        current_player->coup(*players[selected_target], game);
                        game.eliminate_player(players[selected_target].get());
                        message = current_player->get_name() + " performed coup on " + players[selected_target]->get_name();
                        game.next_turn();
                    } else {
                        message = "Select a target first!";
                    }
                    break;
            }
        } catch (const std::exception& e) {
            message = "Error: " + std::string(e.what());
        }
        
        selected_target = -1;
    }
    
    void run() {
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
                
                if (event.type == sf::Event::MouseButtonPressed) {
                    sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
                    
                    // Check action buttons
                    for (size_t i = 0; i < action_buttons.size(); ++i) {
                        if (action_buttons[i].getGlobalBounds().contains(mouse_pos.x, mouse_pos.y)) {
                            handle_action(i);
                        }
                    }
                    
                    // Check player selection for targeting
                    for (size_t i = 0; i < player_info.size(); ++i) {
                        if (player_info[i].getGlobalBounds().contains(mouse_pos.x, mouse_pos.y)) {
                            if (players[i]->is_player_active() && players[i] != game.get_current_player()) {
                                selected_target = i;
                                message = "Selected target: " + players[i]->get_name();
                            }
                        }
                    }
                }
                
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Space) {
                        // Skip turn (for testing)
                        if (game.is_game_active()) {
                            game.next_turn();
                            message = "Turn skipped";
                        }
                    }
                }
            }
            
            update_player_info();
            
            window.clear(sf::Color::Black);
            
            // Draw UI elements
            window.draw(current_turn_text);
            
            for (const auto& info : player_info) {
                window.draw(info);
            }
            
            for (const auto& button : action_buttons) {
                window.draw(button);
            }
            
            for (const auto& label : action_labels) {
                window.draw(label);
            }
            
            window.draw(message_text);
            
            // Highlight selected target
            if (selected_target >= 0) {
                sf::RectangleShape highlight(sf::Vector2f(400, 50));
                highlight.setPosition(45, 95 + selected_target * 60);
                highlight.setFillColor(sf::Color(255, 255, 0, 50));
                window.draw(highlight);
            }
            
            window.display();
        }
    }
};

int main() {
    // Create game and players
    Game game;
    
    auto alice = std::make_shared<Player>("Alice");
    auto bob = std::make_shared<Player>("Bob");
    auto charlie = std::make_shared<Player>("Charlie");
    
    alice->set_role(std::make_shared<Governor>());
    bob->set_role(std::make_shared<Baron>());
    charlie->set_role(std::make_shared<Merchant>());
    
    std::vector<std::shared_ptr<Player>> player_list = {alice, bob, charlie};
    
    game.add_player(alice);
    game.add_player(bob);
    game.add_player(charlie);
    
    game.start_game();
    
    // Run GUI
    GameGUI gui(game, player_list);
    gui.run();
    
    return 0;
}