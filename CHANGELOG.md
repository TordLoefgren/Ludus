# Changelog

All notable changes to this project will be documented in this file.   

The project uses [Semantic Versioning](https://semver.org/).


## [v0.1.0] - 2025-10-19

### Added

- #### Ludus::Engine
  - Added registries for game objects, transforms, and colliders.
  - Introduced scene and object management system.
  - Implemented deterministic time-step and timer utilities.
  - Added random number generator with seed support.
  - Introduced layer-based collision filtering.
  - Added debug and assert functionality.
	
- #### Ludus::Graphics
  - Implemented 2D renderer supporting quads, circles, lines, and text.
  - Added shaders, textures, and buffer abstractions.
  - Added color utilities and camera system.
  - Introduced basic text alignment and glyph rendering.

- #### Ludus::Math
  - Added core math primitives: vector, transform, and shape types.
  - Introduced numeric constants and helper functions.

- #### Ludus::Physics
  - Implemented AABB collision detection and MTV resolution.
  - Added broadphase and narrowphase collision steps.
  - Added contact data structure for collision details.

- #### Ludus::Platform
  - Added window and input handling.
  - Implemented keyboard input mapping and window configuration.

- #### Infrastructure
  - Added CI pipeline with GitHub Actions.
  - Integrated unit testing using Doctest.
  - Added repository structure, labels, and project templates.
  - Introduced semantic versioning and release flow.

- #### Demo Game - Pong (1972)
  - Created full demo built on Ludus systems.
  - Implemented game states: Menu, Playing, Paused, Score.
  - Added AI and multiplayer modes.
