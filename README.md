<h1 align="center">
  <br>
  <img src=".tools/assets/logo.png" alt="Ludus" width="280">
</h1>

<h4 align="center">
  A C++ game engine and editor for learning and exploring core engine systems.
</h4>

<p align="center">
  <img src="https://img.shields.io/badge/language-C++-00599C?style=flat&logo=c%2B%2B&logoColor=white">
  <img src="https://img.shields.io/badge/rendering-OpenGL-blue?style=flat">
  <img src="https://img.shields.io/badge/platform-Windows-lightgrey?style=flat">
  <img src="https://img.shields.io/badge/status-Prototype-yellow?style=flat">
  <img src="https://img.shields.io/badge/license-MIT-green?style=flat">
</p>

<p align="center"><i>
A personal experiment in graphics, physics, and engine architecture design.
</i></p>

---

## Overview

**Ludus** is a C++ game engine with an integrated editor, developed as a long-term learning and portfolio project.

Rather than aiming for a finished engine product, the goal is to explore and understand:
- Rendering systems
- Editor tooling
- Data-driven architecture
- Physics pipelines, simulation systems, and geometry-focused workflows

Ludus currently targets **2D**, with a clear intent to add **3D** support later.

---

## Architecture

Ludus follows a layered architecture separating:

- **Engine**: Runtime systems used by games (rendering, ECS/component workflows, physics, persistence, and platform abstractions).
- **Editor**: A desktop tooling application built on top of the engine runtime (panels, inspectors, and scene/project workflows).
- **UI**: Reusable Dear ImGui abstraction and utility layer used by editor tooling.

This separation is enforced throughout rendering, input, persistence, and tooling systems.

---

## Editor (Current State)

The editor is built on **Dear ImGui** and follows a layered architecture clearly separating UI, editor logic, and engine runtime.
It is a client of the engine runtime, not part of the runtime itself.

Implemented or in active development:
- Dockable panel system (Hierarchy, Viewport, Inspector, Console, Project).
- Scene hierarchy and entity inspection.
- Component-based editing.
- Orthographic camera and 2D viewport rendering.
- Basic extensible physics pipeline.
- Event system.
- Explicit control over framebuffers.
- Project and scene serialization with persistence layer.
- Logging, assertions, and debug tooling.

<figure>
  <img src=".tools/assets/editor.png" alt="Ludus Editor" width="900"/>
  <figcaption><i>Editor view while developing an in-engine Pong prototype (currently not playable, pending scripting integration).</i></figcaption>
</figure>

---

## Project Status

Ludus is **not yet distributed**.

- Builds and runs locally.
- Some third-party dependencies and build steps are not fully automated.
- This repository represents an **in-progress** engine, not a consumer-ready product.
- Third-party dependency and license details are documented in `THIRD_PARTY_NOTICES.md`.

Before reaching a “complete editor” milestone, planned additions include:

- Scripting integration.
- Asset import pipeline.
- Build system for packaging games and executables.

This repository exists primarily as a **technical portfolio** and architectural exploration.

---

## Tech Highlights

- Engine stack: C++20, OpenGL rendering backend, custom math library, extensible physics and event systems, custom serialization / persistence system.
- Editor stack: Dear ImGui–based editor.
- Testing: GoogleTest.

---

## Repository Structure

```text
Engine/   -> Runtime library
Editor/   -> Desktop editor application
UI/       -> UI wrapper/utilities for editor tooling
Games/    -> Games built directly on the engine as a framework
Lab/      -> Framework-based sandbox for building and iterating on demos
Tests/    -> Unit and integration tests
```

---

## Building

Prerequisites:
- Windows 10/11
- Visual Studio 2022
- Desktop development with C++
- C++20 toolset

```bash
git clone https://github.com/TordLoefgren/Ludus.git
cd Ludus

# Build
msbuild .\Ludus.sln /m /p:Configuration=Debug /p:Platform=x64
```

Notes:
- Ensure any local dependencies are installed before building.

## Running Tests

- Build the `Tests` project in `Ludus.sln`.
- Run tests from Visual Studio Test Explorer.
- Optional (CLI with cache logs):
```powershell
powershell -ExecutionPolicy Bypass -File .\.tools\scripts\invoke-msbuild.ps1 `
  -ProjectOrSolution .\Tests\Tests.vcxproj `
  -Configuration Debug `
  -Platform x64 `
  -LogName tests-build.log `
  -Diagnostics
```
  Logs are written to `.cache\logs\` instead of repository root.

## Inspiration & References
This project draws ideas from great resources in the C++ and graphics programming community:

- **The Cherno** – _Game engine series_  
  https://www.youtube.com/@TheCherno
- **Games with Gabe** – _Coding a 2D Physics Engine series_  
  https://www.youtube.com/@GamesWithGabe
- **LearnOpenGL** – _Modern OpenGL concepts (shaders, VAOs/VBOs, texturing)_  
  https://learnopengl.com/
- **Gaffer on Games** – _Fix-your-timestep, networking, and simulation articles_  
  https://gafferongames.com/
- **Box2D Lite / Erin Catto Notes** – _Collision detection and resolution fundamentals_  
  https://box2d.org/
- **Game Programming Patterns** – _Design and architecture approaches for game development_  
  https://gameprogrammingpatterns.com/
- **IndieGameDev.net** – _Articles, tutorials, and insights on game architecture and engine design_  
  https://indiegamedev.net/
- **GoogleTest** – _Unit testing library used in Ludus_
  https://github.com/google/googletest
- **GLFW / OpenGL References** – _Windowing, input, and API specifications_  
  https://www.glfw.org/ · https://www.khronos.org/opengl/
