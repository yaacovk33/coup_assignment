// yaacovkrawiec@gmail.com

#include <iostream>
#include <memory>
#include "../include/Game.hpp"
#include "../include/Player.hpp"
#include "../include/Role.hpp"

int main() {
    std::cout << "=== Coup Game Demo ===" << std::endl;
    
    // Create game instance
    Game game;
    
    // Create players
    auto alice = std::make_shared<Player>("Alice");
    auto bob = std::make_shared<Player>("Bob");
    auto charlie = std::make_shared<Player>("Charlie");
    
    // Assign roles
    alice->set_role(std::make_shared<Governor>());
    bob->set_role(std::make_shared<Baron>());
    charlie->set_role(std::make_shared<Merchant>());
    
    // Add players to game
    game.add_player(alice);
    game.add_player(bob);
    game.add_player(charlie);
    
    // Start the game
    game.start_game();
    
    std::cout << "\nGame started with players: ";
    for (const auto& name : game.players_names()) {
        std::cout << name << " ";
    }
    std::cout << std::endl;
    
    // Simulate some turns
    std::cout << "\n--- Turn 1 ---" << std::endl;
    std::cout << "Current player: " << game.turn() << std::endl;
    std::cout << "Alice coins: " << alice->get_coins() << std::endl;
    
    // Alice uses gather
    alice->gather(game);
    std::cout << "Alice gathers 1 coin" << std::endl;
    std::cout << "Alice coins after gather: " << alice->get_coins() << std::endl;
    
    game.next_turn();
    
    std::cout << "\n--- Turn 2 ---" << std::endl;
    std::cout << "Current player: " << game.turn() << std::endl;
    std::cout << "Bob coins: " << bob->get_coins() << std::endl;
    
    // Bob uses tax
    bob->tax(game);
    std::cout << "Bob uses tax and gets 2 coins" << std::endl;
    std::cout << "Bob coins after tax: " << bob->get_coins() << std::endl;
    
    game.next_turn();
    
    std::cout << "\n--- Turn 3 ---" << std::endl;
    std::cout << "Current player: " << game.turn() << std::endl;
    std::cout << "Charlie coins: " << charlie->get_coins() << std::endl;
    
    // Charlie gathers
    charlie->gather(game);
    std::cout << "Charlie gathers 1 coin" << std::endl;
    std::cout << "Charlie coins after gather: " << charlie->get_coins() << std::endl;
    
    game.next_turn();
    
    std::cout << "\n--- Turn 4 ---" << std::endl;
    std::cout << "Current player: " << game.turn() << std::endl;
    
    // Alice uses tax (as Governor, gets 3 coins)
    alice->tax(game);
    std::cout << "Alice (Governor) uses tax and gets 3 coins" << std::endl;
    std::cout << "Alice coins after tax: " << alice->get_coins() << std::endl;
    
    game.next_turn();
    
    std::cout << "\n--- Turn 5 ---" << std::endl;
    std::cout << "Current player: " << game.turn() << std::endl;
    
    // Bob uses Baron's invest ability
    if (bob->get_coins() >= 3) {
        auto baron = std::dynamic_pointer_cast<Baron>(bob->get_role());
        if (baron) {
            baron->invest(*bob);
            std::cout << "Bob (Baron) invests 3 coins and gets 6 back" << std::endl;
            std::cout << "Bob coins after invest: " << bob->get_coins() << std::endl;
        }
    }
    
    game.next_turn();
    
    // Show final state
    std::cout << "\n=== Current Game State ===" << std::endl;
    std::cout << "Active players: ";
    for (const auto& name : game.players_names()) {
        std::cout << name << " ";
    }
    std::cout << std::endl;
    
    std::cout << "\nPlayer coins:" << std::endl;
    std::cout << "Alice: " << alice->get_coins() << " coins" << std::endl;
    std::cout << "Bob: " << bob->get_coins() << " coins" << std::endl;
    std::cout << "Charlie: " << charlie->get_coins() << " coins" << std::endl;
    
    // Demonstrate arrest
    std::cout << "\n--- Arrest Demo ---" << std::endl;
    if (alice->get_coins() > 0) {
        std::cout << "Bob arrests Alice" << std::endl;
        bob->arrest(*alice, game);
        std::cout << "Alice coins after arrest: " << alice->get_coins() << std::endl;
        std::cout << "Bob coins after arrest: " << bob->get_coins() << std::endl;
    }
    
    // Demonstrate coup (if someone has enough coins)
    std::cout << "\n--- Coup Demo ---" << std::endl;
    if (bob->get_coins() >= 7) {
        std::cout << "Bob performs coup on Charlie" << std::endl;
        bob->coup(*charlie, game);
        game.eliminate_player(charlie.get());
        std::cout << "Charlie is eliminated from the game" << std::endl;
        std::cout << "Bob coins after coup: " << bob->get_coins() << std::endl;
        
        std::cout << "\nRemaining players: ";
        for (const auto& name : game.players_names()) {
            std::cout << name << " ";
        }
        std::cout << std::endl;
    }
    
    std::cout << "\n=== Demo Completed ===" << std::endl;
    
    return 0;
}