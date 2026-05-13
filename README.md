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
## Current Milestone Update — Porch / Temple / Stair Environment Lock

The project has reached a new environment milestone focused on building the main porch composition.

The current scene now has a complete readable world layout:

```text
                -Z direction

              [ Raised Temple Area ]
              [ 9 Stairs Up ]
              [ Temple Entrance Depth ]
              [ Temple Front Wall ]
              [ Temple Threshold ]

              [ Porch Terrace ]

              [ Stair Landing ]
              [ 21 Stairs Down ]
              [ Rocky / Grassy Field ]

                +Z direction