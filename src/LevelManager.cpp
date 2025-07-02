#include "LevelManager.h"
#include <iostream>

LevelManager::LevelManager() {
    addLevel("level1.txt");
    addLevel("level2.txt");
}

void LevelManager::addLevel(const std::string& path) {
    m_levels.push_back(path);
    std::cout << "[LevelManager] Added level: " << path << std::endl;
}

bool LevelManager::loadNextLevel() {
    if (m_currentIndex + 1 < m_levels.size()) {
        ++m_currentIndex;
        std::cout << "[LevelManager] Loading next level: " << getCurrentLevelPath()
            << " (" << (m_currentIndex + 1) << "/" << m_levels.size() << ")" << std::endl;
        return true;
    }

    std::cout << "[LevelManager] No more levels! Game completed!" << std::endl;
    return false;  
}

bool LevelManager::loadLevel(std::size_t index) {
    if (index < m_levels.size()) {
        m_currentIndex = index;
        std::cout << "[LevelManager] Loading level " << (index + 1) << ": "
            << getCurrentLevelPath() << std::endl;
        return true;
    }

    std::cout << "[LevelManager] Invalid level index: " << index << std::endl;
    return false;
}

bool LevelManager::reloadCurrentLevel() {
    std::cout << "[LevelManager] Reloading current level: " << getCurrentLevelPath() << std::endl;
    return true;
}

const std::string& LevelManager::getCurrentLevelPath() const {
    return m_levels.at(m_currentIndex);
}

std::size_t LevelManager::getCurrentIndex() const {
    return m_currentIndex;
}

std::size_t LevelManager::getLevelCount() const {
    return m_levels.size();
}

bool LevelManager::hasNextLevel() const {
    return m_currentIndex + 1 < m_levels.size();
}

void LevelManager::resetToFirstLevel() {
    m_currentIndex = 0;
    std::cout << "[LevelManager] Reset to first level: " << getCurrentLevelPath() << std::endl;
}