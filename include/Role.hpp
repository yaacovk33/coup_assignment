// yaacovkrawiec@gmail.com

#ifndef ROLE_HPP
#define ROLE_HPP

#include <string>
#include <memory>

class Player;
class Game;

enum class RoleType {
    GOVERNOR,
    SPY,
    BARON,
    GENERAL,
    JUDGE,
    MERCHANT
};

enum class ActionType {
    GATHER,
    TAX,
    BRIBE,
    ARREST,
    SANCTION,
    COUP
};

class Role {
protected:
    RoleType type;
    std::string name;
    
public:
    Role(RoleType role_type, const std::string& role_name) 
        : type(role_type), name(role_name) {}
    
    virtual ~Role() = default;
    
    RoleType get_type() const { return type; }
    std::string get_name() const { return name; }
    
    virtual void special_ability(Player& player, Game& game) = 0;
    virtual bool can_block_action(ActionType /*action*/, Player* /*actor*/, Player* /*target*/) { return false; }
};

class Governor : public Role {
public:
    Governor() : Role(RoleType::GOVERNOR, "Governor") {}
    
    void special_ability(Player& player, Game& game) override;
    bool can_block_action(ActionType action, Player* actor, Player* target) override;
};

class Spy : public Role {
public:
    Spy() : Role(RoleType::SPY, "Spy") {}
    
    void special_ability(Player& player, Game& game) override;
    int see_coins(Player& target);
    void block_arrest(Player& target);
};

class Baron : public Role {
public:
    Baron() : Role(RoleType::BARON, "Baron") {}
    
    void special_ability(Player& player, Game& game) override;
    void invest(Player& player);
};

class General : public Role {
public:
    General() : Role(RoleType::GENERAL, "General") {}
    
    void special_ability(Player& player, Game& game) override;
    bool block_coup(Player& defender, Player& attacker, Game& game);
};

class Judge : public Role {
public:
    Judge() : Role(RoleType::JUDGE, "Judge") {}
    
    void special_ability(Player& player, Game& game) override;
    bool can_block_action(ActionType action, Player* actor, Player* target) override;
};

class Merchant : public Role {
public:
    Merchant() : Role(RoleType::MERCHANT, "Merchant") {}
    
    void special_ability(Player& player, Game& game) override;
    void start_turn_bonus(Player& player);
};

#endif // ROLE_HPP