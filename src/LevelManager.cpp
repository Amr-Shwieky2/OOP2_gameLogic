#include "LevelManager.h"

LevelManager::LevelManager() {}

void LevelManager::addLevel(const std::string& path) {
    m_levels.push_back(path);
}

bool LevelManager::loadNextLevel() {
    if (m_currentIndex + 1 < m_levels.size()) {
        ++m_currentIndex;
        return true;
    }
    return false;  // No more levels
}

bool LevelManager::loadLevel(std::size_t index) {
    if (index < m_levels.size()) {
        m_currentIndex = index;
        return true;
    }
    return false;
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
