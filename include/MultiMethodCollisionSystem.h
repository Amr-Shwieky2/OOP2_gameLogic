#pragma once

#include <unordered_map>
#include <functional>
#include <typeindex>
#include <memory>
#include <iostream>
#include "GameObject.h"
#include "Player.h"
#include "Coin.h"
#include "MovableBox.h"
#include "GroundTile.h"
#include "Flag.h"
#include "ICollectable.h"
#include "GameState.h"

class MultiMethodCollisionSystem {
public:
    using CollisionHandler = std::function<void(GameObject&, GameObject&)>;
    using TypePair = std::pair<std::type_index, std::type_index>;

    struct TypePairHash {
        std::size_t operator()(const TypePair& pair) const {
            auto h1 = std::hash<std::type_index>{}(pair.first);
            auto h2 = std::hash<std::type_index>{}(pair.second);
            return h1 ^ (h2 << 1);
        }
    };

private:
    std::unordered_map<TypePair, CollisionHandler, TypePairHash> m_handlers;
    GameState& m_gameState;
    Player& m_player;

public:
    MultiMethodCollisionSystem(Player& player, GameState& gameState);

    // تسجيل دالة للتعامل مع تصادم نوعين معينين
    template<typename T1, typename T2>
    void registerHandler(std::function<void(T1&, T2&)> handler) {
        TypePair key = makeTypePair<T1, T2>();

        m_handlers[key] = [handler](GameObject& obj1, GameObject& obj2) {
            T1& typed1 = static_cast<T1&>(obj1);
            T2& typed2 = static_cast<T2&>(obj2);
            handler(typed1, typed2);
            };
    }

    // معالجة التصادم بين كائنين
    bool handleCollision(GameObject& obj1, GameObject& obj2);

    // فحص جميع التصادمات
    void checkAllCollisions(std::vector<std::unique_ptr<GameObject>>& objects);

private:
    // تسجيل جميع الـ handlers
    void registerAllHandlers();

    // Collision Handlers
    void handlePlayerCoinCollision(Player& player, Coin& coin);
    void handlePlayerFlagCollision(Player& player, Flag& flag);
    void handlePlayerBoxCollision(Player& player, MovableBox& box);
    void handlePlayerGroundCollision(Player& player, GroundTile& ground);

    // Helper Methods
    template<typename T1, typename T2>
    TypePair makeTypePair() {
        return { std::type_index(typeid(T1)), std::type_index(typeid(T2)) };
    }

    TypePair makeTypePair(GameObject& obj1, GameObject& obj2);
    bool areColliding(const GameObject& obj1, const GameObject& obj2) const;
    sf::Vector2f calculatePushDirection(const GameObject& pusher, const GameObject& target) const;
};