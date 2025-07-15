#include "RenderSystem.h"
#include "RenderComponent.h"
#include "Transform.h"
#include "EnemyEntity.h"
#include "SmartEnemyEntity.h"
#include <iostream>

//-------------------------------------------------------------------------------------
void RenderSystem::render(EntityManager& entityManager, sf::RenderWindow& window) {
    static int frameCount = 0;
    frameCount++;

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
            }

            // Draw the sprite first
            window.draw(renderComp->getSprite());
            renderedEntities++;

            // Then draw eyes for smart enemies (on top of the sprite)
            if (auto* smartEnemy = dynamic_cast<SmartEnemyEntity*>(entity)) {
                smartEnemiesFound++;
                smartEnemy->drawEyes(window);
            }
        }
    }
}
//-------------------------------------------------------------------------------------