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

## Milestone 19 — Material Quality, Filtering, and Override Pipeline

Milestone 19 improved the material pipeline for imported OBJ scenes and prepared the renderer for higher-quality real 3D assets.

Before this milestone, the imported scene could render multiple materials, but the material system still needed better filtering, per-material tuning, and stronger lighting behavior.

### Goal

Improve imported material quality and make the Blender-to-Direct3D asset workflow cleaner.

The project now keeps Blender-exported files clean:

```text
Blender exports:
OBJ / MTL

Doorways owns:
DMAT material override files

Blender OBJ
-> geometry
-> material assignments with usemtl

Blender MTL
-> Kd diffuse color
-> Ks specular color
-> Ns specular power
-> map_Kd diffuse texture

Doorways DMAT
-> tex_scale
-> specular_strength

ObjLoader
-> combines MTL + DMAT into ObjMaterialData

Renderer
-> converts ObjMaterialData into GpuMaterial
-> loads one diffuse texture per material
-> draws each submesh with its assigned material

HLSL
-> applies texture tiling
-> ambient lighting
-> diffuse lighting
-> specular lighting
-> emissive override