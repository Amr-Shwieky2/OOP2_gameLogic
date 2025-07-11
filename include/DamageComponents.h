#pragma once
#include <string>
#include <iostream>

// Forward declaration of our components
class WeaponComponent;
class ArmorComponent;

/**
 * WeaponComponent - Represents a weapon that can deal damage
 */
class WeaponComponent {
public:
    enum class WeaponType {
        Sword,
        Axe,
        Bow,
        Fire,
        Ice,
        Lightning
    };
    
    WeaponComponent(WeaponType type, int baseDamage) 
        : m_type(type), m_baseDamage(baseDamage) {}
    
    WeaponType getType() const { return m_type; }
    int getBaseDamage() const { return m_baseDamage; }
    
    std::string getTypeName() const {
        switch (m_type) {
            case WeaponType::Sword: return "Sword";
            case WeaponType::Axe: return "Axe";
            case WeaponType::Bow: return "Bow";
            case WeaponType::Fire: return "Fire";
            case WeaponType::Ice: return "Ice";
            case WeaponType::Lightning: return "Lightning";
            default: return "Unknown";
        }
    }
    
private:
    WeaponType m_type;
    int m_baseDamage;
};

/**
 * ArmorComponent - Represents armor that can defend against damage
 */
class ArmorComponent {
public:
    enum class ArmorType {
        Leather,
        Chainmail,
        Plate,
        Magic
    };
    
    ArmorComponent(ArmorType type, int defense) 
        : m_type(type), m_defense(defense) {}
    
    ArmorType getType() const { return m_type; }
    int getDefense() const { return m_defense; }
    
    std::string getTypeName() const {
        switch (m_type) {
            case ArmorType::Leather: return "Leather";
            case ArmorType::Chainmail: return "Chainmail";
            case ArmorType::Plate: return "Plate";
            case ArmorType::Magic: return "Magic";
            default: return "Unknown";
        }
    }
    
private:
    ArmorType m_type;
    int m_defense;
};

/**
 * EntityStats - Simple component to represent an entity's stats
 */
class EntityStats {
public:
    EntityStats(int health, int maxHealth, int mana, int maxMana)
        : m_health(health), m_maxHealth(maxHealth), m_mana(mana), m_maxMana(maxMana) {}
    
    int getHealth() const { return m_health; }
    int getMaxHealth() const { return m_maxHealth; }
    int getMana() const { return m_mana; }
    int getMaxMana() const { return m_maxMana; }
    
    void takeDamage(int damage) {
        m_health = std::max(0, m_health - damage);
        std::cout << "Entity took " << damage << " damage! Health: " << m_health << "/" << m_maxHealth << std::endl;
    }
    
    void heal(int amount) {
        m_health = std::min(m_maxHealth, m_health + amount);
        std::cout << "Entity healed for " << amount << "! Health: " << m_health << "/" << m_maxHealth << std::endl;
    }
    
    void useMana(int amount) {
        if (m_mana >= amount) {
            m_mana -= amount;
            std::cout << "Used " << amount << " mana! Mana: " << m_mana << "/" << m_maxMana << std::endl;
        } else {
            std::cout << "Not enough mana! Need " << amount << " but have " << m_mana << std::endl;
        }
    }
    
private:
    int m_health;
    int m_maxHealth;
    int m_mana;
    int m_maxMana;
};

/**
 * Interaction tags for different types of interactive objects
 */
struct InteractiveObject {
    virtual ~InteractiveObject() = default;
    virtual std::string getInteractionType() const = 0;
};

class Chest : public InteractiveObject {
public:
    enum class ChestState { Locked, Unlocked, Open, Empty };
    
    Chest(ChestState state = ChestState::Locked) : m_state(state) {}
    
    ChestState getState() const { return m_state; }
    void setState(ChestState state) { m_state = state; }
    
    std::string getInteractionType() const override { return "Chest"; }
    
private:
    ChestState m_state;
};

class Door : public InteractiveObject {
public:
    enum class DoorState { Locked, Unlocked, Open };
    
    Door(DoorState state = DoorState::Locked) : m_state(state) {}
    
    DoorState getState() const { return m_state; }
    void setState(DoorState state) { m_state = state; }
    
    std::string getInteractionType() const override { return "Door"; }
    
private:
    DoorState m_state;
};

class Lever : public InteractiveObject {
public:
    enum class LeverState { Off, On };
    
    Lever(LeverState state = LeverState::Off) : m_state(state) {}
    
    LeverState getState() const { return m_state; }
    void setState(LeverState state) { m_state = state; }
    void toggle() { m_state = (m_state == LeverState::Off) ? LeverState::On : LeverState::Off; }
    
    std::string getInteractionType() const override { return "Lever"; }
    
private:
    LeverState m_state;
};

/**
 * Rendering components for transparent/opaque objects
 */
class RenderableComponent {
public:
    enum class RenderLayer {
        Background,
        Terrain,
        Object,
        Character,
        Effect,
        UI
    };
    
    RenderableComponent(RenderLayer layer, bool isTransparent)
        : m_layer(layer), m_isTransparent(isTransparent) {}
    
    RenderLayer getLayer() const { return m_layer; }
    bool isTransparent() const { return m_isTransparent; }
    
private:
    RenderLayer m_layer;
    bool m_isTransparent;
};