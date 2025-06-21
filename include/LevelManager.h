#pragma once

#include <string>
#include <vector>

class LevelManager {
public:
    LevelManager();

    void addLevel(const std::string& path);
    bool loadNextLevel();
    bool loadLevel(std::size_t index);
    const std::string& getCurrentLevelPath() const;
    std::size_t getCurrentIndex() const;
    std::size_t getLevelCount() const;

private:
    std::vector<std::string> m_levels;
    std::size_t m_currentIndex = 0; // level 1 
};
