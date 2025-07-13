#include "RenderSystem.h"
#include "RenderComponent.h"
#include "Transform.h"
#include "EnemyEntity.h"
#include "SmartEnemyEntity.h"
#include <iostream>

void RenderSystem::render(EntityManager& entityManager, sf::RenderWindow& window) {
    static int frameCount = 0;
    frameCount++;

    // Debug output every 60 frames (approximately 1 second at 60 FPS)
    bool debugThisFrame = (frameCount % 60 == 0);
    if (debugThisFrame) {
        std::cout << "\n[RENDER DEBUG] Frame " << frameCount << " - Starting render pass" << std::endl;
    }

    int totalEntities = 0;
    int renderedEntities = 0;
    int enemiesFound = 0;
    int smartEnemiesFound = 0;

    for (Entity* entity : entityManager.getAllEntities()) {
        totalEntities++;
        if (!entity->isActive()) {
            continue;
        }

        auto* renderComp = entity->getComponent<RenderComponent>();
        auto* transform = entity->getComponent<Transform>();

        if (renderComp && transform) {
            // Update sprite position from transform
            sf::Vector2f pos = transform->getPosition();
            renderComp->getSprite().setPosition(pos);

            // Check if this is an enemy
            if (dynamic_cast<EnemyEntity*>(entity)) {
                enemiesFound++;
                if (debugThisFrame) {
                    std::cout << "[RENDER DEBUG] Enemy ID " << entity->getId()
                        << " at position (" << pos.x << ", " << pos.y << ")"
                        << " Scale: " << renderComp->getSprite().getScale().x
                        << " Color A: " << (int)renderComp->getSprite().getColor().a << std::endl;
                }
            }

            // Draw the sprite first
            window.draw(renderComp->getSprite());
            renderedEntities++;

            // Then draw eyes for smart enemies (on top of the sprite)
            if (auto* smartEnemy = dynamic_cast<SmartEnemyEntity*>(entity)) {
                smartEnemiesFound++;
                smartEnemy->drawEyes(window);

                if (debugThisFrame) {
                    std::cout << "[RENDER DEBUG] Drawing eyes for SmartEnemy ID " << entity->getId() << std::endl;
                }
            }
        }
    }

    if (debugThisFrame) {
        std::cout << "[RENDER DEBUG] Rendered " << renderedEntities << "/" << totalEntities
            << " entities, including " << enemiesFound << " enemies ("
            << smartEnemiesFound << " smart enemies with eyes)" << std::endl;
    }
}