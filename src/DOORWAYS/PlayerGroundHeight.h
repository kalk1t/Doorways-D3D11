#pragma once

#include <Windows.h>
#include <algorithm>
#include <cmath>

// ------------------------------------------------------------
// Temporary Milestone 22 scene-height function.
//
// This is not real mesh collision yet.
// This is hand-authored gameplay height logic based on the
// current main scene layout.
//
// Current measured / known layout:
// Porch floor is around Y = 0.
// Porch extends roughly from Z = -6 to Z = +6.
// Stairs begin near Z = +5.5 and climb toward Z = +14.5.
// There are 9 stairs.
// Each stair rises by about 0.25.
// ------------------------------------------------------------
inline float GetMainSceneGroundHeight(
    float x,
    float z)
{
    // X is currently unused because the stairs are full-width
    // across the central playable area.
    // Later, X can be used for side stairs, ramps, rocks, etc.
    UNREFERENCED_PARAMETER(x);

    constexpr float porchGroundY = 0.0f;

    constexpr float stairStartZ = 5.5f;
    constexpr float stairDepth = 1.0f;
    constexpr int stairCount = 9;
    constexpr float stairHeight = 0.25f;

    constexpr float stairEndZ =
        stairStartZ + stairDepth * static_cast<float>(stairCount);

    constexpr float templeLandingY =
        stairHeight * static_cast<float>(stairCount);

    // Flat porch / lower area.
    if (z < stairStartZ)
    {
        return porchGroundY;
    }

    // Top landing after the stairs.
    if (z >= stairEndZ)
    {
        return templeLandingY;
    }

    // Stair area.
    float stairProgress =
        (z - stairStartZ) / stairDepth;

    int stairIndex =
        static_cast<int>(std::floor(stairProgress));

    stairIndex =
        std::clamp(
            stairIndex,
            0,
            stairCount - 1);

    // First stair top is 0.25, second is 0.50, etc.
    return static_cast<float>(stairIndex + 1) * stairHeight;
}
