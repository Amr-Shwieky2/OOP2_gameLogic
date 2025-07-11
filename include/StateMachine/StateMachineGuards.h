#pragma once
#include <functional>
#include <string>
#include <memory>
#include <unordered_map>
#include <typeindex>

// Forward declarations
class PlayerState;
class PlayerEntity;

/**
 * @brief Guard function type for state transitions
 * 
 * Guard functions return true if a transition should be allowed
 */
using TransitionGuard = std::function<bool(const PlayerEntity& player)>;

/**
 * @brief Action function type for state transitions
 * 
 * Actions are executed when a transition occurs
 */
using TransitionAction = std::function<void(PlayerEntity& player)>;

/**
 * @brief A condition that must be satisfied for a transition to occur
 */
class TransitionCondition {
public:
    TransitionCondition() = default;
    
    /**
     * @brief Construct a transition condition with a name
     * @param name Human-readable name for the condition
     */
    explicit TransitionCondition(const std::string& name) 
        : m_name(name), m_guard([](const PlayerEntity&){ return true; }) {}
    
    /**
     * @brief Construct a transition condition with name and guard
     * @param name Human-readable name for the condition
     * @param guard Guard function to evaluate
     */
    TransitionCondition(const std::string& name, TransitionGuard guard)
        : m_name(name), m_guard(guard) {}
    
    /**
     * @brief Evaluate the condition
     * @param player Player entity to check
     * @return true if the transition is allowed
     */
    bool evaluate(const PlayerEntity& player) const {
        return m_guard ? m_guard(player) : true;
    }
    
    /**
     * @brief Get the name of the condition
     * @return Condition name
     */
    const std::string& getName() const { return m_name; }
    
private:
    std::string m_name;
    TransitionGuard m_guard;
};

/**
 * @brief Manager for transition guards and actions
 * 
 * Stores and evaluates guards and actions for state transitions
 */
class TransitionManager {
public:
    /**
     * @brief Register a guard for a state transition
     * @param fromType Source state type
     * @param toType Destination state type
     * @param condition Condition that must be satisfied
     */
    static void RegisterGuard(
        const std::type_index& fromType, 
        const std::type_index& toType, 
        const TransitionCondition& condition) 
    {
        auto& manager = getInstance();
        auto key = std::make_pair(fromType, toType);
        manager.m_guards[key] = condition;
    }
    
    /**
     * @brief Register an action for a state transition
     * @param fromType Source state type
     * @param toType Destination state type
     * @param name Action name
     * @param action Action to execute
     */
    static void RegisterAction(
        const std::type_index& fromType, 
        const std::type_index& toType, 
        const std::string& name,
        const TransitionAction& action) 
    {
        auto& manager = getInstance();
        auto key = std::make_pair(fromType, toType);
        manager.m_actions[key] = std::make_pair(name, action);
    }
    
    /**
     * @brief Check if a transition is allowed
     * @param from Source state
     * @param to Destination state
     * @param player Player entity
     * @return true if the transition is allowed
     */
    static bool CanTransition(
        const PlayerState* from, 
        const PlayerState* to, 
        const PlayerEntity& player) 
    {
        if (!from || !to) return true;
        
        auto& manager = getInstance();
        auto key = std::make_pair(typeid(*from), typeid(*to));
        
        auto it = manager.m_guards.find(key);
        if (it != manager.m_guards.end()) {
            return it->second.evaluate(player);
        }
        
        return true; // No guard means transition is allowed
    }
    
    /**
     * @brief Execute actions for a transition
     * @param from Source state
     * @param to Destination state
     * @param player Player entity
     */
    static void ExecuteAction(
        const PlayerState* from, 
        const PlayerState* to, 
        PlayerEntity& player) 
    {
        if (!from || !to) return;
        
        auto& manager = getInstance();
        auto key = std::make_pair(typeid(*from), typeid(*to));
        
        auto it = manager.m_actions.find(key);
        if (it != manager.m_actions.end() && it->second.second) {
            it->second.second(player);
        }
    }
    
private:
    using TransitionKey = std::pair<std::type_index, std::type_index>;
    
    struct PairHash {
        std::size_t operator()(const TransitionKey& key) const {
            return std::hash<std::type_index>{}(key.first) ^ 
                   std::hash<std::type_index>{}(key.second);
        }
    };
    
    std::unordered_map<TransitionKey, TransitionCondition, PairHash> m_guards;
    std::unordered_map<TransitionKey, std::pair<std::string, TransitionAction>, PairHash> m_actions;
    
    static TransitionManager& getInstance() {
        static TransitionManager instance;
        return instance;
    }
};

/**
 * @brief Helper template to register guards for state transitions
 * @tparam From Source state type
 * @tparam To Destination state type
 * @param condition Condition that must be satisfied
 */
template<typename From, typename To>
void RegisterTransitionGuard(const TransitionCondition& condition) {
    TransitionManager::RegisterGuard(
        typeid(From), typeid(To), condition);
}

/**
 * @brief Helper template to register actions for state transitions
 * @tparam From Source state type
 * @tparam To Destination state type
 * @param name Action name
 * @param action Action to execute
 */
template<typename From, typename To>
void RegisterTransitionAction(const std::string& name, const TransitionAction& action) {
    TransitionManager::RegisterAction(
        typeid(From), typeid(To), name, action);
}