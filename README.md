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

---

## Milestone 22 — First Playable Character V1

Milestone 22 moved Doorways from a scene-viewing project into the first playable character version.

The goal of this milestone was not to add a final imported character model yet. Instead, the focus was to build the gameplay foundation required before character art, animation assets, collision, and interaction are added.

### Goal

Add the first visible playable character to the Doorways main scene.

The player can now move through the Blender-authored porch/stairs environment with third-person camera control, simple animation, stair height handling, and debug tools for tuning the playable area.

### Completed

- Enabled the player render in the main scene.
- Replaced the single debug cube with a simple humanoid placeholder character:
  - body
  - head
  - arms
  - legs
  - front-facing marker
- Refactored player drawing into smaller helper functions:
  - `DrawPlayerBody`
  - `DrawPlayerHead`
  - `DrawPlayerArms`
  - `DrawPlayerLegs`
  - `DrawPlayerFrontMarker`
  - `DrawPlayerShadow`
- Added player movement state:
  - idle
  - moving
  - walk cycle timer
- Added simple procedural walking animation.
- Added simple idle breathing animation.
- Added third-person mouse-look camera.
- Added cursor hiding and cursor locking while the game window is active.
- Fixed the Alt-key system-menu freeze issue.
- Added camera follow based on camera yaw.
- Added mouse-wheel camera zoom distance.
- Added player spawn settings.
- Separated player reset from imported scene reset:
  - `R` resets player/camera
  - `T` resets imported scene transform
- Added player movement bounds for the porch/stairs playable area.
- Added debug bounds overlay.
- Added `B` key to show/hide the player bounds debug overlay.
- Added stair height handling.
- Added smooth stair height following.
- Added a simple player contact shadow.
- Added player debug information to the window title:
  - position
  - yaw
  - movement state
  - camera distance
  - bounds debug state

### Current Controls

```text
W / A / S / D      Move player
Mouse movement     Rotate camera
Mouse wheel        Zoom camera in / out
R                  Reset player and camera
B                  Toggle player bounds debug overlay
F                  Toggle texture filtering debug mode
T                  Reset imported scene transform
P                  Print imported scene transform
Escape             Exit application