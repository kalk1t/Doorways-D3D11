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

---

## Milestone 21 — Main Scene Blender Composition V1

Milestone 21 moved Doorways from a renderer-built environment toward a Blender-authored main starting scene.

The goal of this milestone is to establish the first real version of the main scene: a Greek-style porch and temple placed in the middle of a water environment, with mountains surrounding the scene and two waterfalls.

This milestone treats Blender as the main scene editor while the Direct3D 11 renderer loads the exported scene through the existing OBJ/MTL pipeline.

### Goal

Create the first complete main starting scene composition for Doorways.

The scene should represent the core visual identity of the game:

- Greek-style porch and temple
- 9 stairs leading toward the temple
- water surrounding the central temple area
- Mountains around
- two waterfall locations coming down from the mountain area, behind the temple
- night sky and moon still rendered by the engine
- first real imported prop support through the Blender/OBJ workflow

### Blender Scene Source

Created/updated the main Blender scene file:

```text
assets/blender/main_starting_scene_v1.blend