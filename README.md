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
---

## Milestone 20 — Imported Scene Transform and Asset Validation Tools

Milestone 20 improved the imported scene workflow by making the primary Blender/OBJ scene easier to place, inspect, validate, and tune at runtime.

Before this milestone, the imported scene rendered at a hardcoded identity transform. That worked for the first OBJ pipeline tests, but it was not practical for future real 3D asset work. As the project moves toward real Poly Haven assets and more detailed Blender-authored scenes, imported meshes need clean transform controls and useful diagnostics.

### Goal

Create a stronger asset-import workflow for the primary imported scene.

The imported scene can now be moved, scaled, rotated, reset, and printed at runtime. This makes it easier to tune asset placement without constantly editing code, rebuilding, and guessing values.

### Completed

- Fixed imported mesh specular constant-buffer upload.
- Added `ImportedSceneSettings`.
- Stored imported scene transform settings in `WorldState`.
- Updated `WorldRenderer::DrawImportedScene()` to use world-owned scene transform settings.
- Added a reusable imported-scene world matrix helper.
- Added OBJ load diagnostics:
  - path
  - vertex count
  - index count
  - material count
  - submesh count
- Added material validation warnings for:
  - missing DMAT texture scale overrides
  - missing diffuse texture paths
  - failed material texture loads
  - submeshes with missing material indices
  - submeshes with invalid material indices
- Added runtime imported scene placement controls:
  - move X/Y/Z
  - uniform scale
  - Y-axis rotation
  - reset transform
  - print transform values
- Added window-title feedback for:
  - texture filter mode
  - imported scene position
  - imported scene scale
  - imported scene Y rotation
- Added fast and slow adjustment modes using Shift and Ctrl.
- Improved `ImportedSceneSettings.h` so tuned transform defaults can be pasted back into one clear location.

### Runtime Asset Placement Controls

```text
J / L       = move imported scene left / right
U / O       = move imported scene up / down
I / K       = move imported scene toward temple / stairs
+ / -       = scale imported scene bigger / smaller
Q / E       = rotate imported scene around Y axis

Hold Shift  = faster adjustment
Hold Ctrl   = slower fine adjustment

R           = reset imported scene transform
P           = print current transform to Visual Studio Output
F           = toggle texture filtering mode