## Third-Party Notices

This repository includes third-party software and assets. This file documents
those components and where their license terms can be found.

Unless otherwise stated, copyright for Ludus source code remains with the
Ludus project and is licensed separately.

### 1) Dear ImGui
- Component: Dear ImGui
- Location: `UI/Vendors/imgui`
- Upstream: https://github.com/ocornut/imgui
- License: MIT
- Local license text: `UI/Vendors/imgui/LICENSE.txt`

Notes:
- The ImGui vendored directory includes `imstb_rectpack.h`,
  `imstb_textedit.h`, and `imstb_truetype.h`, which are stb-derived files and
  contain their own dual license notices (MIT or Public Domain) in-file.

### 2) GLFW
- Component: GLFW 3
- Location: `Engine/Vendors/GLFW`
- Upstream: https://www.glfw.org/
- License: zlib/libpng-style license
- Local license text: Header notice in `Engine/Vendors/GLFW/include/GLFW/glfw3.h`
- Local copied license file: `Engine/Vendors/GLFW/LICENSE.txt`

### 3) GLAD and Khronos Header
- Component: glad OpenGL loader (generated)
- Location: `Engine/Vendors/glad`
- Upstream: https://github.com/Dav1dde/glad
- License: MIT (project), plus Khronos license for `khrplatform.h`
- Local license text:
  - Khronos header notice in `Engine/Vendors/glad/include/KHR/khrplatform.h`
  - Generated file headers in `Engine/Vendors/glad/include/glad/glad.h` and
    `Engine/Vendors/glad/src/glad.c`
- Local copied license file:
  - `Engine/Vendors/glad/LICENSE.txt`
  - `Engine/Vendors/glad/LICENSE.khrplatform.txt`

### 4) GLM
- Component: OpenGL Mathematics (GLM)
- Location: `Engine/Vendors/glm`
- Upstream: https://github.com/g-truc/glm
- License: MIT (alternative to Happy Bunny License)
- Local license text: `Engine/Vendors/glm/copying.txt`

### 5) stb_image
- Component: stb_image
- Location: `Engine/Vendors/stb_image`
- Upstream: https://github.com/nothings/stb
- License: Dual-licensed MIT or Public Domain (Unlicense)
- Local license text: in-file license block in `Engine/Vendors/stb_image/stb_image.h`
- Local copied license file: `Engine/Vendors/stb_image/LICENSE.txt`

### 6) siv::PerlinNoise
- Component: PerlinNoise (siv::PerlinNoise)
- Location: `Lab/Vendors/PerlinNoise-3.0.0/PerlinNoise.hpp`
- Upstream: https://github.com/Reputeless/PerlinNoise
- License: MIT
- Local license text: header notice in `Lab/Vendors/PerlinNoise-3.0.0/PerlinNoise.hpp`
- Local copied license file: `Lab/Vendors/PerlinNoise-3.0.0/LICENSE.txt`

### 7) GoogleTest (NuGet package)
- Component: GoogleTest package for Visual Studio
- Location: `.tools/packages/Microsoft.googletest.v140.windesktop.msvcstl.static.rt-dyn.1.8.1.8`
- Upstream: https://github.com/google/googletest
- Package source: NuGet package `Microsoft.googletest.v140.windesktop.msvcstl.static.rt-dyn` version `1.8.1.8`
- License: MIT
- Local license text:
  - `.tools/packages/Microsoft.googletest.v140.windesktop.msvcstl.static.rt-dyn.1.8.1.8/license (MIT).txt`
  - `.tools/packages/Microsoft.googletest.v140.windesktop.msvcstl.static.rt-dyn.1.8.1.8/ThirdPartyNotices.txt`

### 8) Font Awesome (font file)
- Component: Font Awesome solid font (`fa-solid-900.ttf`)
- Location: `UI/Resources/Fonts/font-awesome/fa-solid-900.ttf`
- Source URL:
  - https://github.com/components/font-awesome/blob/master/webfonts/fa-solid-900.ttf
- Original project:
  - https://fontawesome.com/
  - https://github.com/FortAwesome/Font-Awesome
- License: Font Awesome Free fonts are licensed under SIL OFL 1.1
- Local notice file: `UI/Resources/Fonts/font-awesome/LICENSE.txt`
- Local OFL license text: `UI/Resources/Fonts/font-awesome/OFL.txt`

### 9) Liberation Sans
- Component: Liberation Sans
- Location:
  - `Engine/Resources/Fonts/liberation-sans`
  - `UI/Resources/Fonts/liberation-sans`
- Upstream: https://github.com/liberationfonts/liberation-fonts
- License: SIL Open Font License 1.1
- Local license text:
  - `Engine/Resources/Fonts/liberation-sans/SIL Open Font License.txt`
  - `UI/Resources/Fonts/liberation-sans/SIL Open Font License.txt`
