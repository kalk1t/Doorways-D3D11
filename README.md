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

## Milestone 13 — Blender Primary Scene Pipeline

This milestone introduced the first Blender-authored scene pipeline for Doorways.

### Completed

- Created `Doorways_PrimaryScene.blend` as the master Blender scene file.
- Established Blender-to-engine coordinate conventions.
- Built the first primary blockout scene:
  - porch platform
  - railings and posts
  - nine stairs
  - raised temple landing
  - temple body, roof, and columns
  - surrounding mountain placeholders
  - waterfall placeholder
- Exported a single test cube from Blender as OBJ.
- Loaded the exported OBJ successfully in the DirectX engine.
- Exported the full primary blockout scene as OBJ.
- Confirmed the full scene renders correctly in-engine.

### Pipeline Confirmed

The project can now use Blender as a 3D scene authoring tool and export OBJ files into the custom DirectX renderer.

Current pipeline:

```text
Blender scene -> OBJ export -> DirectX OBJ loader -> rendered scene