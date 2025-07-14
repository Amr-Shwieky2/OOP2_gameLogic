# 🎮 OOP2 Game Logic - 2D Platformer Adventure

A sophisticated 2D platformer game built with modern C++ design patterns, featuring dynamic lighting, physics simulation, and a complete three-level adventure journey.

![Language](https://img.shields.io/badge/C%2B%2B-23-blue.svg)
![Graphics](https://img.shields.io/badge/SFML-2.6.1-green.svg)
![Physics](https://img.shields.io/badge/Box2D-Latest-orange.svg)
![Build](https://img.shields.io/badge/CMake-3.26%2B-red.svg)

## 🌟 Game Features

### 🔥 Core Gameplay
- **Three Unique Levels**: Normal Level → Dark Level → Voice Level
- **Dynamic Lighting System**: Advanced dark level with flashlight mechanics
- **Physics-Based Movement**: Powered by Box2D for realistic interactions
- **Multiple Enemy Types**: Smart AI with different behaviors
- **Collectible System**: Coins, gifts, and power-ups

### 🌙 Dark Level System
- **Realistic Lighting**: Dynamic shadows and light sources
- **Mouse-Controlled Flashlight**: Full directional control with camera-aware coordinates
- **Shadow Casting**: Objects cast realistic shadows from light sources
- **Atmospheric Effects**: Ambient lighting and flickering effects
- **Battery System**: Flashlight management with battery depletion

### 🎯 Player Mechanics
- **State Pattern Implementation**: Normal, Shielded, and Boosted states
- **Responsive Controls**: Smooth movement and jumping
- **Health System**: Lives management with game over handling
- **Score System**: Point collection and tracking
- **Power-ups**: Speed boost, shield, and special abilities

### 🤖 Enemy AI
- **FalconEnemyEntity**: Flying enemies with patrol patterns
- **SmartEnemyEntity**: Adaptive AI that responds to player actions
- **SquareEnemyEntity**: Ground-based enemies with collision detection

## 🏗️ Architecture & Design Patterns

### 🧩 Entity-Component System (ECS)Entity → Base class for all game objects
├── PlayerEntity → Player character with state management
├── EnemyEntity → Base for all enemy types
├── CoinEntity → Collectible items
├── WellEntity → Level transition portals
└── FlagEntity → Level completion triggers
### 🎨 Design Patterns Used
- **Entity-Component System**: Modular game object composition
- **State Pattern**: Player behavior management
- **Observer Pattern**: Event-driven communication
- **Factory Pattern**: Entity creation and management
- **Singleton Pattern**: Resource and system management
- **Strategy Pattern**: AI behavior implementation

### 🏛️ System ArchitectureGameSession
