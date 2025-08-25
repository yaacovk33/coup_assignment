// yaaccovkrawiec@gmail.com

#include "../include/Game.hpp"
#include "../include/Player.hpp"
#include <algorithm>
#include <stdexcept>

Game::Game() : current_player_index(0), treasury_coins(50), game_active(false), extra_turn_allowed(false) {
}

void Game::add_player(std::shared_ptr<Player> player) {
    if (game_active) {
        throw std::runtime_error("Cannot add player to active game");
    }
    if (players.size() >= 6) {
        throw std::runtime_error("Maximum 6 players allowed");
    }
    players.push_back(player);
}

void Game::start_game() {
    if (players.size() < 2) {
        throw std::runtime_error("Need at least 2 players to start");
    }
    game_active = true;
}

void Game::next_turn() {
    if (!game_active) {
        throw std::runtime_error("Game is not active");
    }
    
    if (!extra_turn_allowed) {
        clear_sanctions();
        
        do {
            current_player_index = (current_player_index + 1) % players.size();
        } while (!players[current_player_index]->is_player_active());
        
        // Check if merchant gets bonus
        auto current = players[current_player_index];
        if (current->get_role() && current->get_role()->get_type() == RoleType::MERCHANT) {
            auto merchant_role = std::dynamic_pointer_cast<Merchant>(current->get_role());
            if (merchant_role) {
                merchant_role->start_turn_bonus(*current);
            }
        }
        
        check_forced_coup();
    } else {
        reset_extra_turn();
    }
}

std::string Game::turn() const {
    if (!game_active) {
        throw std::runtime_error("Game is not active");
    }
    return players[current_player_index]->get_name();
}

std::vector<std::string> Game::players_names() const {
    std::vector<std::string> active_players;
    for (const auto& player : players) {
        if (player->is_player_active()) {
            active_players.push_back(player->get_name());
        }
    }
    return active_players;
}

std::string Game::winner() const {
    if (game_active) {
        throw std::runtime_error("Game is still active");
    }
    
    for (const auto& player : players) {
        if (player->is_player_active()) {
            return player->get_name();
        }
    }
    return "";
}

void Game::add_action_to_history(ActionType action, Player* actor, Player* target) {
    action_history.push_back(ActionRecord(action, actor, target));
}

bool Game::can_block_last_action(Player* blocker) {
    if (action_history.empty()) {
        return false;
    }
    
    ActionRecord& last_action = action_history.back();
    if (last_action.was_blocked) {
        return false;
    }
    
    // Check if blocker's role can block this action
    if (blocker->get_role()) {
        return blocker->get_role()->can_block_action(last_action.action, last_action.actor, last_action.target);
    }
    
    return false;
}

void Game::block_last_action() {
    if (!action_history.empty()) {
        action_history.back().was_blocked = true;
    }
}

std::shared_ptr<Player> Game::get_current_player() {
    if (!game_active) {
        return nullptr;
    }
    return players[current_player_index];
}

void Game::eliminate_player(Player* player) {
    player->set_active(false);
    
    // Check if only one player remains
    int active_count = 0;
    for (const auto& p : players) {
        if (p->is_player_active()) {
            active_count++;
        }
    }
    
    if (active_count <= 1) {
        game_active = false;
    }
}

void Game::check_forced_coup() {
    auto current = players[current_player_index];
    if (current->get_coins() >= 10) {
        // Player must perform coup this turn
        // This is enforced in the game logic
    }
}

void Game::clear_sanctions() {
    for (auto& player : players) {
        player->set_sanctioned(false);
    }
}