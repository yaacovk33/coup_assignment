// yaacovkrawiec@gmail.com

#include "../include/Role.hpp"
#include "../include/Player.hpp"
#include "../include/Game.hpp"

void Governor::special_ability(Player& /*player*/, Game& /*game*/) {
    // Tax ability is handled in Player::tax()
}

bool Governor::can_block_action(ActionType action, Player* /*actor*/, Player* /*target*/) {
    return action == ActionType::TAX;
}

void Spy::special_ability(Player& /*player*/, Game& /*game*/) {
    // Special ability implemented in specific methods
}

int Spy::see_coins(Player& target) {
    return target.get_coins();
}

void Spy::block_arrest(Player& /*target*/) {
    // Implementation depends on game state management
}

void Baron::special_ability(Player& player, Game& /*game*/) {
    invest(player);
}

void Baron::invest(Player& player) {
    if (player.get_coins() >= 3) {
        player.remove_coins(3);
        player.add_coins(6);
    }
}

void General::special_ability(Player& /*player*/, Game& /*game*/) {
    // Block coup ability is handled separately
}

bool General::block_coup(Player& defender, Player& /*attacker*/, Game& /*game*/) {
    if (defender.get_coins() >= 5) {
        defender.remove_coins(5);
        return true;
    }
    return false;
}

void Judge::special_ability(Player& /*player*/, Game& /*game*/) {
    // Blocking bribe is handled in can_block_action
}

bool Judge::can_block_action(ActionType action, Player* /*actor*/, Player* /*target*/) {
    return action == ActionType::BRIBE;
}

void Merchant::special_ability(Player& player, Game& /*game*/) {
    start_turn_bonus(player);
}

void Merchant::start_turn_bonus(Player& player) {
    if (player.get_coins() >= 3) {
        player.add_coins(1);
    }
}