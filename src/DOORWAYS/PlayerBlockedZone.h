#pragma once

#include <Windows.h>
#include <DirectXMath.h>

inline bool IsMainSceneBlockedPosition(
    const DirectX::XMFLOAT3& position)
{
    // Step 13 correction:
    //
    // The porch/stairs playable area is already handled by
    // PlayerMovementBounds:
    //
    // X = -9.3 to +9.3
    // Z = -6.0 to +15.0
    //
    // So for now there are no extra internal blocked zones.
    // Water is blocked by the outer movement bounds.

    UNREFERENCED_PARAMETER(position);

    return false;
}