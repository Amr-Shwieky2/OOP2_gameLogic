#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <map>
#include <typeinfo>
#include <typeindex>
#include <any>
#include <optional>

namespace Scripting {

// Forward declaration
class IScriptEngine;

/**
 * @brief Method binding information
 */
struct MethodBinding {
    std::string name;                                   ///< Name of the method in scripts
    std::function<std::any(void*, const std::vector<std::any>&)> function; ///< Function pointer wrapper
    std::vector<std::type_index> paramTypes;            ///< Parameter types
    std::type_index returnType;                         ///< Return type
    std::string documentation;                          ///< Documentation string
};

/**
 * @brief Property binding information
 */
struct PropertyBinding {
    std::string name;                                   ///< Name of the property in scripts
    std::function<std::any(void*)> getter;              ///< Getter function
    std::function<void(void*, const std::any&)> setter; ///< Setter function
    std::type_index type;                               ///< Property type
    bool readOnly = false;                              ///< Whether property is read-only
    std::string documentation;                          ///< Documentation string
};

/**
 * @brief Class binding information
 */
struct ClassBinding {
    std::string name;                                   ///< Name of the class in scripts
    std::type_index typeIndex;                          ///< C++ type information
    std::optional<std::type_index> baseType;            ///< Base class type (if any)
    std::vector<MethodBinding> methods;                 ///< Bound methods
    std::vector<PropertyBinding> properties;            ///< Bound properties
    std::function<void*(const std::vector<std::any>&)> constructor; ///< Constructor function
    std::function<void(void*)> destructor;              ///< Destructor function
    std::vector<std::type_index> constructorParamTypes; ///< Constructor parameter types
    std::string documentation;                          ///< Documentation string
};

/**
 * @brief Script binding generator for C++ classes
 * 
 * This class generates bindings between C++ classes and the scripting system,
 * allowing script code to instantiate, call methods on, and access properties
 * of C++ objects.
 */
class ScriptBindingGenerator {
public:
    /**
     * @brief Get the singleton instance
     * @return Reference to the binding generator
     */
    static ScriptBindingGenerator& getInstance();

    /**
     * @brief Initialize the binding generator
     * @param scriptEngine Script engine to bind to
     * @return True if initialization succeeds
     */
    bool initialize(IScriptEngine* scriptEngine);

    /**
     * @brief Begin defining a class binding
     * @tparam T The C++ class type
     * @param className Name of the class in scripts
     * @param documentation Optional documentation string
     * @return Reference to this binding generator for method chaining
     */
    template<typename T>
    ScriptBindingGenerator& beginClass(const std::string& className, 
                                      const std::string& documentation = "") {
        m_currentBinding = std::make_shared<ClassBinding>();
        m_currentBinding->name = className;
        m_currentBinding->typeIndex = std::type_index(typeid(T));
        m_currentBinding->documentation = documentation;
        
        // Create default constructor if possible
        if constexpr (std::is_default_constructible_v<T>) {
            m_currentBinding->constructor = [](const std::vector<std::any>& args) -> void* {
                return new T();
            };
        }
        
        // Create destructor
        m_currentBinding->destructor = [](void* ptr) {
            delete static_cast<T*>(ptr);
        };
        
        return *this;
    }

    /**
     * @brief Define a base class for the current class binding
     * @tparam Base The base class type
     * @return Reference to this binding generator for method chaining
     */
    template<typename Base>
    ScriptBindingGenerator& inherits() {
        if (m_currentBinding) {
            m_currentBinding->baseType = std::type_index(typeid(Base));
        }
        return *this;
    }

    /**
     * @brief Define a constructor for the current class binding
     * @tparam T The class type
     * @tparam Args Constructor argument types
     * @return Reference to this binding generator for method chaining
     */
    template<typename T, typename... Args>
    ScriptBindingGenerator& addConstructor() {
        if (m_currentBinding) {
            m_currentBinding->constructor = [](const std::vector<std::any>& args) -> void* {
                if (args.size() != sizeof...(Args)) {
                    throw std::runtime_error("Wrong number of constructor arguments");
                }
                
                return createObject<T, Args...>(args, std::index_sequence_for<Args...>());
            };
            
            m_currentBinding->constructorParamTypes = { std::type_index(typeid(Args))... };
        }
        return *this;
    }

    /**
     * @brief Add a method to the current class binding
     * @tparam T The class type
     * @tparam Return Return type
     * @tparam Args Method argument types
     * @param name Method name in scripts
     * @param method Pointer to member function
     * @param documentation Optional documentation string
     * @return Reference to this binding generator for method chaining
     */
    template<typename T, typename Return, typename... Args>
    ScriptBindingGenerator& addMethod(const std::string& name, 
                                     Return (T::*method)(Args...), 
                                     const std::string& documentation = "") {
        if (m_currentBinding) {
            MethodBinding binding;
            binding.name = name;
            binding.function = [method](void* obj, const std::vector<std::any>& args) -> std::any {
                if (args.size() != sizeof...(Args)) {
                    throw std::runtime_error("Wrong number of method arguments");
                }
                
                T* instance = static_cast<T*>(obj);
                return invokeMethod(instance, method, args, std::index_sequence_for<Args...>());
            };
            
            binding.paramTypes = { std::type_index(typeid(Args))... };
            binding.returnType = std::type_index(typeid(Return));
            binding.documentation = documentation;
            
            m_currentBinding->methods.push_back(binding);
        }
        return *this;
    }

    /**
     * @brief Add a const method to the current class binding
     * @tparam T The class type
     * @tparam Return Return type
     * @tparam Args Method argument types
     * @param name Method name in scripts
     * @param method Pointer to const member function
     * @param documentation Optional documentation string
     * @return Reference to this binding generator for method chaining
     */
    template<typename T, typename Return, typename... Args>
    ScriptBindingGenerator& addMethod(const std::string& name, 
                                     Return (T::*method)(Args...) const, 
                                     const std::string& documentation = "") {
        if (m_currentBinding) {
            MethodBinding binding;
            binding.name = name;
            binding.function = [method](void* obj, const std::vector<std::any>& args) -> std::any {
                if (args.size() != sizeof...(Args)) {
                    throw std::runtime_error("Wrong number of method arguments");
                }
                
                const T* instance = static_cast<const T*>(obj);
                return invokeConstMethod(instance, method, args, std::index_sequence_for<Args...>());
            };
            
            binding.paramTypes = { std::type_index(typeid(Args))... };
            binding.returnType = std::type_index(typeid(Return));
            binding.documentation = documentation;
            
            m_currentBinding->methods.push_back(binding);
        }
        return *this;
    }

    /**
     * @brief Add a property to the current class binding
     * @tparam T The class type
     * @tparam PropType Property type
     * @param name Property name in scripts
     * @param getter Getter member function
     * @param setter Setter member function (nullptr for read-only)
     * @param documentation Optional documentation string
     * @return Reference to this binding generator for method chaining
     */
    template<typename T, typename PropType>
    ScriptBindingGenerator& addProperty(const std::string& name, 
                                       PropType (T::*getter)() const, 
                                       void (T::*setter)(PropType) = nullptr,
                                       const std::string& documentation = "") {
        if (m_currentBinding) {
            PropertyBinding binding;
            binding.name = name;
            binding.getter = [getter](void* obj) -> std::any {
                const T* instance = static_cast<const T*>(obj);
                return (instance->*getter)();
            };
            
            if (setter) {
                binding.setter = [setter](void* obj, const std::any& value) {
                    T* instance = static_cast<T*>(obj);
                    (instance->*setter)(std::any_cast<PropType>(value));
                };
                binding.readOnly = false;
            } else {
                binding.readOnly = true;
            }
            
            binding.type = std::type_index(typeid(PropType));
            binding.documentation = documentation;
            
            m_currentBinding->properties.push_back(binding);
        }
        return *this;
    }

    /**
     * @brief Add a direct member variable property to the current class binding
     * @tparam T The class type
     * @tparam PropType Property type
     * @param name Property name in scripts
     * @param member Pointer to member variable
     * @param readOnly Whether the property is read-only
     * @param documentation Optional documentation string
     * @return Reference to this binding generator for method chaining
     */
    template<typename T, typename PropType>
    ScriptBindingGenerator& addMemberProperty(const std::string& name, 
                                            PropType T::* member,
                                            bool readOnly = false,
                                            const std::string& documentation = "") {
        if (m_currentBinding) {
            PropertyBinding binding;
            binding.name = name;
            binding.getter = [member](void* obj) -> std::any {
                T* instance = static_cast<T*>(obj);
                return instance->*member;
            };
            
            if (!readOnly) {
                binding.setter = [member](void* obj, const std::any& value) {
                    T* instance = static_cast<T*>(obj);
                    instance->*member = std::any_cast<PropType>(value);
                };
            }
            
            binding.type = std::type_index(typeid(PropType));
            binding.readOnly = readOnly;
            binding.documentation = documentation;
            
            m_currentBinding->properties.push_back(binding);
        }
        return *this;
    }

    /**
     * @brief End the current class binding and register it
     * @return Reference to this binding generator for method chaining
     */
    ScriptBindingGenerator& endClass() {
        if (m_currentBinding) {
            m_bindings[m_currentBinding->name] = m_currentBinding;
            registerClassWithScriptEngine(m_currentBinding);
            m_currentBinding.reset();
        }
        return *this;
    }

    /**
     * @brief Generate documentation for all bindings
     * @return Documentation string
     */
    std::string generateDocumentation() const;

    /**
     * @brief Get a binding by class name
     * @param className Name of the class
     * @return Pointer to the binding or nullptr if not found
     */
    std::shared_ptr<ClassBinding> getBinding(const std::string& className) const;

    /**
     * @brief Get a binding by C++ type
     * @tparam T The C++ type
     * @return Pointer to the binding or nullptr if not found
     */
    template<typename T>
    std::shared_ptr<ClassBinding> getBindingForType() const {
        std::type_index typeIdx(typeid(T));
        for (const auto& [name, binding] : m_bindings) {
            if (binding->typeIndex == typeIdx) {
                return binding;
            }
        }
        return nullptr;
    }

private:
    // Private constructor for singleton
    ScriptBindingGenerator() = default;
    ~ScriptBindingGenerator() = default;
    
    // Prevent copying or moving
    ScriptBindingGenerator(const ScriptBindingGenerator&) = delete;
    ScriptBindingGenerator& operator=(const ScriptBindingGenerator&) = delete;
    ScriptBindingGenerator(ScriptBindingGenerator&&) = delete;
    ScriptBindingGenerator& operator=(ScriptBindingGenerator&&) = delete;

    /**
     * @brief Helper to create object instances
     * @tparam T Class type
     * @tparam Args Constructor argument types
     * @tparam Indices Index sequence for argument unpacking
     * @param args Vector of arguments
     * @param indices Index sequence
     * @return Pointer to created object
     */
    template<typename T, typename... Args, std::size_t... Indices>
    static void* createObject(const std::vector<std::any>& args, std::index_sequence<Indices...>) {
        return new T(std::any_cast<Args>(args[Indices])...);
    }

    /**
     * @brief Helper to invoke methods
     * @tparam T Class type
     * @tparam Return Return type
     * @tparam Args Method argument types
     * @tparam Indices Index sequence for argument unpacking
     * @param obj Object instance
     * @param method Method pointer
     * @param args Vector of arguments
     * @param indices Index sequence
     * @return Method return value
     */
    template<typename T, typename Return, typename... Args, std::size_t... Indices>
    static std::any invokeMethod(T* obj, Return (T::*method)(Args...), 
                                const std::vector<std::any>& args, 
                                std::index_sequence<Indices...>) {
        if constexpr (std::is_void_v<Return>) {
            (obj->*method)(std::any_cast<Args>(args[Indices])...);
            return {};
        } else {
            return (obj->*method)(std::any_cast<Args>(args[Indices])...);
        }
    }

    /**
     * @brief Helper to invoke const methods
     * @tparam T Class type
     * @tparam Return Return type
     * @tparam Args Method argument types
     * @tparam Indices Index sequence for argument unpacking
     * @param obj Object instance
     * @param method Method pointer
     * @param args Vector of arguments
     * @param indices Index sequence
     * @return Method return value
     */
    template<typename T, typename Return, typename... Args, std::size_t... Indices>
    static std::any invokeConstMethod(const T* obj, Return (T::*method)(Args...) const, 
                                     const std::vector<std::any>& args, 
                                     std::index_sequence<Indices...>) {
        if constexpr (std::is_void_v<Return>) {
            (obj->*method)(std::any_cast<Args>(args[Indices])...);
            return {};
        } else {
            return (obj->*method)(std::any_cast<Args>(args[Indices])...);
        }
    }

    /**
     * @brief Register a class binding with the script engine
     * @param binding Class binding to register
     * @return True if registration succeeds
     */
    bool registerClassWithScriptEngine(const std::shared_ptr<ClassBinding>& binding);

    IScriptEngine* m_scriptEngine = nullptr;
    std::shared_ptr<ClassBinding> m_currentBinding;
    std::map<std::string, std::shared_ptr<ClassBinding>> m_bindings;
};

/**
 * @brief Macro to simplify class binding definition
 */
#define SCRIPT_BEGIN_CLASS(Type, Name) \
    Scripting::ScriptBindingGenerator::getInstance().beginClass<Type>(Name, #Type " class")

/**
 * @brief Macro to simplify constructor binding
 */
#define SCRIPT_ADD_CONSTRUCTOR(...) \
    .addConstructor<__VA_ARGS__>()

/**
 * @brief Macro to simplify method binding
 */
#define SCRIPT_ADD_METHOD(Type, Name, Method) \
    .addMethod<Type>(Name, &Type::Method, #Method)

/**
 * @brief Macro to simplify property binding
 */
#define SCRIPT_ADD_PROPERTY(Type, Name, Getter, Setter) \
    .addProperty<Type>(Name, &Type::Getter, &Type::Setter, #Name " property")

/**
 * @brief Macro to simplify read-only property binding
 */
#define SCRIPT_ADD_READONLY_PROPERTY(Type, Name, Getter) \
    .addProperty<Type>(Name, &Type::Getter, nullptr, #Name " property (read-only)")

/**
 * @brief Macro to simplify member variable binding
 */
#define SCRIPT_ADD_MEMBER(Type, Name, Member) \
    .addMemberProperty<Type>(Name, &Type::Member, false, #Member " member variable")

/**
 * @brief Macro to simplify read-only member variable binding
 */
#define SCRIPT_ADD_READONLY_MEMBER(Type, Name, Member) \
    .addMemberProperty<Type>(Name, &Type::Member, true, #Member " member variable (read-only)")

/**
 * @brief Macro to end class binding definition
 */
#define SCRIPT_END_CLASS() \
    .endClass()

} // namespace Scripting