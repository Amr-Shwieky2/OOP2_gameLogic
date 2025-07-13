#include "PlayerWeaponSystem.h"
#include "PlayerEntity.h"
#include "ProjectileEntity.h"
#include "GameSession.h"
#include <iostream>

// External references
extern int g_nextEntityId;
extern GameSession* g_currentSession;

PlayerWeaponSystem::PlayerWeaponSystem(PlayerEntity& player, b2World& world, TextureManager& textures)
    : m_player(player), m_world(world), m_textures(textures),
    m_lastShotTime(0.0f), m_shotCooldown(0.3f), m_weaponType(WeaponType::Basic) {
}

void PlayerWeaponSystem::update(float dt) {
    m_lastShotTime += dt;
}

void PlayerWeaponSystem::shoot() {
    if (!canShoot()) {
        return;
    }

    if (!g_currentSession) {
        std::cerr << "[WeaponSystem] No active game session for shooting" << std::endl;
        return;
    }

    sf::Vector2f playerPos = m_player.getPosition();
    sf::Vector2f playerVel = m_player.getVelocity();

    // Get the player's state to determine direction
    PlayerState* currentState = m_player.getCurrentState();
    bool isReversedState = (currentState && std::string(currentState->getName()) == "Reversed");

    // Determine shoot direction based on player's state and movement
    sf::Vector2f direction;
    
    // If in reversed state, invert direction logic
    if (isReversedState) {
        direction = playerVel.x <= 0 ? sf::Vector2f(1.f, 0.f) : sf::Vector2f(-1.f, 0.f);
    } else {
        direction = playerVel.x >= 0 ? sf::Vector2f(1.f, 0.f) : sf::Vector2f(-1.f, 0.f);
    }
    
    // If player is not moving, use a default direction
    if (std::abs(playerVel.x) < 0.1f) {
        // Use the last known movement direction or default to right
        direction = sf::Vector2f(1.f, 0.f);
    }
    
    // Offset the projectile position in front of the player
    float offsetX = direction.x * 30.0f;  // Offset in the direction of shooting
    sf::Vector2f projectilePos = playerPos + sf::Vector2f(offsetX, 0);

    createProjectile(projectilePos, direction);

    // Reset shot timer
    m_lastShotTime = 0.0f;

    std::cout << "[WeaponSystem] Shot fired! Type: " << static_cast<int>(m_weaponType) 
              << " Direction: (" << direction.x << "," << direction.y << ")" << std::endl;
}

void PlayerWeaponSystem::shootBackward() {
    if (!canShoot()) {
        return;
    }

    if (!g_currentSession) {
        std::cerr << "[WeaponSystem] No active game session for shooting" << std::endl;
        return;
    }

    sf::Vector2f playerPos = m_player.getPosition();
    sf::Vector2f playerVel = m_player.getVelocity();

    // Get the player's state to determine direction
    PlayerState* currentState = m_player.getCurrentState();
    bool isReversedState = (currentState && std::string(currentState->getName()) == "Reversed");

    // Determine shoot direction based on player's state and movement - but reversed
    sf::Vector2f direction;
    
    // Always shoot in the opposite direction of normal shooting
    if (isReversedState) {
        direction = playerVel.x <= 0 ? sf::Vector2f(-1.f, 0.f) : sf::Vector2f(1.f, 0.f);
    } else {
        direction = playerVel.x >= 0 ? sf::Vector2f(-1.f, 0.f) : sf::Vector2f(1.f, 0.f);
    }
    
    // If player is not moving, use a default backward direction
    if (std::abs(playerVel.x) < 0.1f) {
        // Use the last known movement direction (opposite) or default to left
        direction = sf::Vector2f(-1.f, 0.f);
    }
    
    // Offset the projectile position behind the player
    float offsetX = direction.x * 30.0f;  // Offset in the direction of shooting
    sf::Vector2f projectilePos = playerPos + sf::Vector2f(offsetX, 0);

    createProjectile(projectilePos, direction);

    // Reset shot timer
    m_lastShotTime = 0.0f;

    std::cout << "[WeaponSystem] Backward shot fired! Type: " << static_cast<int>(m_weaponType) 
              << " Direction: (" << direction.x << "," << direction.y << ")" << std::endl;
}

void PlayerWeaponSystem::shootSpecialGravity() {
    if (!canShoot()) {
        return;
    }

    if (!g_currentSession) {
        std::cerr << "[WeaponSystem] No active game session for shooting" << std::endl;
        return;
    }

    sf::Vector2f playerPos = m_player.getPosition();
    
    // Shoot always in -X direction with upward trajectory
    sf::Vector2f direction(-0.8f, -0.6f); // Left and up direction
    
    // Normalize direction vector
    float magnitude = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    direction.x /= magnitude;
    direction.y /= magnitude;
    
    // Offset the projectile position slightly to the left of the player
    sf::Vector2f projectilePos = playerPos + sf::Vector2f(-15.0f, -10.0f);

    // Create gravity-affected projectile
    createGravityProjectile(projectilePos, direction);

    // Reset shot timer
    m_lastShotTime = 0.0f;

    std::cout << "[WeaponSystem] Special gravity shot fired! Direction: (" 
              << direction.x << "," << direction.y << ")" << std::endl;
}

void PlayerWeaponSystem::shootForwardGravity() {
    if (!canShoot()) {
        return;
    }

    if (!g_currentSession) {
        std::cerr << "[WeaponSystem] No active game session for shooting" << std::endl;
        return;
    }

    sf::Vector2f playerPos = m_player.getPosition();
    sf::Vector2f playerVel = m_player.getVelocity();
    
    // Get the player's state to determine direction
    PlayerState* currentState = m_player.getCurrentState();
    bool isReversedState = (currentState && std::string(currentState->getName()) == "Reversed");

    // Determine shoot direction based on player's state and movement
    sf::Vector2f direction;
    
    // If in reversed state, invert direction logic
    if (isReversedState) {
        direction = playerVel.x <= 0 ? sf::Vector2f(1.f, -0.3f) : sf::Vector2f(-1.f, -0.3f);
    } else {
        direction = playerVel.x >= 0 ? sf::Vector2f(1.f, -0.3f) : sf::Vector2f(-1.f, -0.3f);
    }
    
    // If player is not moving, use a default forward direction with slight upward component
    if (std::abs(playerVel.x) < 0.1f) {
        // Default to right with slight upward component
        direction = sf::Vector2f(1.f, -0.3f);
    }
    
    // Normalize direction vector
    float magnitude = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    direction.x /= magnitude;
    direction.y /= magnitude;
    
    // Offset the projectile position slightly in front of the player and slightly up
    float offsetX = direction.x * 30.0f;
    sf::Vector2f projectilePos = playerPos + sf::Vector2f(offsetX, -10.0f);

    // Create gravity-affected projectile
    createGravityProjectile(projectilePos, direction);

    // Reset shot timer
    m_lastShotTime = 0.0f;

    std::cout << "[WeaponSystem] Forward gravity shot fired! Direction: (" 
              << direction.x << "," << direction.y << ")" << std::endl;
}

bool PlayerWeaponSystem::canShoot() const {
    return m_lastShotTime >= getCooldownForWeapon(m_weaponType);
}

void PlayerWeaponSystem::setWeaponType(WeaponType type) {
    if (m_weaponType != type) {
        m_weaponType = type;
        m_shotCooldown = getCooldownForWeapon(type);
        std::cout << "[WeaponSystem] Weapon changed to type: " << static_cast<int>(type) << std::endl;
    }
}

void PlayerWeaponSystem::createProjectile(const sf::Vector2f& position, const sf::Vector2f& direction) {
    try {
        auto projectile = std::make_unique<ProjectileEntity>(
            g_nextEntityId++,
            m_world,
            position.x,
            position.y,
            direction,
            m_textures,
            true // fromPlayer = true
        );

        g_currentSession->spawnEntity(std::move(projectile));
    }
    catch (const std::exception& e) {
        std::cerr << "[WeaponSystem] Error creating projectile: " << e.what() << std::endl;
    }
}

void PlayerWeaponSystem::createGravityProjectile(const sf::Vector2f& position, const sf::Vector2f& direction) {
    try {
        // Create a projectile with the special parameter to enable gravity
        auto projectile = std::make_unique<ProjectileEntity>(
            g_nextEntityId++,
            m_world,
            position.x,
            position.y,
            direction,
            m_textures,
            true, // fromPlayer = true
            true  // withGravity = true (we'll add this parameter)
        );

        g_currentSession->spawnEntity(std::move(projectile));
    }
    catch (const std::exception& e) {
        std::cerr << "[WeaponSystem] Error creating gravity projectile: " << e.what() << std::endl;
    }
}

float PlayerWeaponSystem::getCooldownForWeapon(WeaponType type) const {
    switch (type) {
    case WeaponType::Basic:  return 0.3f;
    case WeaponType::Rapid:  return 0.1f;
    case WeaponType::Spread: return 0.5f;
    case WeaponType::Laser:  return 1.0f;
    default:                 return 0.3f;
    }
}