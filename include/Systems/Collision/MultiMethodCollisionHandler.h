#pragma once

#include <unordered_map>
#include <functional>
#include <typeindex>
#include <memory>
#include "GameObject.h"

class MultiMethodCollisionHandler {
public:
    using CollisionHandler = std::function<void(GameObject&, GameObject&)>;
    using TypePair = std::pair<std::type_index, std::type_index>;

    // تعريف TypePairHash أولاً
    struct TypePairHash {
        std::size_t operator()(const TypePair& pair) const {
            auto h1 = std::hash<std::type_index>{}(pair.first);
            auto h2 = std::hash<std::type_index>{}(pair.second);
            return h1 ^ (h2 << 1);
        }
    };

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
    bool handleCollision(GameObject& obj1, GameObject& obj2) {
        TypePair key = makeTypePair(obj1, obj2);

        auto it = m_handlers.find(key);
        if (it != m_handlers.end()) {
            it->second(obj1, obj2);
            return true;
        }

        // جرب الترتيب العكسي
        TypePair reverseKey = { key.second, key.first };
        auto reverseIt = m_handlers.find(reverseKey);
        if (reverseIt != m_handlers.end()) {
            reverseIt->second(obj2, obj1);
            return true;
        }

        return false; // لا يوجد handler مُسجل
    }

private:
    std::unordered_map<TypePair, CollisionHandler, TypePairHash> m_handlers;

    template<typename T1, typename T2>
    TypePair makeTypePair() {
        return { std::type_index(typeid(T1)), std::type_index(typeid(T2)) };
    }

    TypePair makeTypePair(GameObject& obj1, GameObject& obj2) {
        return { std::type_index(typeid(obj1)), std::type_index(typeid(obj2)) };
    }
};