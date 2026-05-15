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
## Milestone 18 — Multi-Material OBJ Scene Pipeline

Milestone 18 upgraded the imported OBJ scene pipeline from a single global material into a real multi-material scene renderer.

### Goal

Allow one Blender-authored OBJ scene to contain multiple materials and render each material correctly in DirectX.

Before this milestone, the imported scene used one diffuse texture for the whole OBJ. That was enough to prove the Milestone 17 texture pipeline, but it was not enough for a real scene with stone, water, mountains, wood, and future magical doorway materials.

### Completed

- Cleaned the exported `.mtl` texture path so the material file can use a project-relative texture reference instead of an absolute local Windows path.
- Added CPU-side material data structures:
  - `ObjMaterialData`
  - `SubmeshData`
- Updated `MeshData` to store:
  - vertices
  - indices
  - material list
  - submesh ranges
- Replaced the first-pass MTL parser with a fuller material table parser.
- Added support for:
  - `newmtl`
  - `Kd`
  - `map_Kd`
- Added `usemtl` parsing in the OBJ loader.
- Added submesh range generation based on material switches in the OBJ file.
- Added GPU-side material and submesh structures:
  - `GpuMaterial`
  - `GpuSubmesh`
- Updated `GpuMesh` to store:
  - GPU vertex/index buffers
  - material list
  - submesh list
- Updated renderer material loading so each OBJ material can load its own diffuse texture.
- Updated imported mesh drawing to render by submesh:
  - bind submesh material
  - bind diffuse texture if available
  - fall back to white texture and `Kd` diffuse color if no texture exists
  - call `DrawIndexed` for each submesh range
- Created a Blender multi-material test scene with separate materials for:
  - marble/stone porch and temple
  - blue waterfall blockout
  - dark mountain rock blockout
- Exported the scene as OBJ/MTL.
- Confirmed the DirectX renderer can render different materials from one imported OBJ scene.

### Current Pipeline

```text
Blender scene
-> OBJ export
-> MTL material table
-> newmtl / Kd / map_Kd
-> OBJ usemtl ranges
-> CPU MeshData materials + submeshes
-> GPU materials + submeshes
-> per-submesh DrawIndexed
-> rendered multi-material scene