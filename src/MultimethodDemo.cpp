#include "MultiMethodDispatcher.h"
#include "DamageComponents.h"
#include "Entity.h"
#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>

/**
 * Player class that can interact with objects
 */
class Player {
public:
    Player(const std::string& name) 
        : m_name(name), 
          m_stats(100, 100, 50, 50),
          m_weapon(WeaponComponent::WeaponType::Sword, 10),
          m_armor(ArmorComponent::ArmorType::Leather, 5) {}
    
    std::string getName() const { return m_name; }
    EntityStats& getStats() { return m_stats; }
    WeaponComponent& getWeapon() { return m_weapon; }
    ArmorComponent& getArmor() { return m_armor; }
    
    void say(const std::string& message) {
        std::cout << m_name << " says: \"" << message << "\"" << std::endl;
    }
    
private:
    std::string m_name;
    EntityStats m_stats;
    WeaponComponent m_weapon;
    ArmorComponent m_armor;
};

/**
 * Enemy class with different weapon and armor types
 */
class Enemy {
public:
    Enemy(const std::string& name, WeaponComponent::WeaponType weaponType, ArmorComponent::ArmorType armorType)
        : m_name(name), 
          m_stats(50, 50, 30, 30),
          m_weapon(weaponType, 8),
          m_armor(armorType, 3) {}
    
    std::string getName() const { return m_name; }
    EntityStats& getStats() { return m_stats; }
    WeaponComponent& getWeapon() { return m_weapon; }
    ArmorComponent& getArmor() { return m_armor; }
    
    void say(const std::string& message) {
        std::cout << m_name << " says: \"" << message << "\"" << std::endl;
    }
    
private:
    std::string m_name;
    EntityStats m_stats;
    WeaponComponent m_weapon;
    ArmorComponent m_armor;
};

/**
 * Boss class with special damage calculations
 */
class Boss : public Enemy {
public:
    Boss(const std::string& name) 
        : Enemy(name, WeaponComponent::WeaponType::Lightning, ArmorComponent::ArmorType::Magic),
          m_specialAbilityCooldown(0) {}
    
    bool canUseSpecialAbility() const { return m_specialAbilityCooldown <= 0; }
    void useSpecialAbility() {
        if (canUseSpecialAbility()) {
            std::cout << getName() << " uses special ability!" << std::endl;
            m_specialAbilityCooldown = 3;
        } else {
            std::cout << getName() << "'s special ability is on cooldown!" << std::endl;
        }
    }
    
    void update() {
        if (m_specialAbilityCooldown > 0) {
            m_specialAbilityCooldown--;
        }
    }
    
private:
    int m_specialAbilityCooldown;
};

/**
 * Potion for healing
 */
class HealingPotion {
public:
    HealingPotion(int healAmount) : m_healAmount(healAmount), m_used(false) {}
    
    int getHealAmount() const { return m_healAmount; }
    bool isUsed() const { return m_used; }
    void setUsed(bool used) { m_used = used; }
    
private:
    int m_healAmount;
    bool m_used;
};

/**
 * Transparent object for render order testing
 */
class TransparentObject {
public:
    TransparentObject(const std::string& name, RenderableComponent::RenderLayer layer)
        : m_name(name), m_renderComponent(layer, true) {}
    
    std::string getName() const { return m_name; }
    RenderableComponent& getRenderComponent() { return m_renderComponent; }
    
private:
    std::string m_name;
    RenderableComponent m_renderComponent;
};

/**
 * Opaque object for render order testing
 */
class OpaqueObject {
public:
    OpaqueObject(const std::string& name, RenderableComponent::RenderLayer layer)
        : m_name(name), m_renderComponent(layer, false) {}
    
    std::string getName() const { return m_name; }
    RenderableComponent& getRenderComponent() { return m_renderComponent; }
    
private:
    std::string m_name;
    RenderableComponent m_renderComponent;
};

/**
 * Setup function to register all handlers for damage system
 */
void setupDamageSystem() {
    std::cout << "Setting up damage calculation system...\n";
    
    // Register weapon vs armor damage calculations
    DamageSystem::registerHandler<WeaponComponent, ArmorComponent>(
        [](WeaponComponent& weapon, ArmorComponent& armor) -> int {
            int damage = weapon.getBaseDamage();
            
            // Apply effectiveness based on weapon vs armor type
            if (weapon.getType() == WeaponComponent::WeaponType::Sword) {
                if (armor.getType() == ArmorComponent::ArmorType::Leather) {
                    damage *= 1.5f; // Swords are effective against leather
                } else if (armor.getType() == ArmorComponent::ArmorType::Plate) {
                    damage *= 0.7f; // Swords are less effective against plate
                }
            } else if (weapon.getType() == WeaponComponent::WeaponType::Axe) {
                if (armor.getType() == ArmorComponent::ArmorType::Plate) {
                    damage *= 1.2f; // Axes are somewhat effective against plate
                }
            } else if (weapon.getType() == WeaponComponent::WeaponType::Fire) {
                if (armor.getType() == ArmorComponent::ArmorType::Leather) {
                    damage *= 2.0f; // Fire is very effective against leather
                } else if (armor.getType() == ArmorComponent::ArmorType::Magic) {
                    damage *= 0.5f; // Fire is resisted by magic armor
                }
            } else if (weapon.getType() == WeaponComponent::WeaponType::Ice) {
                if (armor.getType() == ArmorComponent::ArmorType::Magic) {
                    damage *= 0.3f; // Ice is strongly resisted by magic armor
                }
            } else if (weapon.getType() == WeaponComponent::WeaponType::Lightning) {
                if (armor.getType() == ArmorComponent::ArmorType::Chainmail) {
                    damage *= 2.5f; // Lightning is super effective against metal
                } else if (armor.getType() == ArmorComponent::ArmorType::Plate) {
                    damage *= 2.0f; // Lightning is effective against metal
                }
            }
            
            // Apply armor defense
            damage = std::max(1, damage - armor.getDefense());
            
            std::cout << "Damage calculation: " << weapon.getTypeName() << " vs " 
                      << armor.getTypeName() << " = " << damage << std::endl;
                      
            return damage;
        }
    );
    
    // Special calculation for Player vs Enemy
    DamageSystem::registerHandler<Player, Enemy>(
        [](Player& player, Enemy& enemy) -> int {
            // Calculate base damage using weapon vs armor
            int damage = DamageSystem::calculateDamage(player.getWeapon(), enemy.getArmor());
            
            // Apply player-specific bonuses
            damage += 2; // Player has +2 strength
            
            std::cout << player.getName() << " attacks " << enemy.getName() 
                      << " for " << damage << " damage!" << std::endl;
            
            return damage;
        }
    );
    
    // Special calculation for Enemy vs Player
    DamageSystem::registerHandler<Enemy, Player>(
        [](Enemy& enemy, Player& player) -> int {
            // Calculate base damage using weapon vs armor
            int damage = DamageSystem::calculateDamage(enemy.getWeapon(), player.getArmor());
            
            std::cout << enemy.getName() << " attacks " << player.getName() 
                      << " for " << damage << " damage!" << std::endl;
            
            return damage;
        }
    );
    
    // Special calculation for Boss vs Player (bosses deal more damage)
    DamageSystem::registerHandler<Boss, Player>(
        [](Boss& boss, Player& player) -> int {
            // Calculate base damage using weapon vs armor
            int damage = DamageSystem::calculateDamage(boss.getWeapon(), player.getArmor());
            
            // Bosses deal 50% more damage
            damage = static_cast<int>(damage * 1.5f);
            
            if (boss.canUseSpecialAbility()) {
                damage *= 2;
                boss.useSpecialAbility();
            }
            
            std::cout << boss.getName() << " attacks " << player.getName() 
                      << " for " << damage << " critical damage!" << std::endl;
            
            return damage;
        }
    );
}

/**
 * Setup function to register all handlers for interaction system
 */
void setupInteractionSystem() {
    std::cout << "Setting up interaction system...\n";
    
    // Player interaction with chests
    InteractionSystem::registerHandler<Player, Chest>(
        [](Player& player, Chest& chest) {
            player.say("I'm opening a chest...");
            
            switch (chest.getState()) {
                case Chest::ChestState::Locked:
                    player.say("This chest is locked!");
                    break;
                    
                case Chest::ChestState::Unlocked:
                    player.say("Opening the chest...");
                    chest.setState(Chest::ChestState::Open);
                    std::cout << "The chest opens!" << std::endl;
                    break;
                    
                case Chest::ChestState::Open:
                    player.say("Found some loot!");
                    chest.setState(Chest::ChestState::Empty);
                    std::cout << "You collect items from the chest." << std::endl;
                    break;
                    
                case Chest::ChestState::Empty:
                    player.say("This chest is empty.");
                    break;
            }
        }
    );
    
    // Player interaction with doors
    InteractionSystem::registerHandler<Player, Door>(
        [](Player& player, Door& door) {
            player.say("I'm checking a door...");
            
            switch (door.getState()) {
                case Door::DoorState::Locked:
                    player.say("This door is locked!");
                    break;
                    
                case Door::DoorState::Unlocked:
                    player.say("Opening the door...");
                    door.setState(Door::DoorState::Open);
                    std::cout << "The door swings open!" << std::endl;
                    break;
                    
                case Door::DoorState::Open:
                    player.say("This door is already open.");
                    break;
            }
        }
    );
    
    // Player interaction with levers
    InteractionSystem::registerHandler<Player, Lever>(
        [](Player& player, Lever& lever) {
            player.say("I'm pulling a lever...");
            lever.toggle();
            
            if (lever.getState() == Lever::LeverState::On) {
                std::cout << "The lever clicks into place. You hear a mechanism activate!" << std::endl;
            } else {
                std::cout << "The lever returns to its original position. The mechanism deactivates." << std::endl;
            }
        }
    );
    
    // Player using healing potions
    InteractionSystem::registerHandler<Player, HealingPotion>(
        [](Player& player, HealingPotion& potion) {
            if (!potion.isUsed()) {
                player.say("Drinking a healing potion...");
                player.getStats().heal(potion.getHealAmount());
                potion.setUsed(true);
            } else {
                player.say("This potion is empty!");
            }
        }
    );
}

/**
 * Setup function to register all handlers for AI communication
 */
void setupAICommunicationSystem() {
    std::cout << "Setting up AI communication system...\n";
    
    // Enemy cooperation with other enemies
    AICommunicationSystem::registerHandler<Enemy, Enemy>(
        [](Enemy& enemy1, Enemy& enemy2) {
            std::cout << enemy1.getName() << " signals to " << enemy2.getName() 
                      << " to coordinate an attack!" << std::endl;
            
            enemy1.say("Let's flank the player!");
            enemy2.say("I'll go left, you go right!");
        }
    );
    
    // Boss commanding enemies
    AICommunicationSystem::registerHandler<Boss, Enemy>(
        [](Boss& boss, Enemy& minion) {
            std::cout << boss.getName() << " commands " << minion.getName() << "!" << std::endl;
            
            boss.say("Attack the intruder!");
            minion.say("Yes, master!");
        }
    );
}

/**
 * Setup function to register all handlers for render order system
 */
void setupRenderOrderSystem() {
    std::cout << "Setting up render order system...\n";
    
    // Compare render layers
    RenderOrderSystem::registerHandler<RenderableComponent, RenderableComponent>(
        [](RenderableComponent& obj1, RenderableComponent& obj2) -> bool {
            // First compare by layer
            if (obj1.getLayer() != obj2.getLayer()) {
                return static_cast<int>(obj1.getLayer()) < static_cast<int>(obj2.getLayer());
            }
            
            // If same layer, transparent objects are rendered after opaque ones
            if (obj1.isTransparent() != obj2.isTransparent()) {
                return !obj1.isTransparent() && obj2.isTransparent();
            }
            
            // If both are transparent or both opaque, no specific order
            return false;
        }
    );
    
    // Helper for transparent/opaque objects
    RenderOrderSystem::registerHandler<TransparentObject, OpaqueObject>(
        [](TransparentObject& transparent, OpaqueObject& opaque) -> bool {
            return RenderOrderSystem::shouldRenderBefore(
                transparent.getRenderComponent(),
                opaque.getRenderComponent()
            );
        }
    );
}

/**
 * Main function to demonstrate the MultiMethodDispatcher
 */
void runMultimethodDemo() {
    std::cout << "===== MULTIMETHOD DISPATCHER DEMONSTRATION =====\n\n";
    
    // Setup systems
    setupDamageSystem();
    setupInteractionSystem();
    setupAICommunicationSystem();
    setupRenderOrderSystem();
    
    // Create entities
    Player player("Hero");
    Enemy goblin("Goblin", WeaponComponent::WeaponType::Axe, ArmorComponent::ArmorType::Leather);
    Enemy knight("Knight", WeaponComponent::WeaponType::Sword, ArmorComponent::ArmorType::Plate);
    Boss dragon("Dragon");
    
    // Create interactive objects
    Chest treasureChest(Chest::ChestState::Unlocked);
    Door lockedDoor(Door::DoorState::Locked);
    Lever wallLever;
    HealingPotion healthPotion(30);
    
    // Create renderable objects
    TransparentObject glassWindow("Window", RenderableComponent::RenderLayer::Object);
    OpaqueObject stone("Stone", RenderableComponent::RenderLayer::Object);
    TransparentObject magicEffect("Magic", RenderableComponent::RenderLayer::Effect);
    OpaqueObject character("Character", RenderableComponent::RenderLayer::Character);
    
    std::cout << "\n===== DAMAGE SYSTEM DEMO =====\n";
    
    // Demonstrate damage system
    int damageToGoblin = DamageSystem::calculateDamage(player, goblin);
    goblin.getStats().takeDamage(damageToGoblin);
    
    int damageToPlayer = DamageSystem::calculateDamage(knight, player);
    player.getStats().takeDamage(damageToPlayer);
    
    int damageToBoss = DamageSystem::calculateDamage(player, dragon);
    dragon.getStats().takeDamage(damageToBoss);
    
    int bossAttack = DamageSystem::calculateDamage(dragon, player);
    player.getStats().takeDamage(bossAttack);
    
    std::cout << "\n===== INTERACTION SYSTEM DEMO =====\n";
    
    // Demonstrate interaction system
    InteractionSystem::interact(player, treasureChest);
    InteractionSystem::interact(player, treasureChest);
    InteractionSystem::interact(player, lockedDoor);
    InteractionSystem::interact(player, wallLever);
    InteractionSystem::interact(player, healthPotion);
    InteractionSystem::interact(player, healthPotion); // Already used
    
    std::cout << "\n===== AI COMMUNICATION SYSTEM DEMO =====\n";
    
    // Demonstrate AI communication
    AICommunicationSystem::communicate(goblin, knight);
    AICommunicationSystem::communicate(dragon, goblin);
    
    std::cout << "\n===== RENDER ORDER SYSTEM DEMO =====\n";
    
    // Create a list of renderable items
    std::vector<std::pair<std::string, RenderableComponent*>> renderables;
    renderables.emplace_back("Glass Window", &glassWindow.getRenderComponent());
    renderables.emplace_back("Stone", &stone.getRenderComponent());
    renderables.emplace_back("Magic Effect", &magicEffect.getRenderComponent());
    renderables.emplace_back("Character", &character.getRenderComponent());
    
    // Sort them by render order
    std::sort(renderables.begin(), renderables.end(), 
        [](const auto& a, const auto& b) {
            return RenderOrderSystem::shouldRenderBefore(*a.second, *b.second);
        }
    );
    
    // Print the sorted render order
    std::cout << "Render order:\n";
    for (const auto& item : renderables) {
        std::string type = item.second->isTransparent() ? "Transparent" : "Opaque";
        std::cout << "  " << item.first << " (Layer: " 
                  << static_cast<int>(item.second->getLayer()) << ", " << type << ")\n";
    }
    
    // Test direct comparison
    bool windowBeforeStone = RenderOrderSystem::shouldRenderBefore(glassWindow, stone);
    std::cout << "Should render window before stone? " << (windowBeforeStone ? "Yes" : "No") << std::endl;
}

// Function to be called from main application
void runDispatcherDemo() {
    runMultimethodDemo();
    runCollisionAdapterTest();
}