# Coup Game Implementation

## Description
This is a C++ implementation of the Coup card game for 2-6 players. The game involves strategy, bluffing, and resource management where players compete to be the last one standing by performing coups and using special role abilities.

## Project Structure
```
coup_assignment/
├── include/          # Header files
│   ├── Player.hpp    # Player class definition
│   ├── Role.hpp      # Role classes definitions
│   └── Game.hpp      # Game board class definition
├── src/              # Source files
│   ├── Player.cpp    # Player implementation
│   ├── Role.cpp      # Roles implementation
│   ├── Game.cpp      # Game logic implementation
│   └── Demo.cpp      # Demo program
├── tests/            # Test files
│   ├── Test.cpp      # Unit tests
│   └── doctest.hpp   # Testing framework
├── Makefile          # Build configuration
└── README.md         # This file
```

## Game Rules

### Basic Actions
Every player can perform these actions regardless of their role:
- **Gather**: Take 1 coin from the treasury (blockable by sanction)
- **Tax**: Take 2 coins from the treasury (blockable by sanction and Governor)
- **Bribe**: Pay 4 coins to get an extra turn
- **Arrest**: Take 1 coin from another player (cannot target same player twice in a row)
- **Sanction**: Pay 3 coins to block a player's economic actions for one turn
- **Coup**: Pay 7 coins to eliminate another player from the game

### Roles and Special Abilities
- **Governor**: Gets 3 coins from tax instead of 2, can block other players' tax actions
- **Spy**: Can see other players' coins and block their arrest attempts
- **Baron**: Can invest 3 coins to get 6 back, receives compensation when sanctioned
- **General**: Can pay 5 coins to block a coup, gets coin back when arrested
- **Judge**: Can block bribe actions, sanctioning player pays extra when targeting Judge
- **Merchant**: Gets 1 bonus coin at turn start if has 3+ coins, pays treasury instead of player when arrested

### Special Rules
- Players start with 2 coins
- If a player has 10+ coins at turn start, they must perform a coup
- Game ends when only one player remains active

## Building the Project

### Prerequisites
- C++ compiler with C++17 support (clang++)
- Make utility
- Valgrind

### Compilation Commands

Build all executables:
```bash
make
```

Run the demo:
```bash
make Main
```

Run tests:
```bash
make test
```

Check for memory leaks:
```bash
make valgrind
```

Clean build files:
```bash
make clean
```

## Class Architecture

### Player Class
Manages player state including:
- Name and coin count
- Active/sanctioned status
- Role assignment
- Action execution

### Role Classes
Hierarchy of role implementations:
- Base `Role` class with virtual methods
- Derived classes for each specific role (Governor, Spy, Baron, General, Judge, Merchant)
- Each role implements special abilities and blocking capabilities

### Game Class
Controls game flow:
- Player management and turn order
- Action history and blocking system
- Game state (active players, winner determination)
- Treasury management

## Testing
The project includes comprehensive unit tests covering:
- Player creation and coin management
- All basic actions and their constraints
- Role-specific abilities
- Game state management
- Edge cases and error handling

## Implementation Notes
- Uses smart pointers (shared_ptr) for memory management
- Implements exception handling for invalid actions
- Follows RAII principles
- Modular design with clear separation of concerns

## Author
yaacovkrawiec@gmail.com