<h1 align="center">
  <br>
  <img src=".tools/assets/logo.png" alt="Ludus" width="280">
</h1>

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



## Overview

**Ludus** is a **C++** game engine and integrated editor built as a long-term learning and portfolio project.

Right now, Ludus is focused on **2D** engine and editor development, with **3D** support planned later. It serves as a space to explore rendering, editor design, data-driven architecture, and physics systems.



## Architecture

Ludus is organized into layers, with each layer building on the one below it:

- `Engine` is the runtime core
- `UI` provides shared user interface components for the editor
- `Editor` adds tools for creating and editing content
- `EditorHost` serves as the desktop application shell
- `Scripting` defines the script-facing application programming interface (API) and application binary interface (ABI)


<img
  src=".tools/assets/ludus_reduced_dependency_diagram.svg"
  alt="Reduced dependency diagram of the Ludus repository"
  width="340"
/>

*Simplified dependency view of the repository, showing the main architectural layers and their test projects rather than every direct project reference.*



## Current Snapshot

Ludus is in active development and currently focused on **2D** engine and editor features. Simple projects such as Pong can be created in the editor and built into runnable Windows builds.

<p align="center">
  <img src=".tools/assets/editor_snapshot.png" alt="Ludus Editor" width="900"/>
</p>

<p align="center"><i>
Pong created in the editor and built into a runnable build.
</i></p>



## Quick Start

```cmd
git clone https://github.com/TordLoefgren/Ludus.git
cd Ludus
msbuild .\Ludus.sln /m /p:Configuration=Debug /p:Platform=x64 /p:VcpkgEnableManifest=true
```



## Requirements

- Windows 10/11
- Visual Studio 2022
- Desktop development with C++
- C++20 toolset
- `vcpkg`



## Inspiration & References
This project is influenced by several resources from the C++ and graphics programming community:

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
- **GLFW / OpenGL References** – _Windowing, input, and API specifications_  
  https://www.glfw.org/ · https://www.khronos.org/opengl/
