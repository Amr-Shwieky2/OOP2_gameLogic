#pragma once

#include "MultiMethodCollisionSystem.h"
#include "ResourceManager.h"
#include "EntityManager.h"
#include "PlayerEntity.h"
#include "EnemyEntity.h"
#include "GiftEntity.h"
#include "CoinEntity.h"


/**
 * Sets up all collision handlers for the game
 */
void setupGameCollisionHandlers(MultiMethodCollisionSystem& collisionSystem);
void registerGameEntities(b2World& world, TextureManager& textures, EntityManager& entityManager);