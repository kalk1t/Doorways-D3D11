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

## Current Milestone

## Milestone 13 — Textured Night Sky Foundation

Milestone 13 improves the porch night environment by replacing the old procedural star-field approach with a textured sky foundation. The goal was to make the sky feel fuller, cleaner, and more cinematic while keeping the Moon as a separate realistic focal object.

### Completed

- Added a dedicated night-sky texture asset for the porch environment.
- Added renderer texture resources for the star sky:
  - `mStarSkyTexture`
  - `mStarSkySRV`
- Cleaned up reusable texture naming:
  - Moon texture renamed to Moon albedo naming.
  - Moon glow texture renamed to reusable soft glow naming.
- Loaded the star-sky texture through DirectXTK/WIC texture loading.
- Updated `DrawNightSky()` to use the star-sky texture instead of a flat color or checker texture.
- Kept the sky material emissive so the background sky is not affected by directional lighting.
- Added separate UV offsets for front, back, left, right, and top sky panels so the same texture feels less obviously repeated.
- Tuned the sky tint for a colder moonlit night mood.
- Added subtle lower-horizon darkening to help the sky blend better with future mountains and terrain.
- Removed the old procedural `DrawStars()` path as the main star-field renderer.


### Result

The porch scene now has a full textured night sky with many stars, better color tone, less repetition across sky panels, and a darker horizon blend. The Moon remains a separate textured sphere with soft glow, while the sky texture now provides the main star-field background.

This gives the porch environment a much stronger nighttime atmosphere and prepares the scene for the next realism passes: mountain silhouettes, waterfall placement, porch fencing, and better environmental composition.