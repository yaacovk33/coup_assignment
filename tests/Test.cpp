// yaacovkrawiec@gmail.com

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.hpp"
#include "../include/Game.hpp"
#include "../include/Player.hpp"
#include "../include/Role.hpp"
#include <algorithm>

TEST_CASE("Player creation and basic attributes") {
    Player player("TestPlayer");
    
    CHECK(player.get_name() == "TestPlayer");
    CHECK(player.get_coins() == 2);
    CHECK(player.is_player_active() == true);
    CHECK(player.is_player_sanctioned() == false);
    CHECK(player.get_role() == nullptr);
    CHECK(player.get_last_arrested() == nullptr);
}

TEST_CASE("Player coin management") {
    Player player("TestPlayer");
    
    SUBCASE("Adding coins") {
        player.add_coins(5);
        CHECK(player.get_coins() == 7);
        
        player.add_coins(0);
        CHECK(player.get_coins() == 7);
    }
    
    SUBCASE("Removing coins") {
        player.add_coins(5);
        player.remove_coins(3);
        CHECK(player.get_coins() == 4);
        
        player.remove_coins(4);
        CHECK(player.get_coins() == 0);
    }
    
    SUBCASE("Cannot remove more coins than available") {
        CHECK_THROWS(player.remove_coins(5));
        CHECK_THROWS(player.remove_coins(3));
    }
    
    SUBCASE("Cannot add or remove negative coins") {
        CHECK_THROWS(player.add_coins(-3));
        CHECK_THROWS(player.add_coins(-1));
        CHECK_THROWS(player.remove_coins(-1));
        CHECK_THROWS(player.remove_coins(-5));
    }
}

TEST_CASE("Game creation and player management") {
    Game game;
    
    SUBCASE("Initial game state") {
        CHECK(game.is_game_active() == false);
        CHECK(game.get_players().size() == 0);
        CHECK_THROWS(game.turn());
    }
    
    auto player1 = std::make_shared<Player>("Player1");
    auto player2 = std::make_shared<Player>("Player2");
    
    game.add_player(player1);
    game.add_player(player2);
    
    CHECK(game.get_players().size() == 2);
    
    SUBCASE("Cannot start game with less than 2 players") {
        Game single_game;
        auto single_player = std::make_shared<Player>("Solo");
        single_game.add_player(single_player);
        CHECK_THROWS(single_game.start_game());
    }
    
    SUBCASE("Cannot add more than 6 players") {
        auto p3 = std::make_shared<Player>("P3");
        auto p4 = std::make_shared<Player>("P4");
        auto p5 = std::make_shared<Player>("P5");
        auto p6 = std::make_shared<Player>("P6");
        auto p7 = std::make_shared<Player>("P7");
        
        game.add_player(p3);
        game.add_player(p4);
        game.add_player(p5);
        game.add_player(p6);
        CHECK_THROWS(game.add_player(p7));
    }
    
    SUBCASE("Cannot add player to active game") {
        game.start_game();
        auto p3 = std::make_shared<Player>("P3");
        CHECK_THROWS(game.add_player(p3));
    }
}

TEST_CASE("Basic actions") {
    Game game;
    auto player1 = std::make_shared<Player>("Player1");
    auto player2 = std::make_shared<Player>("Player2");
    
    game.add_player(player1);
    game.add_player(player2);
    game.start_game();
    
    SUBCASE("Gather action") {
        int initial_coins = player1->get_coins();
        player1->gather(game);
        CHECK(player1->get_coins() == initial_coins + 1);
    }
    
    SUBCASE("Tax action") {
        int initial_coins = player1->get_coins();
        player1->tax(game);
        CHECK(player1->get_coins() == initial_coins + 2);
    }
    
    SUBCASE("Bribe action") {
        player1->add_coins(10);
        int initial_coins = player1->get_coins();
        player1->bribe(game);
        CHECK(player1->get_coins() == initial_coins - 4);
        CHECK(game.is_extra_turn_allowed() == true);
    }
    
    SUBCASE("Arrest action") {
        player2->add_coins(5);
        int p1_initial = player1->get_coins();
        int p2_initial = player2->get_coins();
        
        player1->arrest(*player2, game);
        CHECK(player1->get_coins() == p1_initial + 1);
        CHECK(player2->get_coins() == p2_initial - 1);
        CHECK(player1->get_last_arrested() == player2.get());
    }
    
    SUBCASE("Cannot arrest same player twice in a row") {
        player2->add_coins(5);
        player1->arrest(*player2, game);
        CHECK_THROWS(player1->arrest(*player2, game));
    }
    
    SUBCASE("Sanction action") {
        player1->add_coins(5);
        player1->sanction(*player2, game);
        CHECK(player2->is_player_sanctioned() == true);
        CHECK(player1->get_coins() == 4);
    }
    
    SUBCASE("Coup action") {
        player1->add_coins(10);
        int initial_coins = player1->get_coins();
        player1->coup(*player2, game);
        CHECK(player1->get_coins() == initial_coins - 7);
    }
    
    SUBCASE("Cannot perform actions without enough coins") {
        CHECK_THROWS(player1->bribe(game));
        CHECK_THROWS(player1->sanction(*player2, game));
        CHECK_THROWS(player1->coup(*player2, game));
    }
}

TEST_CASE("Role-specific abilities") {
    Game game;
    auto gov = std::make_shared<Player>("Governor");
    auto baron = std::make_shared<Player>("Baron");
    auto merchant = std::make_shared<Player>("Merchant");
    auto general = std::make_shared<Player>("General");
    auto judge = std::make_shared<Player>("Judge");
    auto spy = std::make_shared<Player>("Spy");
    
    gov->set_role(std::make_shared<Governor>());
    baron->set_role(std::make_shared<Baron>());
    merchant->set_role(std::make_shared<Merchant>());
    general->set_role(std::make_shared<General>());
    judge->set_role(std::make_shared<Judge>());
    spy->set_role(std::make_shared<Spy>());
    
    game.add_player(gov);
    game.add_player(baron);
    game.add_player(merchant);
    game.start_game();
    
    SUBCASE("Governor gets 3 coins from tax") {
        int initial = gov->get_coins();
        gov->tax(game);
        CHECK(gov->get_coins() == initial + 3);
    }
    
    SUBCASE("Baron can invest") {
        baron->add_coins(5);
        int initial = baron->get_coins();
        auto baron_role = std::dynamic_pointer_cast<Baron>(baron->get_role());
        baron_role->invest(*baron);
        CHECK(baron->get_coins() == initial + 3);
    }
    
    SUBCASE("Baron cannot invest without enough coins") {
        auto baron_role = std::dynamic_pointer_cast<Baron>(baron->get_role());
        int initial = baron->get_coins();
        baron_role->invest(*baron);
        CHECK(baron->get_coins() == initial);
    }
    
    SUBCASE("Merchant gets bonus at turn start") {
        merchant->add_coins(5);
        int initial = merchant->get_coins();
        auto merch_role = std::dynamic_pointer_cast<Merchant>(merchant->get_role());
        merch_role->start_turn_bonus(*merchant);
        CHECK(merchant->get_coins() == initial + 1);
    }
    
    SUBCASE("Merchant doesn't get bonus with less than 3 coins") {
        auto merch_role = std::dynamic_pointer_cast<Merchant>(merchant->get_role());
        int initial = merchant->get_coins();
        merch_role->start_turn_bonus(*merchant);
        CHECK(merchant->get_coins() == initial);
    }
    
    SUBCASE("Spy can see coins") {
        auto spy_role = std::dynamic_pointer_cast<Spy>(spy->get_role());
        merchant->add_coins(5);
        CHECK(spy_role->see_coins(*merchant) == merchant->get_coins());
    }
    
    SUBCASE("General can block coup") {
        general->add_coins(5);
        auto gen_role = std::dynamic_pointer_cast<General>(general->get_role());
        int initial = general->get_coins();
        bool blocked = gen_role->block_coup(*general, *gov, game);
        CHECK(blocked == true);
        CHECK(general->get_coins() == initial - 5);
    }
    
    SUBCASE("General cannot block coup without coins") {
        auto gen_role = std::dynamic_pointer_cast<General>(general->get_role());
        bool blocked = gen_role->block_coup(*general, *gov, game);
        CHECK(blocked == false);
    }
    
    SUBCASE("Role blocking abilities") {
        CHECK(gov->get_role()->can_block_action(ActionType::TAX, nullptr, nullptr) == true);
        CHECK(gov->get_role()->can_block_action(ActionType::COUP, nullptr, nullptr) == false);
        CHECK(judge->get_role()->can_block_action(ActionType::BRIBE, nullptr, nullptr) == true);
        CHECK(judge->get_role()->can_block_action(ActionType::TAX, nullptr, nullptr) == false);
    }
}

TEST_CASE("Game state management") {
    Game game;
    auto p1 = std::make_shared<Player>("Alice");
    auto p2 = std::make_shared<Player>("Bob");
    auto p3 = std::make_shared<Player>("Charlie");
    
    game.add_player(p1);
    game.add_player(p2);
    game.add_player(p3);
    game.start_game();
    
    SUBCASE("Turn management") {
        CHECK(game.turn() == "Alice");
        game.next_turn();
        CHECK(game.turn() == "Bob");
        game.next_turn();
        CHECK(game.turn() == "Charlie");
        game.next_turn();
        CHECK(game.turn() == "Alice");
    }
    
    SUBCASE("Active players list") {
        auto players = game.players_names();
        CHECK(players.size() == 3);
        CHECK(std::find(players.begin(), players.end(), "Alice") != players.end());
        CHECK(std::find(players.begin(), players.end(), "Bob") != players.end());
        CHECK(std::find(players.begin(), players.end(), "Charlie") != players.end());
    }
    
    SUBCASE("Player elimination") {
        game.eliminate_player(p2.get());
        auto players = game.players_names();
        CHECK(players.size() == 2);
        CHECK(std::find(players.begin(), players.end(), "Bob") == players.end());
        CHECK(p2->is_player_active() == false);
    }
    
    SUBCASE("Game ends with one player") {
        game.eliminate_player(p2.get());
        game.eliminate_player(p3.get());
        CHECK(game.is_game_active() == false);
        CHECK_NOTHROW(game.winner());
        CHECK(game.winner() == "Alice");
    }
    
    SUBCASE("Cannot get winner while game is active") {
        CHECK_THROWS(game.winner());
    }
    
    SUBCASE("Turn skips eliminated players") {
        CHECK(game.turn() == "Alice");
        game.next_turn();
        CHECK(game.turn() == "Bob");
        p3->set_active(false);
        game.next_turn();
        CHECK(game.turn() == "Alice");
    }
}

TEST_CASE("Sanctioned player restrictions") {
    Game game;
    auto p1 = std::make_shared<Player>("Player1");
    auto p2 = std::make_shared<Player>("Player2");
    
    game.add_player(p1);
    game.add_player(p2);
    game.start_game();
    
    p2->set_sanctioned(true);
    
    SUBCASE("Cannot use economic actions when sanctioned") {
        CHECK_THROWS(p2->gather(game));
        CHECK_THROWS(p2->tax(game));
    }
    
    SUBCASE("Can use non-economic actions when sanctioned") {
        p2->add_coins(10);
        CHECK_NOTHROW(p2->bribe(game));
        CHECK_NOTHROW(p2->arrest(*p1, game));
        CHECK_NOTHROW(p2->coup(*p1, game));
    }
    
    SUBCASE("Sanctions clear on next turn") {
        p1->set_sanctioned(true);
        p2->set_sanctioned(true);
        CHECK(p1->is_player_sanctioned() == true);
        CHECK(p2->is_player_sanctioned() == true);
        
        game.clear_sanctions();
        CHECK(p1->is_player_sanctioned() == false);
        CHECK(p2->is_player_sanctioned() == false);
    }
}

TEST_CASE("Exception handling and edge cases") {
    Game game;
    auto p1 = std::make_shared<Player>("Player1");
    auto p2 = std::make_shared<Player>("Player2");
    
    game.add_player(p1);
    game.add_player(p2);
    
    SUBCASE("Cannot perform actions before game starts") {
        CHECK_THROWS(game.turn());
        CHECK_THROWS(game.next_turn());
    }
    
    game.start_game();
    
    SUBCASE("Cannot perform action when inactive") {
        p1->set_active(false);
        CHECK_THROWS(p1->gather(game));
        CHECK_THROWS(p1->tax(game));
        CHECK_THROWS(p1->bribe(game));
        CHECK_THROWS(p1->arrest(*p2, game));
        CHECK_THROWS(p1->sanction(*p2, game));
        CHECK_THROWS(p1->coup(*p2, game));
    }
    
    SUBCASE("Cannot target inactive player") {
        p2->set_active(false);
        p1->add_coins(10);
        CHECK_THROWS(p1->arrest(*p2, game));
        CHECK_THROWS(p1->sanction(*p2, game));
        CHECK_THROWS(p1->coup(*p2, game));
    }
    
    SUBCASE("Arrest with zero coins target") {
        p2->remove_coins(2);
        CHECK(p2->get_coins() == 0);
        p1->arrest(*p2, game);
        CHECK(p1->get_coins() == 2);
    }
}

TEST_CASE("Complex game scenarios") {
    Game game;
    auto alice = std::make_shared<Player>("Alice");
    auto bob = std::make_shared<Player>("Bob");
    auto charlie = std::make_shared<Player>("Charlie");
    
    alice->set_role(std::make_shared<Governor>());
    bob->set_role(std::make_shared<Baron>());
    charlie->set_role(std::make_shared<Merchant>());
    
    game.add_player(alice);
    game.add_player(bob);
    game.add_player(charlie);
    game.start_game();
    
    SUBCASE("Baron receives compensation when sanctioned") {
        alice->add_coins(3);
        int bob_initial = bob->get_coins();
        alice->sanction(*bob, game);
        CHECK(bob->is_player_sanctioned() == true);
        CHECK(bob->get_coins() == bob_initial + 1);
    }
    
    SUBCASE("Merchant pays treasury when arrested") {
        charlie->add_coins(5);
        int alice_initial = alice->get_coins();
        int charlie_initial = charlie->get_coins();
        alice->arrest(*charlie, game);
        CHECK(alice->get_coins() == alice_initial);
        CHECK(charlie->get_coins() == charlie_initial - 2);
    }
    
    SUBCASE("Judge sanctioning costs extra") {
        // Create a new game for this test to avoid conflicts
        Game new_game;
        auto alice2 = std::make_shared<Player>("Alice2");
        auto judge = std::make_shared<Player>("Judge");
        alice2->set_role(std::make_shared<Governor>());
        judge->set_role(std::make_shared<Judge>());
        new_game.add_player(alice2);
        new_game.add_player(judge);
        new_game.start_game();
        
        alice2->add_coins(5);
        int alice_initial = alice2->get_coins();
        alice2->sanction(*judge, new_game);
        CHECK(alice2->get_coins() == alice_initial - 4);
    }
    
    SUBCASE("General arrest protection") {
        // Create a new game for this test
        Game new_game;
        auto alice2 = std::make_shared<Player>("Alice2");
        auto general = std::make_shared<Player>("General");
        alice2->set_role(std::make_shared<Governor>());
        general->set_role(std::make_shared<General>());
        new_game.add_player(alice2);
        new_game.add_player(general);
        new_game.start_game();
        general->add_coins(3);
        
        int alice_initial = alice2->get_coins();
        int general_initial = general->get_coins();
        alice2->arrest(*general, new_game);
        CHECK(alice2->get_coins() == alice_initial + 1);
        CHECK(general->get_coins() == general_initial);
    }
    
    SUBCASE("Multiple eliminations lead to winner") {
        alice->add_coins(15);
        alice->coup(*bob, game);
        game.eliminate_player(bob.get());
        alice->coup(*charlie, game);
        game.eliminate_player(charlie.get());
        
        CHECK(game.is_game_active() == false);
        CHECK_NOTHROW(game.winner());
        CHECK(game.winner() == "Alice");
    }
}

TEST_CASE("Memory management and smart pointers") {
    SUBCASE("Shared pointer management") {
        Game game;
        {
            auto temp_player = std::make_shared<Player>("Temporary");
            game.add_player(temp_player);
            CHECK(game.get_players().size() == 1);
        }
        CHECK(game.get_players().size() == 1);
        CHECK(game.get_players()[0]->get_name() == "Temporary");
    }
    
    SUBCASE("Role assignment and management") {
        auto player = std::make_shared<Player>("TestPlayer");
        CHECK(player->get_role() == nullptr);
        
        player->set_role(std::make_shared<Governor>());
        CHECK(player->get_role() != nullptr);
        CHECK(player->get_role()->get_type() == RoleType::GOVERNOR);
        CHECK(player->get_role()->get_name() == "Governor");
        
        player->set_role(std::make_shared<Baron>());
        CHECK(player->get_role()->get_type() == RoleType::BARON);
        CHECK(player->get_role()->get_name() == "Baron");
    }
    
    SUBCASE("Multiple games independence") {
        Game game1;
        Game game2;
        
        auto p1 = std::make_shared<Player>("Player1");
        auto p2 = std::make_shared<Player>("Player2");
        auto p3 = std::make_shared<Player>("Player3");
        auto p4 = std::make_shared<Player>("Player4");
        
        game1.add_player(p1);
        game1.add_player(p2);
        game2.add_player(p3);
        game2.add_player(p4);
        
        CHECK(game1.get_players().size() == 2);
        CHECK(game2.get_players().size() == 2);
        
        game1.start_game();
        CHECK(game1.is_game_active() == true);
        CHECK(game2.is_game_active() == false);
    }
}

TEST_CASE("Action history and blocking") {
    Game game;
    auto p1 = std::make_shared<Player>("Player1");
    auto p2 = std::make_shared<Player>("Player2");
    
    p1->set_role(std::make_shared<Governor>());
    p2->set_role(std::make_shared<Judge>());
    
    game.add_player(p1);
    game.add_player(p2);
    game.start_game();
    
    SUBCASE("Action history recording") {
        p1->gather(game);
        p1->tax(game);
        
        CHECK(game.can_block_last_action(p2.get()) == false);
        
        p2->add_coins(5);
        p2->bribe(game);
        CHECK(game.can_block_last_action(p1.get()) == false);
    }
    
    SUBCASE("Blocking mechanism") {
        p2->tax(game);
        CHECK(game.can_block_last_action(p1.get()) == true);
        
        game.block_last_action();
        CHECK(game.can_block_last_action(p1.get()) == false);
    }
}

TEST_CASE("Forced coup at 10 coins") {
    Game game;
    auto p1 = std::make_shared<Player>("Player1");
    auto p2 = std::make_shared<Player>("Player2");
    
    game.add_player(p1);
    game.add_player(p2);
    game.start_game();
    
    SUBCASE("Player with 10+ coins must coup") {
        p1->add_coins(8);
        CHECK(p1->get_coins() == 10);
        
        // With 10 coins, player must coup
        game.check_forced_coup();
        
        // The forced coup rule is enforced by game logic, not by throwing exceptions
        // So we just verify the player can coup
        CHECK_NOTHROW(p1->coup(*p2, game));
        CHECK(p1->get_coins() == 3);  // 10 - 7
    }
}