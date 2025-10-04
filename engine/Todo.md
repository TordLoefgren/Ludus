# TODO for Ludus Game Engine

## Ludus

### Past
- Rendering.
  - [x] Basic text rendering.
  - [x] Create distinct buffers for lines and for quads.
  - [x] DrawCircle.
  - [x] Write a circle shader (prototype shader at https://www.shadertoy.com/new)
  - [x] Add a DrawCircle Renderer2D method.
  - [x] Update Renderer2D to draw centered quads everywhere instead of using corner system.
  - [x] Refactor DrawQuad. Too many overloads.
- Physics.
  - [x] Implement a LayerMask that colliders can use.
  - [x] Create naive narrow collision step system with a collision manager and collider objects.
    - [x] Create collision manager with a Step method and a collider collection.
    - [x] Create collider objects with transform and layer mask.
    - [x] Create a layer mask that is used to tag collider objects.
    - [x] Build a ColliderSystem that manages a collider collection.
    - [x] Build a TransformSystem that manages a game object transform collection.
    - [x] Implement a Minimum Translation Vector (MTV) collision handling. This will include defining a penetration and normal property on the colliders. 
    - [x] Implement a version of Unity's Bounds struct, supplies Center and Min/Max references: https://docs.unity3d.com/6000.2/Documentation/ScriptReference/Bounds.html
    - [x] Have the collision manager use layer masks when checking for collisions.
    - [x] Check for collisions according to the layer mask collision pairs.
- Rendering.
  - [x] Generalize coordinate system: ~1. Corner system (Left, Top, Right, Bottom)~, 2. Center system (Center, HalfExtents), ~3. Min/Max system (X1, Y1, X2, Y2)~. 
- Miscellaneous.
  - [x] Add an interval counter that serves as a basis for cooldowns (like firing projectiles at a fixed rate).

### In Progress  
- [ ] 

 
### Future
- Physics.
  - [ ] RigidBody.
  - [ ] Create shapes for the collider. Start with simple shapes: Box and Circle. 
  - [ ] Have the collision manager use collider shapes to perform collision checks. Broad phase / narrow phase? 
  - [ ] Implement multiple colliders per game object handle. Add ViewByOwner to expose only owner handles. Use Owner handles when saving collision info.
- Rendering.
  - [ ] Add flexibility for text rendering. Centered text and fonts.
  - [ ] Add a fill parameter for drawing quads and circles.
  - [ ] Add IsTrigger property to Collider2D. Update ColliderSystem to maintain a separate trigger collection. 
  - [ ] Have the collision manager use isTrigger to either add triggers or physical collisions.
- Game engine UI.
  - [ ] Debug info window (FPS, draw calls, buffers) (togglable).
  - [ ] Game engine UI helpers showing objects and options (like Unity).
  - [ ] Consider allowing Ludus to execute games directly (vs library usage). Could be useful for an editor/launcher later.
  - [ ] Debug logging system.
  - [ ] Create helper draw method to show collider size / circumference (Needs fill being implemented in the shader for quads and circles).
- Miscellaneous.
  - [ ] Add generic resize logic for GLFW window Callbacks and event subscriptions.
  - [ ] Create helper functions similar to LINQ in C#, or wrappers around C++ equivalent functions.
  - [ ] Consider adding SDL-like window option bit flags: https://wiki.libsdl.org/SDL3/SDL_WindowFlags.
  - [ ] Consider adding a Constants header to include properties and types that are used in many places.
  - [ ] Update Random class to use a template for generic random number values.

---

## Games  
(Complexity: 1 = simple, 3 = complex)

### In Progress  

#### Combat (1977) (1)  
- [x] Create tanks as circles with a gun barrel.
- [x] Collisions for tank on tank.
- [x] Collisions for tank on boundary.
- [x] Collisions for tank on projectile.
- [x] Add despawn logic for tanks and projectiles.
- [x] Collision handling for tank on tank.
- [x] Collision handling for tank on boundary.
- [x] Collision handling for tank on projectile.
- [x] Add layer masks for all possible collision pairs.
- [x] Add Transforms and colliders for boundaries.
- [x] Refactor SpawnProjectile function.
- [x] Add fire rate to projectiles (needs **Ludus** interval counter system).
- [ ] Add additional collider for tank gun barrels (needs **Ludus** collider system update).
- [ ] Enemy movement AI.  
  - [ ] Define field of view for AI. Consider raycasting. 
- [x] Enemy attack AI.  
  - [ ] Consider adding a rotational speed (needs **Ludus** vector updates).
- [ ] Level loader.  
  - [ ] Consider a grid view of the game. Grids can all be game objects with transforms and colliders. 
**Engine demands:** collisions, simple AI, input abstraction  

#### Pong (1972) (1)  
- [x] Random ball direction at startup (needs Ludus randomizer)  
- [x] Add vertical bounds for opponent  
- [x] Use unit vector direction + scalar speed  
- [x] Reflection on ball wall bounces (needs Vector2D)  
- [x] Game menu (needs **Ludus** text renderer).  
- [x] Game score text (needs **Ludus** text renderer).  
- [x] Frame-rate independent movement (delta time / game clock)  
- [x] Game state struct + reset logic  
- [x] Input: single key press vs continuous press  
- [x] Multiplayer mode  
  - [x] Submenu for selecting mode  
  - [x] Arrow key logic for second player  
- [ ] Refactor paddle/ball/boundaries to use Transform2D  
- [ ] Refactor paddle/ball/boundaries to use Transform2D  
- [ ] Remove Rectangle2D.
- [ ] Refactor simulation section to only integrate already computed inputs.
- [ ] Menu text update: symmetric, centered, and correctly scaled text (needs **Ludus** shader update).  
- [ ] Create Main Menu return button in the pause menu. Enables the player(s) to change game type without restarting the game.
**Engine demands:** AABB collisions, text rendering, input system  

---

### Future  

#### Breakout (1976) / Arkanoid (1986) (1)  
- [ ] Ball vs. multiple bricks  
- [ ] Powerups  
**Engine demands:** entity management, collision events, particle effects  

#### Snake / Blockade (1976 / 1997) (1)  
- [ ] Snake tail growth  
- [ ] Self-collision  
**Engine demands:** tilemap/grid system, simple state resets  

#### Asteroids (1979) (1)  
- [ ] Ship rotation + thrust  
- [ ] Bullet shooting  
- [ ] Screen wrapping  
**Engine demands:** vector math (rotation, acceleration), projectile system  

#### Flappy Bird (2013) (1)  
- [ ] Endless scrolling pipes  
- [ ] Gravity + jump physics  
**Engine demands:** camera scrolling, parallax backgrounds, gravity simulation  

#### Space Invaders (1978) / Galaga (1981) (2)  
- [ ] Rows of enemies + movement patterns  
- [ ] Player bullets + scoring  
**Engine demands:** object pooling, sprite batching, scoring/levels  

#### Pac-Man (1980) (2)  
- [ ] Maze with pellets  
- [ ] Ghost AI (chase / scatter)  
**Engine demands:** tilemap navigation, pathfinding (BFS), multiple game states  

#### Platformer (Mario-lite, ~1985) (2–3)  
- [ ] Jumping and gravity  
- [ ] Simple enemies  
- [ ] Scrolling camera  
**Engine demands:** physics resolution, camera follow, layered rendering  

#### Bomberman (1983) (2–3)  
- [ ] Bomb placement  
- [ ] Explosions + destructible blocks  
**Engine demands:** tilemap grid, explosion propagation, entity-component relations  

#### Tetris (1984) (1–2)  
- [ ] Falling blocks  
- [ ] Line clearing  
- [ ] Speed increases  
**Engine demands:** game loop timing, grid snapping, input buffering  

#### Tower Defense (1990s) (3)  
- [ ] Enemy pathing  
- [ ] Turrets + projectiles  
- [ ] Wave spawning  
**Engine demands:** pathfinding, wave logic, projectile management, UI overlays  
