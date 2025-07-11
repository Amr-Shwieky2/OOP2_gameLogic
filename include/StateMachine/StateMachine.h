#pragma once
#include <type_traits>
#include <tuple>
#include <unordered_map>
#include <string>
#include <stdexcept>
#include <typeindex>
#include <iostream>

// Forward declarations
class PlayerState;

/**
 * @brief Type traits to check if a type is in a tuple
 */
template<typename T, typename Tuple>
struct IsTupleElement;

template<typename T>
struct IsTupleElement<T, std::tuple<>> : std::false_type {};

template<typename T, typename U, typename... Ts>
struct IsTupleElement<T, std::tuple<U, Ts...>> : IsTupleElement<T, std::tuple<Ts...>> {};

template<typename T, typename... Ts>
struct IsTupleElement<T, std::tuple<T, Ts...>> : std::true_type {};

/**
 * @brief Compile-time state transition validation
 * @tparam StateT The base state type
 * @tparam TransitionsT A tuple of state transition pairs
 */
template<typename StateT, typename TransitionsT>
class StateMachine {
public:
    /**
     * @brief Define a state transition
     * @tparam From Source state type
     * @tparam To Destination state type
     */
    template<typename From, typename To>
    struct Transition {
        using FromState = From;
        using ToState = To;
    };

    /**
     * @brief Check if a transition is valid at compile-time
     * @tparam From Source state type
     * @tparam To Destination state type
     * @return true if transition is valid, false otherwise
     */
    template<typename From, typename To>
    static constexpr bool IsValidTransition() {
        return IsTupleElement<Transition<From, To>, TransitionsT>::value;
    }

    /**
     * @brief Validate a transition at runtime
     * @param from Source state pointer
     * @param to Destination state pointer
     * @return true if transition is valid, false otherwise
     */
    static bool ValidateTransition(const StateT* from, const StateT* to) {
        if (!from || !to) return true; // Allow null transitions
        
        std::type_index fromType = typeid(*from);
        std::type_index toType = typeid(*to);
        
        auto& registry = GetTransitionRegistry();
        auto key = std::make_pair(fromType, toType);
        
        return registry.find(key) != registry.end();
    }

    /**
     * @brief Register all valid transitions in the registry
     * This should be called once at program initialization
     */
    static void RegisterTransitions() {
        // This will be specialized for each instantiation of the StateMachine
    }

private:
    // Runtime transition registry
    using TransitionPair = std::pair<std::type_index, std::type_index>;
    
    struct PairHash {
        std::size_t operator()(const TransitionPair& pair) const {
            return std::hash<std::type_index>{}(pair.first) ^ 
                   std::hash<std::type_index>{}(pair.second);
        }
    };

    static std::unordered_map<TransitionPair, bool, PairHash>& GetTransitionRegistry() {
        static std::unordered_map<TransitionPair, bool, PairHash> registry;
        return registry;
    }
};

// Helper to register a specific transition
template<typename From, typename To>
void RegisterTransition() {
    std::type_index fromType = typeid(From);
    std::type_index toType = typeid(To);
    
    auto key = std::make_pair(fromType, toType);
    auto& registry = StateMachine<PlayerState, void>::GetTransitionRegistry();
    
    registry[key] = true;
}