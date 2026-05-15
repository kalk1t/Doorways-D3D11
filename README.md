# Doorways

Doorways is a Direct3D 11 game project written in C++.

The project started as a learning-focused graphics demo, but it has now moved into a playable 3D game direction. The goal is to build an atmospheric game world using a custom Direct3D 11 renderer, imported 3D assets, Blender-based scene preparation, and step-by-step engine improvements.

The current focus is no longer to manually build the entire world from hardcoded boxes. Instead, Doorways now uses Blender as the asset preparation and scene-building tool, then exports game-ready OBJ files into the Direct3D runtime.

---

## Project Vision

Doorways is intended to become a playable atmospheric 3D experience.

The player begins in a mysterious night environment under a moonlit sky. The world will gradually be built from real 3D assets: ruins, stone structures, terrain, rocks, wooden objects, mountains, temple pieces, and other environment details.

The long-term vision is to create a scene that feels like a real game space rather than a renderer test:

- a playable 3D world
- strong night atmosphere
- moon and sky presence
- imported 3D environment assets
- first playable exploration mechanics
- clean camera/player controls
- eventually doorways, transitions, and different environments

The project will continue to grow through small playable milestones.

---
## Milestone 16 — Ancient Greek Porch and Temple Foundation

Milestone 16 replaced the simple blockout porch and temple with a stronger ancient Greek architectural foundation.


### Completed

- Replaced the single porch floor block with individual ancient stone slabs.
- Added a low stone trim/parapet around the porch platform.
- Preserved the center stair opening toward the temple.
- Replaced the simple stair blocks with wider ceremonial stone steps.
- Added stair cheek walls on both sides of the stairway.
- Replaced square porch posts with simple Greek-style columns:
  - base blocks
  - cylindrical shafts
  - capital blocks
  - collar accents
- Added additional porch trim details:
  - platform base
  - top cap stones
  - front decorative band
- Replaced the old temple blockout with a clearer ancient Greek temple structure:
  - landing platform
  - cella/main chamber
  - front and back colonnades
  - entablature beams
  - roof block
  - triangular pediments
- Exported the updated scene from Blender as OBJ.
- Loaded and rendered the Milestone 16 scene successfully in the DirectX engine.

### Current Visual Direction

The environment now follows an ancient Greek architectural style. The porch and temple are designed as a connected stone ceremonial approach rather than a modern wooden porch.

### Pipeline Confirmed

```text
Blender ancient Greek scene
-> OBJ export
-> DirectX OBJ loader
-> rendered scene