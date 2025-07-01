# OOP2 Game Logic

This project implements the core game logic for a small 2D platformer built in C++.
It uses [SFML](https://www.sfml-dev.org/) for graphics and window management and [Box2D](https://box2d.org/) for physics simulation. The code is organized using an entity-component system and applies several design patterns including the State pattern for player behavior.

## Building

The project uses CMake. A preset file `CMakePresets.json` is provided. To build the debug configuration run:

```bash
cmake --preset x64-Debug
cmake --build --preset x64-Debug
```

Assets from `resources/` are copied to the build directory at configure time.

## Key Modules

- **Entity & Components** – `Entity` is the base class for all objects in the game. Entities own components such as `PhysicsComponent`, `RenderComponent`, `InputComponent` and `AIComponent` to implement behavior.
- **EntityManager** – Keeps track of all entities and updates them every frame.
- **GameSession** – Manages the Box2D world, entity manager, collision system and loading of levels.
- **PlayerEntity** – Represents the player. It uses a State pattern (`PlayerState`) with concrete states `NormalState`, `ShieldedState` and `BoostedState` located in `PlayerStates.cpp`.
- **EnemyEntity** – Base class for enemies. Derived classes include `SquareEnemyEntity`, `FalconEnemyEntity` and `SmartEnemyEntity` which use different AI strategies.
- **CollectibleEntity** – Base for collectible items. `CoinEntity` and `GiftEntity` inherit from it and trigger events on collection.
- **SurpriseBoxManager** – After collecting a certain number of coins it opens a bonus screen (`SurpriseBoxScreen`) that spawns special gifts.
- **Events** – `GameEvents.h` defines event types like `ScoreChangedEvent` and `PlayerStateChangedEvent` to decouple game logic.

## Player States

Player behavior changes at runtime through state classes:

- **NormalState** – Default movement and abilities.
- **ShieldedState** – Grants temporary invulnerability and uses a transparent texture.
- **BoostedState** – Increases movement speed and jump impulse for a short duration.

`PlayerEntity` holds a pointer to the current `PlayerState` and delegates input handling to it.

## Levels and Assets

Level descriptions reside in `resources/levels/`. Images, fonts and other assets live under `resources/images/`, `resources/icons/`, and `resources/fonts/`. They are copied next to the executable via CMake rules.

---
This README only covers the main classes and game status system. See the source files in `include/` and `src/` for detailed implementation.
