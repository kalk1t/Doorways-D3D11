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
## Milestone 17 — Ancient Stone Material / Texture Pipeline

Milestone 17 added the first working material and texture pipeline for Blender-authored OBJ scenes.

### Goal

Move beyond flat blockout colors by applying a real stone/marble texture to the ancient Greek porch and temple scene.

### Completed

- Added a Poly Haven marble/stone texture to the project asset folder.
- Applied the marble material to the ancient Greek porch and temple in Blender.
- Generated usable UV coordinates for the porch and temple geometry.
- Exported the textured Blender scene as OBJ/MTL.
- Confirmed that the OBJ file includes UV coordinates.
- Confirmed that the MTL file references the diffuse texture through `map_Kd`.
- Updated the file references the diffuse texture through `map_Kd`.
- Updated the OBJ loading pipeline to read basic material information.
- Added first-pass `.mtl` parsing support:
  - reads `mtllib` from the OBJ file
  - opens the referenced `.mtl` file
  - finds the first `map_Kd` diffuse texture path
  - resolves texture paths relative to the MTL file location
- Updated the renderer to load the OBJ diffuse texture with `CreateWICTextureFromFile`.
- Added a Shader Resource View for the imported primary scene texture.
- Updated imported scene rendering to bind the loaded texture instead of always using the white fallback texture.
- Confirmed the textured ancient Greek scene renders successfully in DirectX.

### Current Pipeline

```text
Blender scene
-> OBJ export
-> MTL material file
-> map_Kd diffuse texture
-> DirectX OBJ loader
-> WIC texture loader
-> ShaderResourceView
-> HLSL texture sampling
-> rendered textured scene