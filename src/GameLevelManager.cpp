#include "GameLevelManager.h"
#include "EntityManager.h"
#include "PhysicsManager.h"
#include "ResourceManager.h"
#include <iostream>
#include <PlayerEntity.h>
#include "EntityFactory.h"

GameLevelManager::GameLevelManager() {
    // Add default levels
    m_levelManager.addLevel("level1.txt");
    m_levelManager.addLevel("level2.txt");

    std::cout << "[GameLevelManager] Created with " << m_levelManager.getLevelCount() << " levels" << std::endl;
}

void GameLevelManager::initialize(EntityManager& entityManager, PhysicsManager& physicsManager, TextureManager& textures) {
    m_entityManager = &entityManager;
    m_physicsManager = &physicsManager;
    m_textures = &textures;

    setupEventHandlers();
    std::cout << "[GameLevelManager] Initialized" << std::endl;
}

bool GameLevelManager::loadLevel(const std::string& levelPath) {
    if (!m_entityManager || !m_physicsManager || !m_textures) {
        std::cerr << "[GameLevelManager] Not initialized - cannot load level" << std::endl;
        return false;
    }

    std::cout << "[GameLevelManager] Loading level: " << levelPath << std::endl;

    try {
        // إيقاف التحديثات مؤقتاً لضمان عدم الوصول للكائنات أثناء التنظيف
        m_transitionPending = false;
        m_transitionTimer = 0.0f;
        m_needLevelSwitch = false;

        // إزالة أي مراجع للكائنات الحالية
        std::cout << "[GameLevelManager] Clearing current entities..." << std::endl;

        // تنظيف الكائنات الحالية - هذا يتعامل مع الذاكرة بأمان
        m_entityManager->clear();

        // انتظار قصير للتأكد من تنظيف الفيزياء
        std::cout << "[GameLevelManager] Entities cleared, loading new level..." << std::endl;

        // تحميل المستوى الجديد
        TextureManager& textureManager = *m_textures;
        bool success = m_levelLoader.loadFromFile(levelPath, *m_entityManager, m_physicsManager->getWorld(), textureManager);

        if (success) {
            // البحث عن اللاعب في المستوى الجديد
            bool playerFound = false;
            for (auto* entity : m_entityManager->getAllEntities()) {
                if (auto* player = dynamic_cast<PlayerEntity*>(entity)) {
                    playerFound = true;
                    std::cout << "[GameLevelManager] Player found in level" << std::endl;
                    break;
                }
            }

            // إنشاء لاعب افتراضي إذا لم يوجد
            if (!playerFound) {
                std::cout << "[GameLevelManager] No player in level, creating default player" << std::endl;
                try {
                    auto playerEntity = EntityFactory::instance().create("Player", 200.0f, 400.0f);
                    if (playerEntity) {
                        m_entityManager->addEntity(std::move(playerEntity));
                        std::cout << "[GameLevelManager] Default player created successfully" << std::endl;
                    }
                    else {
                        std::cerr << "[GameLevelManager] Failed to create default player" << std::endl;
                    }
                }
                catch (const std::exception& e) {
                    std::cerr << "[GameLevelManager] Error creating player: " << e.what() << std::endl;
                }
            }

            std::cout << "[GameLevelManager] Level loaded successfully: " << levelPath << std::endl;
        }
        else {
            std::cerr << "[GameLevelManager] Failed to load level: " << levelPath << std::endl;
        }

        return success;
    }
    catch (const std::exception& e) {
        std::cerr << "[GameLevelManager] Exception in loadLevel: " << e.what() << std::endl;
        return false;
    }
}

bool GameLevelManager::loadNextLevel() {
    if (m_levelManager.hasNextLevel()) {
        std::string currentLevel = m_levelManager.getCurrentLevelPath();

        if (m_levelManager.loadNextLevel()) {
            std::string nextLevel = m_levelManager.getCurrentLevelPath();

            std::cout << "[GameLevelManager] Queuing next level: " << nextLevel << std::endl;

            // Queue the level switch instead of doing it immediately
            m_nextLevelPath = nextLevel;
            m_needLevelSwitch = true;

            try {
                EventSystem::getInstance().publish(
                    LevelTransitionEvent(currentLevel, nextLevel, false)
                );
            }
            catch (const std::exception& e) {
                std::cerr << "[GameLevelManager] Error publishing LevelTransitionEvent: " << e.what() << std::endl;
            }

            return true;
        }
    }
    else {
        std::cout << "[GameLevelManager] No more levels - game complete!" << std::endl;
        try {
            EventSystem::getInstance().publish(
                LevelTransitionEvent(m_levelManager.getCurrentLevelPath(), "", true)
            );
        }
        catch (const std::exception& e) {
            std::cerr << "[GameLevelManager] Error publishing game complete event: " << e.what() << std::endl;
        }
    }

    return false;
}

bool GameLevelManager::reloadCurrentLevel() {
    std::string currentLevel = m_levelManager.getCurrentLevelPath();
    std::cout << "[GameLevelManager] Reloading current level: " << currentLevel << std::endl;
    return loadLevel(currentLevel);
}

const std::string& GameLevelManager::getCurrentLevelPath() const {
    return m_levelManager.getCurrentLevelPath();
}

std::size_t GameLevelManager::getCurrentLevelIndex() const {
    return m_levelManager.getCurrentIndex();
}

bool GameLevelManager::hasNextLevel() const {
    return m_levelManager.hasNextLevel();
}

std::size_t GameLevelManager::getLevelCount() const {
    return m_levelManager.getLevelCount();
}

void GameLevelManager::update(float deltaTime) {
    // Handle delayed level switching
    if (m_needLevelSwitch) {
        std::cout << "[GameLevelManager] Performing delayed level switch to: " << m_nextLevelPath << std::endl;
        m_needLevelSwitch = false;

        // تحميل المستوى الجديد بأمان
        try {
            loadLevel(m_nextLevelPath);
        }
        catch (const std::exception& e) {
            std::cerr << "[GameLevelManager] Error during level switch: " << e.what() << std::endl;
        }
        return;
    }

    // Handle level transition timing
    if (m_transitionPending) {
        m_transitionTimer += deltaTime;

        if (m_transitionTimer >= m_transitionDelay) {
            m_transitionPending = false;

            std::cout << "[GameLevelManager] Starting delayed level transition..." << std::endl;

            try {
                if (!loadNextLevel()) {
                    std::cout << "[GameLevelManager] All levels completed!" << std::endl;
                }
            }
            catch (const std::exception& e) {
                std::cerr << "[GameLevelManager] Error during level transition: " << e.what() << std::endl;
            }
        }
    }
}

void GameLevelManager::setupEventHandlers() {
    try {
        EventSystem::getInstance().subscribe<FlagReachedEvent>(
            [this](const FlagReachedEvent& event) {
                this->onFlagReached(event);
            }
        );

        EventSystem::getInstance().subscribe<LevelTransitionEvent>(
            [this](const LevelTransitionEvent& event) {
                this->onLevelTransition(event);
            }
        );

        // إضافة handler للبئر - هذا مهم جداً!
        EventSystem::getInstance().subscribe<WellEnteredEvent>(
            [this](const WellEnteredEvent& event) {
                this->onWellEntered(event);
            }
        );
    }
    catch (const std::exception& e) {
        std::cerr << "[GameLevelManager] Error setting up event handlers: " << e.what() << std::endl;
    }
}

void GameLevelManager::onFlagReached(const FlagReachedEvent& event) {
    std::cout << "[GameLevelManager] Flag reached! Starting transition timer..." << std::endl;
    m_transitionPending = true;
    m_transitionTimer = 0.0f;
}

void GameLevelManager::onLevelTransition(const LevelTransitionEvent& event) {
    if (event.isGameComplete) {
        std::cout << "[GameLevelManager] Game completed!" << std::endl;
    }
    else {
        std::cout << "[GameLevelManager] Transitioning from " << event.fromLevel
            << " to " << event.toLevel << std::endl;
    }
}

// إضافة handler جديد للبئر
void GameLevelManager::onWellEntered(const WellEnteredEvent& event) {
    std::cout << "[GameLevelManager] Well entered! Target level: " << event.targetLevel << std::endl;

    // تحميل المستوى المظلم مباشرة
    try {
        std::string targetLevel = event.targetLevel;
        if (targetLevel.empty()) {
            targetLevel = "dark_level.txt";
        }

        std::cout << "[GameLevelManager] Loading dark level: " << targetLevel << std::endl;

        // تحميل المستوى فوراً بدلاً من انتظار timer
        if (loadLevel(targetLevel)) {
            std::cout << "[GameLevelManager] Successfully loaded dark level!" << std::endl;
        }
        else {
            std::cerr << "[GameLevelManager] Failed to load dark level: " << targetLevel << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[GameLevelManager] Error loading dark level: " << e.what() << std::endl;
    }
}