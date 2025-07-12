#include "GameLevelManager.h"
#include "EntityManager.h"
#include "PhysicsManager.h"
#include "ResourceManager.h"
#include "GameSession.h"
#include <iostream>
#include <PlayerEntity.h>
#include "EntityFactory.h"

GameLevelManager::GameLevelManager() {
    // Add default levels
    m_levelManager.addLevel("level1.txt");
    m_levelManager.addLevel("level2.txt");
}

void GameLevelManager::initialize(EntityManager& entityManager, PhysicsManager& physicsManager, TextureManager& textures) {
    m_entityManager = &entityManager;
    m_physicsManager = &physicsManager;
    m_textures = &textures;

    setupEventHandlers();
}

bool GameLevelManager::loadLevel(const std::string& levelPath) {
    if (!m_entityManager || !m_physicsManager || !m_textures) {
        return false;
    }

    try {
        // إيقاف التحديثات مؤقتاً لضمان عدم الوصول للكائنات أثناء التنظيف
        m_transitionPending = false;
        m_transitionTimer = 0.0f;
        m_needLevelSwitch = false;

        // Invalidate cached player in current session since entities will be cleared
        if (g_currentSession) {
            g_currentSession->invalidateCachedPlayer();
        }

        // إزالة أي مراجع للكائنات الحالية
        
        // تنظيف الكائنات الحالية - هذا يتعامل مع الذاكرة بأمان
        m_entityManager->clear();

        // تحميل المستوى الجديد
        TextureManager& textureManager = *m_textures;
        bool success = m_levelLoader.loadFromFile(levelPath, *m_entityManager, m_physicsManager->getWorld(), textureManager);

        if (success) {
            // البحث عن اللاعب في المستوى الجديد
            bool playerFound = false;
            for (auto* entity : m_entityManager->getAllEntities()) {
                if (auto* player = dynamic_cast<PlayerEntity*>(entity)) {
                    playerFound = true;
                    break;
                }
            }

            // إنشاء لاعب افتراضي إذا لم يوجد
            if (!playerFound) {
                try {
                    auto playerEntity = EntityFactory::instance().create("Player", 200.0f, 400.0f);
                    if (playerEntity) {
                        m_entityManager->addEntity(std::move(playerEntity));
                    }
                }
                catch (const std::exception& e) {
                }
            }
        }

        return success;
    }
    catch (const std::exception& e) {
        return false;
    }
}

bool GameLevelManager::loadNextLevel() {
    if (m_levelManager.hasNextLevel()) {
        std::string currentLevel = m_levelManager.getCurrentLevelPath();

        if (m_levelManager.loadNextLevel()) {
            std::string nextLevel = m_levelManager.getCurrentLevelPath();

            // Queue the level switch instead of doing it immediately
            m_nextLevelPath = nextLevel;
            m_needLevelSwitch = true;

            try {
                EventSystem::getInstance().publish(
                    LevelTransitionEvent(currentLevel, nextLevel, false)
                );
            }
            catch (const std::exception& e) {
            }

            return true;
        }
    }
    else {
        try {
            EventSystem::getInstance().publish(
                LevelTransitionEvent(m_levelManager.getCurrentLevelPath(), "", true)
            );
        }
        catch (const std::exception& e) {
        }
    }

    return false;
}

bool GameLevelManager::reloadCurrentLevel() {
    std::string currentLevel = m_levelManager.getCurrentLevelPath();
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
        m_needLevelSwitch = false;

        // تحميل المستوى الجديد بأمان
        try {
            loadLevel(m_nextLevelPath);
        }
        catch (const std::exception& e) {
        }
        return;
    }

    // Handle level transition timing
    if (m_transitionPending) {
        m_transitionTimer += deltaTime;

        if (m_transitionTimer >= m_transitionDelay) {
            m_transitionPending = false;

            try {
                if (!loadNextLevel()) {
                }
            }
            catch (const std::exception& e) {
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
    }
}

void GameLevelManager::onFlagReached(const FlagReachedEvent&) {
    m_transitionPending = true;
    m_transitionTimer = 0.0f;
}

void GameLevelManager::onLevelTransition(const LevelTransitionEvent& event) {
    if (event.isGameComplete) {
    }
    else {
    }
}

// إضافة handler جديد للبئر
void GameLevelManager::onWellEntered(const WellEnteredEvent& event) {
    // تحميل المستوى المظلم مباشرة
    try {
        std::string targetLevel = event.targetLevel;
        if (targetLevel.empty()) {
            targetLevel = "dark_level.txt";
        }

        // تحميل المستوى فوراً بدلاً من انتظار timer
        if (loadLevel(targetLevel)) {
        }
        else {
        }
    }
    catch (const std::exception& e) {
    }
}