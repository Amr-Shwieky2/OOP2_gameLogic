#pragma once

#include <string>
#include <iostream>

/**
 * @brief Represents a semantic version for plugins
 * 
 * Follows semantic versioning format (MAJOR.MINOR.PATCH)
 * - MAJOR version increments indicate incompatible API changes
 * - MINOR version increments indicate backward-compatible functionality additions
 * - PATCH version increments indicate backward-compatible bug fixes
 */
class PluginVersion {
public:
    /**
     * @brief Construct a default version (0.0.0)
     */
    PluginVersion() : m_major(0), m_minor(0), m_patch(0) {}
    
    /**
     * @brief Construct a version from major, minor, patch components
     */
    PluginVersion(int major, int minor, int patch) 
        : m_major(major), m_minor(minor), m_patch(patch) {}
    
    /**
     * @brief Construct a version from a string in format "MAJOR.MINOR.PATCH"
     */
    explicit PluginVersion(const std::string& versionStr);
    
    // Getters
    int getMajor() const { return m_major; }
    int getMinor() const { return m_minor; }
    int getPatch() const { return m_patch; }
    
    /**
     * @brief Convert to string representation
     * @return Version string in format "MAJOR.MINOR.PATCH"
     */
    std::string toString() const;
    
    /**
     * @brief Check if this version is compatible with a target version
     * 
     * Compatibility rules:
     * - Same MAJOR version required
     * - This MINOR version must be >= target MINOR version
     * - If MINOR versions are equal, this PATCH must be >= target PATCH
     * 
     * @param target The target version to check compatibility with
     * @return True if compatible, false otherwise
     */
    bool isCompatibleWith(const PluginVersion& target) const;
    
    // Comparison operators
    bool operator==(const PluginVersion& other) const;
    bool operator!=(const PluginVersion& other) const;
    bool operator<(const PluginVersion& other) const;
    bool operator<=(const PluginVersion& other) const;
    bool operator>(const PluginVersion& other) const;
    bool operator>=(const PluginVersion& other) const;
    
    // Stream insertion operator for easy printing
    friend std::ostream& operator<<(std::ostream& os, const PluginVersion& version);
    
private:
    int m_major;
    int m_minor;
    int m_patch;
};