#pragma once

#include "Exception.h"
#include <filesystem>

namespace GameExceptions {

//----------------------------------------------------------------------------------------
// Resource-related exceptions
//----------------------------------------------------------------------------------------

/**
 * @brief Base class for all resource-related exceptions
 */
class ResourceException : public Exception {
public:
    ResourceException(const std::string& message, 
                     const std::string& resourceName,
                     Severity severity = Severity::Error,
                     int errorCode = 100,
                     const std::source_location& location = std::source_location::current())
        : Exception(message, severity, errorCode, location), 
          m_resourceName(resourceName) {
        addContext(std::format("Resource: {}", resourceName));
    }
    
    const std::string& getResourceName() const { return m_resourceName; }
    
private:
    std::string m_resourceName;
};

/**
 * @brief Thrown when a resource file cannot be found
 */
class ResourceNotFoundException : public ResourceException {
public:
    ResourceNotFoundException(const std::string& resourceName,
                             const std::filesystem::path& searchPath = "",
                             const std::source_location& location = std::source_location::current())
        : ResourceException(std::format("Resource not found: {}", resourceName),
                          resourceName, Severity::Error, 101, location),
          m_searchPath(searchPath) {
        if (!searchPath.empty()) {
            addContext(std::format("Search path: {}", searchPath.string()));
        }
    }
    
    const std::filesystem::path& getSearchPath() const { return m_searchPath; }
    
private:
    std::filesystem::path m_searchPath;
};

/**
 * @brief Thrown when a resource file exists but cannot be loaded
 */
class ResourceLoadException : public ResourceException {
public:
    ResourceLoadException(const std::string& resourceName,
                         const std::string& reason,
                         const std::source_location& location = std::source_location::current())
        : ResourceException(std::format("Failed to load resource: {}", resourceName),
                          resourceName, Severity::Error, 102, location) {
        addContext(std::format("Reason: {}", reason));
    }
};

/**
 * @brief Thrown when a resource is of an invalid format
 */
class InvalidResourceFormatException : public ResourceException {
public:
    InvalidResourceFormatException(const std::string& resourceName,
                                  const std::string& expectedFormat,
                                  const std::source_location& location = std::source_location::current())
        : ResourceException(std::format("Invalid resource format: {}", resourceName),
                          resourceName, Severity::Error, 103, location),
          m_expectedFormat(expectedFormat) {
        addContext(std::format("Expected format: {}", expectedFormat));
    }
    
    const std::string& getExpectedFormat() const { return m_expectedFormat; }
    
private:
    std::string m_expectedFormat;
};

//----------------------------------------------------------------------------------------
// Physics-related exceptions
//----------------------------------------------------------------------------------------

/**
 * @brief Base class for all physics-related exceptions
 */
class PhysicsException : public Exception {
public:
    PhysicsException(const std::string& message, 
                    Severity severity = Severity::Error,
                    int errorCode = 200,
                    const std::source_location& location = std::source_location::current())
        : Exception(message, severity, errorCode, location) {
    }
};

/**
 * @brief Thrown when a physics body encounters an error
 */
class PhysicsBodyException : public PhysicsException {
public:
    PhysicsBodyException(const std::string& message,
                        const std::string& entityName,
                        const std::source_location& location = std::source_location::current())
        : PhysicsException(message, Severity::Error, 201, location),
          m_entityName(entityName) {
        addContext(std::format("Entity: {}", entityName));
    }
    
    const std::string& getEntityName() const { return m_entityName; }
    
private:
    std::string m_entityName;
};

//----------------------------------------------------------------------------------------
// Entity-related exceptions
//----------------------------------------------------------------------------------------

/**
 * @brief Base class for all entity-related exceptions
 */
class EntityException : public Exception {
public:
    EntityException(const std::string& message, 
                   const std::string& entityName,
                   Severity severity = Severity::Error,
                   int errorCode = 300,
                   const std::source_location& location = std::source_location::current())
        : Exception(message, severity, errorCode, location), 
          m_entityName(entityName) {
        addContext(std::format("Entity: {}", entityName));
    }
    
    const std::string& getEntityName() const { return m_entityName; }
    
private:
    std::string m_entityName;
};

/**
 * @brief Thrown when an entity cannot be created
 */
class EntityCreationException : public EntityException {
public:
    EntityCreationException(const std::string& entityName,
                           const std::string& reason,
                           const std::source_location& location = std::source_location::current())
        : EntityException(std::format("Failed to create entity: {}", entityName),
                        entityName, Severity::Error, 301, location) {
        addContext(std::format("Reason: {}", reason));
    }
};

//----------------------------------------------------------------------------------------
// Level-related exceptions
//----------------------------------------------------------------------------------------

/**
 * @brief Base class for all level-related exceptions
 */
class LevelException : public Exception {
public:
    LevelException(const std::string& message, 
                  const std::string& levelName,
                  Severity severity = Severity::Error,
                  int errorCode = 400,
                  const std::source_location& location = std::source_location::current())
        : Exception(message, severity, errorCode, location), 
          m_levelName(levelName) {
        addContext(std::format("Level: {}", levelName));
    }
    
    const std::string& getLevelName() const { return m_levelName; }
    
private:
    std::string m_levelName;
};

/**
 * @brief Thrown when a level file cannot be loaded
 */
class LevelLoadException : public LevelException {
public:
    LevelLoadException(const std::string& levelName,
                      const std::string& reason,
                      const std::source_location& location = std::source_location::current())
        : LevelException(std::format("Failed to load level: {}", levelName),
                       levelName, Severity::Error, 401, location) {
        addContext(std::format("Reason: {}", reason));
    }
};

/**
 * @brief Thrown when a level file has an invalid format
 */
class InvalidLevelFormatException : public LevelException {
public:
    InvalidLevelFormatException(const std::string& levelName,
                               const std::string& reason,
                               const std::source_location& location = std::source_location::current())
        : LevelException(std::format("Invalid level format: {}", levelName),
                       levelName, Severity::Error, 402, location) {
        addContext(std::format("Issue: {}", reason));
    }
};

//----------------------------------------------------------------------------------------
// Graphics-related exceptions
//----------------------------------------------------------------------------------------

/**
 * @brief Base class for all graphics-related exceptions
 */
class GraphicsException : public Exception {
public:
    GraphicsException(const std::string& message, 
                     Severity severity = Severity::Error,
                     int errorCode = 500,
                     const std::source_location& location = std::source_location::current())
        : Exception(message, severity, errorCode, location) {
    }
};

/**
 * @brief Thrown when a render operation fails
 */
class RenderException : public GraphicsException {
public:
    RenderException(const std::string& message,
                   const std::source_location& location = std::source_location::current())
        : GraphicsException(message, Severity::Error, 501, location) {
    }
};

} // namespace GameExceptions