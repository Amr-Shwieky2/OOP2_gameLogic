#include "RenderSystem.h"
#include "RenderComponent.h"
#include "Transform.h"

void RenderSystem::render(EntityManager& entityManager, sf::RenderWindow& window) {
    for (Entity* entity : entityManager.getAllEntities()) {
        if (!entity->isActive())
            continue;
        auto* renderComp = entity->getComponent<RenderComponent>();
        auto* transform = entity->getComponent<Transform>();
        if (renderComp && transform) {
            renderComp->getSprite().setPosition(transform->getPosition());
            window.draw(renderComp->getSprite());
        }
    }
}
