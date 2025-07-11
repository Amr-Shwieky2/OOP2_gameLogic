#include "Exceptions/ResourceRetry.h"
#include "Exceptions/Logger.h"
#include <filesystem>
#include <algorithm>
#include <format>

namespace GameExceptions {

ResourceRetryManager& ResourceRetryManager::getInstance() {
    static ResourceRetryManager instance;
    return instance;
}

void ResourceRetryManager::registerAlternativePath(const std::string& primaryPath, const std::string& alternativePath) {
    m_alternativePaths[primaryPath] = alternativePath;
    getLogger().debug(std::format("Registered alternative path for {}: {}", primaryPath, alternativePath));
}

void ResourceRetryManager::registerAlternativeFormat(const std::string& primaryFormat, const std::string& alternativeFormat) {
    m_alternativeFormats[primaryFormat] = alternativeFormat;
    getLogger().debug(std::format("Registered alternative format {} -> {}", primaryFormat, alternativeFormat));
}

std::string ResourceRetryManager::findAlternativePath(const std::string& resourcePath) const {
    // First check direct mapping
    auto it = m_alternativePaths.find(resourcePath);
    if (it != m_alternativePaths.end()) {
        return it->second;
    }
    
    // Then check if we can create an alternative path
    try {
        std::filesystem::path path(resourcePath);
        std::string filename = path.filename().string();
        std::string extension = path.extension().string();
        
        // Try common alternative directories
        std::vector<std::string> altDirs = {
            "resources/", "assets/", "data/", "content/", "fallbacks/"
        };
        
        for (const auto& dir : altDirs) {
            std::string altPath = dir + filename;
            if (std::filesystem::exists(altPath)) {
                getLogger().info(std::format("Found alternative path: {}", altPath));
                return altPath;
            }
        }
        
        // Try without subdirectories if original path has them
        if (path.has_parent_path()) {
            std::string altPath = filename;
            if (std::filesystem::exists(altPath)) {
                getLogger().info(std::format("Found alternative path: {}", altPath));
                return altPath;
            }
        }
    }
    catch (const std::exception& ex) {
        getLogger().warning(std::format("Error finding alternative path: {}", ex.what()));
    }
    
    return ""; // No alternative found
}

std::string ResourceRetryManager::findAlternativeFormat(const std::string& resourcePath) const {
    try {
        std::filesystem::path path(resourcePath);
        std::string extension = path.extension().string();
        
        // Check for registered alternative formats
        auto it = m_alternativeFormats.find(extension);
        if (it != m_alternativeFormats.end()) {
            std::string altPath = replaceExtension(resourcePath, it->second);
            getLogger().info(std::format("Found alternative format: {}", altPath));
            return altPath;
        }
        
        // Try common format alternatives
        std::unordered_map<std::string, std::vector<std::string>> commonAlternatives = {
            {".png", {".jpg", ".bmp", ".tga"}},
            {".jpg", {".png", ".bmp", ".tga"}},
            {".wav", {".ogg", ".mp3"}},
            {".ogg", {".wav", ".mp3"}},
            {".ttf", {".otf"}}
        };
        
        auto altIt = commonAlternatives.find(extension);
        if (altIt != commonAlternatives.end()) {
            for (const auto& altExt : altIt->second) {
                std::string altPath = replaceExtension(resourcePath, altExt);
                if (std::filesystem::exists(altPath)) {
                    getLogger().info(std::format("Found alternative format: {}", altPath));
                    return altPath;
                }
            }
        }
    }
    catch (const std::exception& ex) {
        getLogger().warning(std::format("Error finding alternative format: {}", ex.what()));
    }
    
    return ""; // No alternative found
}

std::string ResourceRetryManager::replaceExtension(const std::string& path, const std::string& newExtension) const {
    try {
        std::filesystem::path fsPath(path);
        fsPath.replace_extension(newExtension);
        return fsPath.string();
    }
    catch (const std::exception& ex) {
        getLogger().warning(std::format("Error replacing extension: {}", ex.what()));
        return path;
    }
}

} // namespace GameExceptions