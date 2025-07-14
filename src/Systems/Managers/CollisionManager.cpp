#include "CollisionManager.h"
#include "EntityManager.h"
#include "Entity.h"
#include "Transform.h"
#include "GameCollisionSetup.h"
#include "WellEntity.h"
#include "SeaEntity.h"
#include "PlayerEntity.h"
#include <cmath>
#include <iostream>
#include <algorithm>

CollisionManager::CollisionManager() {
    std::cout << "[CollisionManager] Created" << std::endl;
}

void CollisionManager::setupGameCollisionHandlers() {
    ::setupGameCollisionHandlers(m_collisionSystem);
    std::cout << "[CollisionManager] Game collision handlers setup complete" << std::endl;
}

void CollisionManager::checkCollisions(EntityManager& entityManager) {
    m_collisionChecks = 0;
    m_collisionsProcessed = 0;

    auto entities = entityManager.getAllEntities();

    // Debug: Count wells and players
    int wellCount = 0;
    int playerCount = 0;
    for (auto* entity : entities) {
        if (dynamic_cast<WellEntity*>(entity)) wellCount++;
        if (dynamic_cast<PlayerEntity*>(entity)) playerCount++;
    }

    static int frameCount = 0;
    frameCount++;

    // Debug every 120 frames (2 seconds at 60 FPS)
    if (frameCount % 120 == 0) {
        std::cout << "[CollisionManager] Frame " << frameCount
            << " - Entities: " << entities.size()
            << " (Wells: " << wellCount
            << ", Players: " << playerCount << ")" << std::endl;
    }

    for (size_t i = 0; i < entities.size(); ++i) {
        for (size_t j = i + 1; j < entities.size(); ++j) {
            if (entities[i]->isActive() && entities[j]->isActive()) {
                m_collisionChecks++;

                // Special debug for well collisions
                WellEntity* well = dynamic_cast<WellEntity*>(entities[i]);
                if (!well) well = dynamic_cast<WellEntity*>(entities[j]);

                PlayerEntity* player = dynamic_cast<PlayerEntity*>(entities[i]);
                if (!player) player = dynamic_cast<PlayerEntity*>(entities[j]);

                if (well && player) {
                    bool colliding = areColliding(*entities[i], *entities[j]);

                    // Debug well-player collision checks
                    static int wellCheckCount = 0;
                    wellCheckCount++;
                    if (wellCheckCount % 60 == 0) { // Every second
                        auto* wellTransform = well->getComponent<Transform>();
                        auto* playerTransform = player->getComponent<Transform>();

                        if (wellTransform && playerTransform) {
                            sf::Vector2f wellPos = wellTransform->getPosition();
                            sf::Vector2f playerPos = playerTransform->getPosition();
                            float distance = std::sqrt((wellPos.x - playerPos.x) * (wellPos.x - playerPos.x) +
                                (wellPos.y - playerPos.y) * (wellPos.y - playerPos.y));

                            std::cout << "[DEBUG WELL] Check " << wellCheckCount
                                << " - Well pos: (" << wellPos.x << ", " << wellPos.y
                                << "), Player pos: (" << playerPos.x << ", " << playerPos.y
                                << "), Distance: " << distance
                                << ", Colliding: " << (colliding ? "YES" : "NO")
                                << ", Well activated: " << well->isActivated() << std::endl;
                        }
                    }

                    if (colliding) {
                        std::cout << "[DEBUG WELL] COLLISION DETECTED! Processing..." << std::endl;
                        if (m_collisionSystem.processCollision(*entities[i], *entities[j])) {
                            m_collisionsProcessed++;
                            std::cout << "[DEBUG WELL] Collision processed successfully!" << std::endl;
                        }
                        else {
                            std::cout << "[DEBUG WELL] WARNING: Collision NOT processed!" << std::endl;
                        }
                    }
                }
                else {
                    // Normal collision check for non-well entities
                    if (areColliding(*entities[i], *entities[j])) {
                        if (m_collisionSystem.processCollision(*entities[i], *entities[j])) {
                            m_collisionsProcessed++;
                        }
                    }
                }
            }
        }
    }
}

bool CollisionManager::areColliding(Entity& a, Entity& b) const {
    auto* transA = a.getComponent<Transform>();
    auto* transB = b.getComponent<Transform>();

    if (!transA || !transB) return false;

    sf::Vector2f posA = transA->getPosition();
    sf::Vector2f posB = transB->getPosition();

    float dx = posA.x - posB.x;
    float dy = posA.y - posB.y;
    float distSq = dx * dx + dy * dy;

    // Base collision radius
    float collisionDistance = 100.0f;

    // Special handling for wells - larger collision radius
    WellEntity* well = dynamic_cast<WellEntity*>(&a);
    if (!well) well = dynamic_cast<WellEntity*>(&b);

    if (well) {
        collisionDistance = 150.0f; // Much larger radius for wells
    }

    // Hazards like the sea cover a full tile which is larger
    if (dynamic_cast<SeaEntity*>(&a) || dynamic_cast<SeaEntity*>(&b)) {
        collisionDistance = std::max(collisionDistance, 150.0f);
    }

    return distSq < (collisionDistance * collisionDistance);
}

void CollisionManager::clearHandlers() {
    m_collisionSystem.clear();
}

void CollisionManager::resetStats() {
    m_collisionChecks = 0;
    m_collisionsProcessed = 0;
}