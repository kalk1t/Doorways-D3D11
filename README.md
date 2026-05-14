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

## Current Direction

The project is now asset-driven.

Blender is used for:

- importing downloaded 3D assets
- cleaning models
- scaling models
- arranging scene pieces
- preparing game-ready exports
- exporting OBJ files for Doorways

Doorways is used for:

- running the playable scene
- rendering with Direct3D 11
- controlling the player/camera
- loading exported assets
- building the actual game experience

Current asset workflow:

```text
External 3D asset / Blender scene
        ↓
clean and prepare in Blender
        ↓
export as OBJ
        ↓
assets/models/exported/primary_scene.obj
        ↓
Doorways loads the model
        ↓
Direct3D 11 renders the playable scene