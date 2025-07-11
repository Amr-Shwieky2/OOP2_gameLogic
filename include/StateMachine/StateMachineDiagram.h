#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <typeinfo>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <tuple>

/**
 * @brief Helper to get readable type names
 * @tparam T Type to get name for
 * @return Demangled type name
 */
template<typename T>
std::string GetTypeName() {
    const char* name = typeid(T).name();
    // For simplicity, just remove common prefixes
    std::string result(name);
    
    // Remove "class " prefix if present
    const std::string classPrefix = "class ";
    if (result.substr(0, classPrefix.size()) == classPrefix) {
        result = result.substr(classPrefix.size());
    }
    
    return result;
}

/**
 * @brief Class for generating state machine diagrams in DOT format
 * @tparam StateMachineT The state machine type
 * 
 * This class can generate diagrams for visualization using Graphviz
 */
template<typename StateMachineT>
class StateMachineDiagramGenerator {
public:
    /**
     * @brief Generate a DOT file for the state machine
     * @param filename Output filename
     * @return true if successful
     */
    static bool GenerateDiagram(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open file for diagram: " << filename << std::endl;
            return false;
        }
        
        // Start DOT file
        file << "digraph StateMachine {" << std::endl;
        file << "  rankdir=LR;" << std::endl;
        file << "  node [shape=rectangle, style=rounded];" << std::endl;
        file << std::endl;
        
        // Process all transitions in the state machine
        GenerateTransitions(file);
        
        // End DOT file
        file << "}" << std::endl;
        
        std::cout << "State machine diagram generated: " << filename << std::endl;
        return true;
    }
    
private:
    // Extract transitions from the tuple and generate DOT notation
    template<typename TransitionsT, std::size_t... Indices>
    static void GenerateTransitionsHelper(
        std::ofstream& file,
        std::index_sequence<Indices...>) 
    {
        // Process each transition in the tuple
        (GenerateTransition<
            typename std::tuple_element<Indices, TransitionsT>::type
        >(file), ...);
    }
    
    // Generate diagram for a single transition
    template<typename TransitionT>
    static void GenerateTransition(std::ofstream& file) {
        using FromState = typename TransitionT::FromState;
        using ToState = typename TransitionT::ToState;
        
        std::string fromName = GetTypeName<FromState>();
        std::string toName = GetTypeName<ToState>();
        
        file << "  \"" << fromName << "\" -> \"" << toName << "\";" << std::endl;
    }
    
    // Generate all transitions in the state machine
    static void GenerateTransitions(std::ofstream& file) {
        using TransitionsT = typename StateMachineT::Transitions;
        constexpr std::size_t transitionCount = std::tuple_size_v<TransitionsT>;
        
        GenerateTransitionsHelper<TransitionsT>(
            file, std::make_index_sequence<transitionCount>{});
    }
};

/**
 * @brief Helper function to generate a state machine diagram
 * @tparam StateMachineT The state machine type
 * @param filename Output filename
 * @return true if successful
 */
template<typename StateMachineT>
bool GenerateStateMachineDiagram(const std::string& filename) {
    return StateMachineDiagramGenerator<StateMachineT>::GenerateDiagram(filename);
}