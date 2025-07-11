#include "MultiMethodDispatcher.h"
#include <iostream>
#include <string>
#include <memory>

// Base classes for our demo
class Entity {
public:
    Entity(int id, const std::string& name) : m_id(id), m_name(name) {}
    virtual ~Entity() = default;
    
    int getId() const { return m_id; }
    const std::string& getName() const { return m_name; }
    
private:
    int m_id;
    std::string m_name;
};

// Derived entity classes
class PlayerEntity : public Entity {
public:
    PlayerEntity(int id, const std::string& name) : Entity(id, name) {}
    int getScore() const { return m_score; }
    void addScore(int points) { m_score += points; }
    
private:
    int m_score = 0;
};

class CoinEntity : public Entity {
public:
    CoinEntity(int id, const std::string& name, int value) 
        : Entity(id, name), m_value(value), m_collected(false) {}
    
    int getValue() const { return m_value; }
    bool isCollected() const { return m_collected; }
    void collect() { m_collected = true; }
    
private:
    int m_value;
    bool m_collected;
};

class EnemyEntity : public Entity {
public:
    EnemyEntity(int id, const std::string& name, int damage) 
        : Entity(id, name), m_damage(damage) {}
    
    int getDamage() const { return m_damage; }
    
private:
    int m_damage;
};

// Weapon and armor components for damage system
class Weapon {
public:
    Weapon(const std::string& name, int damage) : m_name(name), m_damage(damage) {}
    const std::string& getName() const { return m_name; }
    int getDamage() const { return m_damage; }
    
private:
    std::string m_name;
    int m_damage;
};

class Armor {
public:
    Armor(const std::string& name, int defense) : m_name(name), m_defense(defense) {}
    const std::string& getName() const { return m_name; }
    int getDefense() const { return m_defense; }
    
private:
    std::string m_name;
    int m_defense;
};

// Interactive objects
class Chest {
public:
    enum class State { Locked, Unlocked, Open, Empty };
    
    Chest(const std::string& name, State state = State::Locked) 
        : m_name(name), m_state(state) {}
    
    const std::string& getName() const { return m_name; }
    State getState() const { return m_state; }
    void setState(State state) { m_state = state; }
    
private:
    std::string m_name;
    State m_state;
};

// Demo main function that can be called from main.cpp
int demoMain() {
    std::cout << "===== MULTIMETHOD DISPATCHER DEMONSTRATION =====\n\n";
    
    // Get the dispatcher instance
    auto& dispatcher = MultiMethodDispatcher::getInstance();
    
    // Register collision handlers
    std::cout << "Registering collision handlers...\n";
    
    dispatcher.registerHandler<PlayerEntity, CoinEntity>("collision",
        [](PlayerEntity& player, CoinEntity& coin) {
            if (!coin.isCollected()) {
                player.addScore(coin.getValue());
                coin.collect();
                std::cout << player.getName() << " collected " << coin.getName() 
                          << " worth " << coin.getValue() << " points!" << std::endl;
                std::cout << player.getName() << "'s score is now " << player.getScore() << std::endl;
            }
        }
    );
    
    dispatcher.registerHandler<PlayerEntity, EnemyEntity>("collision",
        [](PlayerEntity& player, EnemyEntity& enemy) {
            std::cout << enemy.getName() << " attacks " << player.getName() 
                      << " for " << enemy.getDamage() << " damage!" << std::endl;
        }
    );
    
    // Register damage calculation handlers
    std::cout << "\nRegistering damage handlers...\n";
    
    dispatcher.registerHandlerWithResult<Weapon, Armor, int>("damage",
        [](Weapon& weapon, Armor& armor) -> int {
            int damage = std::max(1, weapon.getDamage() - armor.getDefense());
            std::cout << weapon.getName() << " hits " << armor.getName() 
                      << " for " << damage << " damage!" << std::endl;
            return damage;
        },
        0 // Default damage value
    );
    
    // Register interaction handlers
    std::cout << "\nRegistering interaction handlers...\n";
    
    dispatcher.registerHandler<PlayerEntity, Chest>("interaction",
        [](PlayerEntity& player, Chest& chest) {
            std::cout << player.getName() << " interacts with " << chest.getName() << std::endl;
            
            switch (chest.getState()) {
                case Chest::State::Locked:
                    std::cout << "The chest is locked!" << std::endl;
                    break;
                case Chest::State::Unlocked:
                    std::cout << "The chest opens!" << std::endl;
                    chest.setState(Chest::State::Open);
                    break;
                case Chest::State::Open:
                    std::cout << "Player takes items from the chest." << std::endl;
                    chest.setState(Chest::State::Empty);
                    player.addScore(50);
                    std::cout << player.getName() << "'s score is now " << player.getScore() << std::endl;
                    break;
                case Chest::State::Empty:
                    std::cout << "The chest is empty." << std::endl;
                    break;
            }
        }
    );
    
    // Create test entities
    PlayerEntity player(1, "Hero");
    CoinEntity goldCoin(2, "Gold Coin", 10);
    CoinEntity silverCoin(3, "Silver Coin", 5);
    EnemyEntity goblin(4, "Goblin", 5);
    
    // Create weapons and armor
    Weapon sword("Steel Sword", 10);
    Weapon axe("Battle Axe", 15);
    Armor leatherArmor("Leather Armor", 3);
    Armor plateArmor("Plate Armor", 8);
    
    // Create interactive objects
    Chest treasureChest("Treasure Chest", Chest::State::Unlocked);
    
    // Test collision system
    std::cout << "\n===== TESTING COLLISION SYSTEM =====\n";
    dispatcher.dispatch(player, goldCoin, "collision");
    dispatcher.dispatch(player, silverCoin, "collision");
    dispatcher.dispatch(player, goblin, "collision");
    
    // Test damage system
    std::cout << "\n===== TESTING DAMAGE SYSTEM =====\n";
    int damage1 = dispatcher.dispatchWithResult<int>(sword, leatherArmor, "damage");
    int damage2 = dispatcher.dispatchWithResult<int>(axe, plateArmor, "damage");
    std::cout << "Final damage values: " << damage1 << ", " << damage2 << std::endl;
    
    // Test interaction system
    std::cout << "\n===== TESTING INTERACTION SYSTEM =====\n";
    dispatcher.dispatch(player, treasureChest, "interaction");
    dispatcher.dispatch(player, treasureChest, "interaction");
    dispatcher.dispatch(player, treasureChest, "interaction");
    
    // Debug output
    std::cout << "\n===== REGISTERED HANDLERS =====\n";
    auto domains = dispatcher.getDomains();
    for (const auto& domain : domains) {
        dispatcher.debugPrintHandlers(domain);
    }
    
    std::cout << "\nDemo completed successfully!\n";
    return 0;
}

// If this file is compiled as a standalone executable
#ifdef STANDALONE_DEMO
int main() {
    return demoMain();
}
#endif