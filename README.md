# Doorways

Doorways is a small Direct3D 11 graphics demo written in C++.

The project is being built as a learning and portfolio project. The goal is not to use a ready-made engine, but to build the rendering foundation step by step and understand how the main parts of a 3D program work.

The demo will start on a porch surrounded by mountains. In front of the player will be several labeled doors. Each door will lead to a different environment, such as a rainy scene, snowy scene and sunny scene.

## Project Goal

The main goal of Doorways is to practice the Direct3D 11 knowledge.

This project is meant to demonstrate:

- Direct3D 11 window and device initialization
- swap chain, render target, depth buffer, and viewport setup
- the 3D rendering pipeline
- vertex buffers and index buffers
- world, view, and projection matrices
- camera movement
- basic scene organization
- lighting and materials
- HLSL shader programming
- texture loading and texture mapping
- simple environment transitions
- basic character or player movement
- Git and GitHub project organization

## Project Vision

The player begins on a wooden porch with a mountain view.

The porch contains several doors with labels. Each door represents a separate environment. When the player enters a door, the scene changes to that environment.

Planned first environments:

- Rainy environment
- Snowy environment
- Sunny environment

The first version of the project will focus on simple geometry and clean structure before adding textures, models, and polish.

## Technology

- C++
- Direct3D 11
- HLSL
- Visual Studio
- Git / GitHub

## Current Development Update — Porch Environment Refocus

The project direction has been updated to focus first on building a stronger and more cinematic porch environment before expanding the rest of the world.

The porch is no longer just a flat starting platform. It will be developed into a large terrace-style structure with:

- a wide wooden/stone porch floor
- high front stairs leading down into the future field area
- a temple structure behind the porch
- a temple entrance behind the player
- a future grassy/rocky field at the bottom of the stairs
- later connection back to the main door teleportation system

This update prepares the renderer for a more focused environment-building phase.

---

## Rendering Order Update

`DrawScene()` now renders the world in this order:

```cpp
DrawNightSky(viewProjection);
DrawMoon(viewProjection);
DrawPorchEnvironment(viewProjection);
DrawPlayer(viewProjection);