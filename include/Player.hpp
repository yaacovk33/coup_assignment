// yaacovkrawiec@gmail.com

#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <string>
#include <memory>

class Role; // Forward declaration
class Game; // Forward declaration

// Player class represents a player in the Coup game
// Each player has coins, a role, and can perform various actions
class Player {
private:
    std::string name;                    // Player's name
    int coins;                           // Number of coins the player has
    std::shared_ptr<Role> role;          // Player's role (Governor, Baron, etc.)
    bool is_active;                      // Is player still in the game?
    bool is_sanctioned;                  // Is player sanctioned (cannot use economic actions)?
    Player* last_arrested_target;        // Track last arrest target to prevent consecutive arrests
    
public:
    // Constructor - creates a player with 2 starting coins
    Player(const std::string& player_name);
    
    // Getter methods - return player information
    std::string get_name() const { return name; }
    int get_coins() const { return coins; }
    bool is_player_active() const { return is_active; }
    bool is_player_sanctioned() const { return is_sanctioned; }
    std::shared_ptr<Role> get_role() const { return role; }
    
    // Setter methods - modify player state
    void set_role(std::shared_ptr<Role> new_role);
    void add_coins(int amount);          // Throws exception if amount is negative
    void remove_coins(int amount);       // Throws exception if not enough coins
    void set_active(bool active) { is_active = active; }
    void set_sanctioned(bool sanctioned) { is_sanctioned = sanctioned; }
    void set_last_arrested(Player* target) { last_arrested_target = target; }
    Player* get_last_arrested() const { return last_arrested_target; }
    
    // Basic actions every player can perform
    void gather(Game& game);             // Take 1 coin from treasury
    void tax(Game& game);                // Take 2 coins (3 if Governor)
    void bribe(Game& game);              // Pay 4 coins for extra turn
    void arrest(Player& target, Game& game);    // Take 1 coin from another player
    void sanction(Player& target, Game& game);  // Pay 3 coins to block target's economic actions
    void coup(Player& target, Game& game);      // Pay 7 coins to eliminate target
};

#endif // PLAYER_HPP