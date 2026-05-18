#pragma once

struct PlayerMovementBounds
{
    // Main porch / stairs playable rectangle.
    //
    // Your measured scene:
    // X = -9.3 to +9.3
    // Z = -6.0 to +15.0
    //
    // Small safety margin keeps the player body from visually reaching
    // past the porch edge.
    float MinX = -8.4f;
    float MaxX = 8.4f;

    float MinZ = -5.6f;
    float MaxZ = 27.6f;
};

