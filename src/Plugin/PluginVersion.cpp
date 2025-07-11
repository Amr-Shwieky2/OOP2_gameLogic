#include "Plugin/PluginVersion.h"
#include <sstream>
#include <regex>
#include <stdexcept>

PluginVersion::PluginVersion(const std::string& versionStr) {
    std::regex versionRegex(R"((\d+)\.(\d+)\.(\d+))");
    std::smatch match;
    
    if (std::regex_match(versionStr, match, versionRegex)) {
        m_major = std::stoi(match[1].str());
        m_minor = std::stoi(match[2].str());
        m_patch = std::stoi(match[3].str());
    } else {
        throw std::invalid_argument("Invalid version format. Expected MAJOR.MINOR.PATCH");
    }
}

std::string PluginVersion::toString() const {
    std::stringstream ss;
    ss << m_major << "." << m_minor << "." << m_patch;
    return ss.str();
}

bool PluginVersion::isCompatibleWith(const PluginVersion& target) const {
    // Same major version required for compatibility
    if (m_major != target.m_major) {
        return false;
    }
    
    // This minor version must be >= target minor version
    if (m_minor < target.m_minor) {
        return false;
    }
    
    // If minor versions are equal, this patch must be >= target patch
    if (m_minor == target.m_minor && m_patch < target.m_patch) {
        return false;
    }
    
    return true;
}

bool PluginVersion::operator==(const PluginVersion& other) const {
    return m_major == other.m_major && 
           m_minor == other.m_minor && 
           m_patch == other.m_patch;
}

bool PluginVersion::operator!=(const PluginVersion& other) const {
    return !(*this == other);
}

bool PluginVersion::operator<(const PluginVersion& other) const {
    if (m_major != other.m_major) {
        return m_major < other.m_major;
    }
    if (m_minor != other.m_minor) {
        return m_minor < other.m_minor;
    }
    return m_patch < other.m_patch;
}

bool PluginVersion::operator<=(const PluginVersion& other) const {
    return *this < other || *this == other;
}

bool PluginVersion::operator>(const PluginVersion& other) const {
    return !(*this <= other);
}

bool PluginVersion::operator>=(const PluginVersion& other) const {
    return !(*this < other);
}

std::ostream& operator<<(std::ostream& os, const PluginVersion& version) {
    os << version.toString();
    return os;
}