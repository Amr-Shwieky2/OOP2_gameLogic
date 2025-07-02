#pragma once
#include <vector>
#include <string>

class LevelManager {
public:
    LevelManager();
    void addLevel(const std::string& path);
    bool loadNextLevel();
    bool loadLevel(std::size_t index);
    bool reloadCurrentLevel();

    const std::string& getCurrentLevelPath() const;
    std::size_t getCurrentIndex() const;
    std::size_t getLevelCount() const;

    bool hasNextLevel() const;
    void resetToFirstLevel();

private:
    std::vector<std::string> m_levels;
    std::size_t m_currentIndex = 0;
};