# Changelog

All notable changes to this project will be documented in this file.

The project follows [Semantic Versioning](https://semver.org/).


## [v0.3.0] - 2026-06-13

### Added

#### Editor

- Added a welcome screen for creating projects and opening recent projects.
- Added command-line selection of the project opened at editor startup.
- Added an unsaved changes prompt for scene and project changes.
- Added keyboard shortcuts for saving, building scripts, building runtimes, and controlling simulation.
- Added persisted editor preferences, panel state, runtime launch settings, and shared editor themes.
- Improved the inspector and hierarchy panels, including adding and removing entity components.
- Prevented editor UI input from interfering with game input while simulation is running.

#### Runtime builds

- Added standalone Windows runtime building and packaging from the editor.
- Added editor-built runtime packages containing the executable, manifest, scenes, assets, and scripts.
- Added runtime-owned scene presentation and scene switching.

#### Scripting

- Added C++ scripting with generated project and solution files.
- Added script building from the editor and script loading at runtime.
- Added scripting access to components and runtime scene transitions.

#### Assets and rendering

- Added stable asset identifiers and runtime texture loading with caching.
- Added asset-backed sprite authoring and rendering.
- Added visible fallback rendering for missing or invalid textures.
- Added content-browser classification for registered, candidate, missing, and unsupported files.
- Added manual inclusion of candidate texture files from the content panel.

### Changed

- Separated the editor, editor host, and game runtime so standalone games no longer depend on editor state.
- Separated editor project data from the manifests used by standalone runtime builds.
- Introduced independent integer revisions for project and runtime manifest schemas.
- Changed asset and scene references to use project-relative paths.
- Changed asset discovery so refreshing project files no longer modifies the project manifest.
- Separated the scripting ABI from the user-facing scripting API.
- Updated Windows builds to the MSVC v145 toolset and removed unused Win32 configurations.
- Project and runtime manifests now require schema revision 1. Manifests created with earlier versions are not supported.
- Reorganized previous changelog entries into consistent release categories.



## [v0.2.0] - 2026-02-20

### Added

#### Editor

- Introduced the Editor MVP with dockable panels and a framebuffer-backed viewport.
- Added play, pause, and stop controls.
- Added hierarchy, inspector, console, and grid panels.
- Added centralized command handling for Editor actions and UI input.
- Added editor session state and selection handling.

#### Runtime and rendering

- Added modular system registration and clearer game-loop phases.
- Added event dispatch and GLFW input events.
- Added multi-scene management and execution controls.
- Added pass-based and multi-viewport rendering.
- Added orthographic camera units.

#### Persistence

- Added project and scene saving and loading.
- Added the human-readable LML format.
- Added file dialogs and menus for opening, saving, and loading projects and scenes.

### Changed

- Renamed `GameObject` to `Entity` and continued component naming cleanup.
- Refactored editor systems to reduce direct ImGui coupling.
- Standardized world and local vertex attributes.
- Added precompiled headers to improve build times.
- Migrated dependency management to vcpkg manifest mode.
- Moved CI to GitHub-hosted runners.
- Updated the README to reflect the Editor-first direction of Ludus.

### Removed

- Removed the Pong and Lab projects as development shifted toward the editor.



## [v0.1.0] - 2025-10-19

### Added

#### Core

- Added registries for game objects, transforms, and colliders.
- Added scene and object management.
- Added deterministic time steps, timers, seeded random numbers, debugging, and assertions.

#### Graphics and math

- Added 2D rendering for quads, circles, lines, and text.
- Added shaders, textures, buffers, colors, cameras, and text alignment.
- Added core vector, transform, and shape types.

#### Physics

- Added AABB collision detection and minimum translation vector resolution.
- Added broadphase and narrowphase collision handling.
- Added layer-based collision filtering and collision contact data.

#### Platform and infrastructure

- Added window, keyboard input, and window configuration support.
- Added GitHub Actions CI and Doctest unit testing.
- Established semantic versioning and the initial release process.

#### Pong

- Added a Pong demo with menu, play, pause, score, AI, and multiplayer modes.
