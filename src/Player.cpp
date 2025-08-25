// yaaccovkrawiec@gmail.com

#include "../include/Player.hpp"
#include "../include/Game.hpp"
#include "../include/Role.hpp"
#include <stdexcept>

Player::Player(const std::string& player_name) 
    : name(player_name), coins(2), is_active(true), is_sanctioned(false), last_arrested_target(nullptr) {
}

void Player::set_role(std::shared_ptr<Role> new_role) {
    role = new_role;
}

void Player::add_coins(int amount) {
    if (amount < 0) {
        throw std::invalid_argument("Cannot add negative coins");
    }
    coins += amount;
}

void Player::remove_coins(int amount) {
    if (amount < 0) {
        throw std::invalid_argument("Cannot remove negative coins");
    }
    if (coins < amount) {
        throw std::runtime_error("Not enough coins");
    }
    coins -= amount;
}

// Gather action - take 1 coin from treasury
void Player::gather(Game& game) {
    // Check if player can perform action
    if (!is_active) {
        throw std::runtime_error("Player is not active");
    }
    if (is_sanctioned) {
        throw std::runtime_error("Player is sanctioned and cannot gather");
    }
    
    // Take 1 coin
    add_coins(1);
    game.add_action_to_history(ActionType::GATHER, this, nullptr);
}

// Tax action - take 2 coins (3 if Governor)
void Player::tax(Game& game) {
    // Check if player can perform action
    if (!is_active) {
        throw std::runtime_error("Player is not active");
    }
    if (is_sanctioned) {
        throw std::runtime_error("Player is sanctioned and cannot tax");
    }
    
    // Governor gets 3 coins, others get 2
    int coins_to_add = 2;
    if (role && role->get_type() == RoleType::GOVERNOR) {
        coins_to_add = 3;
    }
    
    add_coins(coins_to_add);
    game.add_action_to_history(ActionType::TAX, this, nullptr);
}

void Player::bribe(Game& game) {
    if (!is_active) {
        throw std::runtime_error("Player is not active");
    }
    if (coins < 4) {
        throw std::runtime_error("Not enough coins for bribe");
    }
    
    remove_coins(4);
    game.add_action_to_history(ActionType::BRIBE, this, nullptr);
    game.allow_extra_turn();
}

// Arrest action - take 1 coin from target player
void Player::arrest(Player& target, Game& game) {
    // Validation checks
    if (!is_active) {
        throw std::runtime_error("Player is not active");
    }
    if (!target.is_active) {
        throw std::runtime_error("Target player is not active");
    }
    if (last_arrested_target == &target) {
        throw std::runtime_error("Cannot arrest the same player twice in a row");
    }
    
    // Handle coin transfer based on target's role
    if (target.get_coins() > 0) {
        if (target.role && target.role->get_type() == RoleType::MERCHANT) {
            // Merchant pays 2 coins to treasury instead of 1 to attacker
            if (target.get_coins() >= 2) {
                target.remove_coins(2);
                game.add_coins_to_treasury(2);
            } else {
                // If merchant has only 1 coin, pay it to treasury
                int available = target.get_coins();
                target.remove_coins(available);
                game.add_coins_to_treasury(available);
            }
        } else if (target.role && target.role->get_type() == RoleType::GENERAL) {
            // General gets the coin back immediately
            target.remove_coins(1);
            add_coins(1);
            target.add_coins(1);
        } else {
            // Normal arrest - take 1 coin from target
            target.remove_coins(1);
            add_coins(1);
        }
    }
    
    // Remember last arrested target
    last_arrested_target = &target;
    game.add_action_to_history(ActionType::ARREST, this, &target);
}

void Player::sanction(Player& target, Game& game) {
    if (!is_active) {
        throw std::runtime_error("Player is not active");
    }
    if (!target.is_active) {
        throw std::runtime_error("Target player is not active");
    }
    if (coins < 3) {
        throw std::runtime_error("Not enough coins for sanction");
    }
    
    remove_coins(3);
    target.set_sanctioned(true);
    
    if (target.role && target.role->get_type() == RoleType::BARON) {
        target.add_coins(1);
    }
    
    if (target.role && target.role->get_type() == RoleType::JUDGE) {
        if (coins > 0) {
            remove_coins(1);
            game.add_coins_to_treasury(1);
        }
    }
    
    game.add_action_to_history(ActionType::SANCTION, this, &target);
}

void Player::coup(Player& target, Game& game) {
    if (!is_active) {
        throw std::runtime_error("Player is not active");
    }
    if (!target.is_active) {
        throw std::runtime_error("Target player is not active");
    }
    if (coins < 7) {
        throw std::runtime_error("Not enough coins for coup");
    }
    
    remove_coins(7);
    game.add_action_to_history(ActionType::COUP, this, &target);
}