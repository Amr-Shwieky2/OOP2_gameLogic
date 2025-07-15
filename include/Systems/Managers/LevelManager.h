#pragma once
#include <vector>
#include <string>

/**
 * @brief LevelManager - Handles management and switching of game levels.
 *
 * Stores a list of level file paths and allows switching between them
 * (next level, specific index, reload, etc). Useful for managing level
 * progression in a game.
 */
class LevelManager {
public:
    /**
     * @brief Constructor - Initializes the level manager.
     */
    LevelManager();

    /**
     * @brief Adds a level path to the list of available levels.
     * @param path The path to the level file.
     */
    void addLevel(const std::string& path);

    /**
     * @brief Loads the next level in the list, if any.
     * @return True if successful, false if already at the last level.
     */
    bool loadNextLevel();

    /**
     * @brief Loads the level at a specific index.
     * @param index The index of the level to load.
     * @return True if index is valid and level is loaded, false otherwise.
     */
    bool loadLevel(std::size_t index);

    /**
     * @brief Reloads the currently active level.
     * @return Always returns true (can be extended for actual reloading logic).
     */
    bool reloadCurrentLevel();

    /**
     * @brief Gets the path to the currently loaded level.
     * @return A reference to the current level file path.
     */
    const std::string& getCurrentLevelPath() const;

    /**
     * @brief Gets the index of the currently loaded level.
     * @return The index of the current level.
     */
    std::size_t getCurrentIndex() const;

    /**
     * @brief Gets the total number of levels stored.
     * @return Number of levels.
     */
    std::size_t getLevelCount() const;

    /**
     * @brief Checks if there is a next level available.
     * @return True if next level exists, false otherwise.
     */
    bool hasNextLevel() const;

    /**
     * @brief Resets the level index to the first level.
     */
    void resetToFirstLevel();

private:
    std::vector<std::string> m_levels;   ///< List of level file paths
    std::size_t m_currentIndex = 0;      ///< Index of the currently active level
};
