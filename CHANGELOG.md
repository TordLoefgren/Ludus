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



## [v0.2.0] - 2026-02-20

### Added

- #### Ludus::Editor
  - Introduced Editor MVP with dockable panels and a framebuffer-backed viewport.
  - Added core editor playback controls (Play / Pause / Stop).
  - Added hierarchy panel MVP (create/select/remove entities).
  - Added inspector panel MVP with live-editing support for selected entities/components.
  - Added editor console panel and aggregate console output.
  - Added request-command architecture for Editor actions and UI-driven workflows.
  - Added editor session/state infrastructure and centralized editor context.
  - Added editor grid render pass.

- #### Ludus::Engine::Core
  - Introduced `Application` as the central application lifecycle abstraction.
  - Added system-component interface and modular system registration.
  - Separated game loop responsibilities into clearer phases and ownership boundaries.
  - Added EventBus system with event dispatch and handler subscription.
  - Added GLFW input callback events routed through the EventBus.
  - Added Scene and SceneRegistry for multi-scene management.
  - Added execution flags and system constraints for controlling runtime behavior.

- #### Ludus::Engine::Graphics
  - Introduced pass-based rendering pipeline architecture.
  - Added multi-viewport rendering support (via view / render requests).
  - Added orthographic camera units + viewport refactor and decoupled rendering responsibilities.

- #### Ludus::Engine::Persistence / Serialization
  - Added engine persistence interfaces for project and scene persistence.
  - Implemented human-readable text format (LML) and codec.
  - Added DOM-based persistence archives and object serializers.
  - Added persistence IO utilities and tests.
  - Added Editor project persistence layer (save/load projects and multiple scenes).
  - Added platform + UI support for persistence workflows.
  - Added Open File Dialog integration for opening projects/scenes.
  - Restructured Persistence namespace and promoted Serialization to a top-level namespace.

### Changed

- #### Architecture / Naming
  - Renamed `GameObject` to `Entity` and continued component naming cleanup.
  - Refactored editor systems for clearer panel interfaces and less direct ImGui coupling.
  - Standardized world vs. local vertex attributes and explicit casting patterns.
  - Added precompiled headers for improved build throughput and build iteration.


- #### Infrastructure
  - Migrated dependency management to vcpkg manifest mode.
  - Updated CI pipeline for a public repository setup (GitHub-hosted runner; removed local runner).
  - Cleaned up issue templates (removed redundant Status field / labels).
  - Updated README structure and content for the current direction of the project.
  - Renamed project folders and updated namespace structures.

### Removed

- #### Repository
  - Removed redundant projects (Pong and Lab) as the repo shifted focus to the editor workflow.

### Notes

- This release represents a significant focus shift from demo-game delivery toward an Editor-first architecture.
- Physics / collision improvements planned under the milestone were deprioritized in favor of editor and persistence work.
