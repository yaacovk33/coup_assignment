// yaaccovkrawiec@gmail.com

#ifndef GAME_HPP
#define GAME_HPP

#include <vector>
#include <memory>
#include <string>
#include "Role.hpp"

class Player;

struct ActionRecord {
    ActionType action;
    Player* actor;
    Player* target;
    bool was_blocked;
    
    ActionRecord(ActionType a, Player* act, Player* targ) 
        : action(a), actor(act), target(targ), was_blocked(false) {}
};

class Game {
private:
    std::vector<std::shared_ptr<Player>> players;
    int current_player_index;
    int treasury_coins;
    bool game_active;
    bool extra_turn_allowed;
    std::vector<ActionRecord> action_history;
    
public:
    Game();
    
    void add_player(std::shared_ptr<Player> player);
    void start_game();
    void next_turn();
    
    // Game state methods
    std::string turn() const;
    std::vector<std::string> players_names() const;
    std::string winner() const;
    
    // Treasury management
    void add_coins_to_treasury(int amount) { treasury_coins += amount; }
    int get_treasury_coins() const { return treasury_coins; }
    
    // Turn management
    void allow_extra_turn() { extra_turn_allowed = true; }
    bool is_extra_turn_allowed() const { return extra_turn_allowed; }
    void reset_extra_turn() { extra_turn_allowed = false; }
    
    // Action history
    void add_action_to_history(ActionType action, Player* actor, Player* target);
    bool can_block_last_action(Player* blocker);
    void block_last_action();
    
    // Player management
    std::shared_ptr<Player> get_current_player();
    void eliminate_player(Player* player);
    void check_forced_coup();
    void clear_sanctions();
    
    // Getters
    bool is_game_active() const { return game_active; }
    std::vector<std::shared_ptr<Player>> get_players() const { return players; }
};

#endif // GAME_HPP