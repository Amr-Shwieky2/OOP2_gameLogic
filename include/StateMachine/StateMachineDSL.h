#pragma once
#include "StateMachine/StateMachine.h"
#include "StateMachine/StateMachineGuards.h"
#include <vector>
#include <tuple>
#include <string>
#include <functional>

/**
 * @brief Domain Specific Language for defining state machines
 * 
 * This file provides a fluent interface for defining state machines
 * and their transitions in a readable, declarative way.
 */

/**
 * @brief Transition with associated guard and action
 * @tparam From Source state
 * @tparam To Destination state
 */
template<typename From, typename To>
struct TransitionDefinition {
    using FromState = From;
    using ToState = To;
    
    std::string name;
    TransitionCondition guard;
    std::pair<std::string, TransitionAction> action;
    
    // Constructor with transition name only
    explicit TransitionDefinition(const std::string& transName)
        : name(transName),
          guard("Default", [](const PlayerEntity&) { return true; }),
          action(std::make_pair("", [](PlayerEntity&) {}))
    {}
    
    // Add a guard condition to the transition
    TransitionDefinition& When(const std::string& condName, TransitionGuard guardFn) {
        guard = TransitionCondition(condName, guardFn);
        return *this;
    }
    
    // Add an action to the transition
    TransitionDefinition& Do(const std::string& actionName, TransitionAction actionFn) {
        action = std::make_pair(actionName, actionFn);
        return *this;
    }
};

/**
 * @brief State machine builder for defining state machines in a fluent style
 * @tparam StateT The base state type
 */
template<typename StateT>
class StateMachineBuilder {
public:
    /**
     * @brief Start defining a new transition
     * @tparam From Source state type
     * @tparam To Destination state type
     * @param name Transition name
     * @return TransitionDefinition for further configuration
     */
    template<typename From, typename To>
    TransitionDefinition<From, To> DefineTransition(const std::string& name) {
        return TransitionDefinition<From, To>(name);
    }
    
    /**
     * @brief Add a transition to the state machine
     * @tparam From Source state type
     * @tparam To Destination state type
     * @param transition The transition definition
     */
    template<typename From, typename To>
    void AddTransition(const TransitionDefinition<From, To>& transition) {
        // Register the transition for compile-time checking
        m_transitions.push_back(
            [](void) {
                RegisterTransition<From, To>();
                
                // Register guard if defined
                if (transition.guard.getName() != "Default") {
                    RegisterTransitionGuard<From, To>(transition.guard);
                }
                
                // Register action if defined
                if (!transition.action.first.empty()) {
                    RegisterTransitionAction<From, To>(
                        transition.action.first, 
                        transition.action.second);
                }
            }
        );
    }
    
    /**
     * @brief Build the state machine and register all transitions
     */
    void Build() {
        for (const auto& registerFn : m_transitions) {
            registerFn();
        }
    }
    
private:
    std::vector<std::function<void(void)>> m_transitions;
};

// DSL macros for cleaner syntax
#define STATE_MACHINE(name) auto name = StateMachineBuilder<PlayerState>()
#define TRANSITION(from, to, name) AddTransition(DefineTransition<from, to>(name))
#define WHEN(condition, guard) When(condition, guard)
#define DO(action, fn) Do(action, fn)