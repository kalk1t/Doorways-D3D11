#pragma once

#include <DirectXMath.h>

struct Player
{
    // Player root position.
  // Think of this as the position of the player's feet on the ground.
    DirectX::XMFLOAT3 Position =
        DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

    // Rotation around the Y axis.
    // This controls which direction the character is facing.
    float Yaw = 0.0f;

    float MoveSpeed = 4.0f;

    // Temporary flat-ground height.
    // Later this will be replaced with real ground / stairs / collision height.
    float GroundY = 0.0f;

    // True while W/A/S/D is producing movement.
    bool IsMoving = false;

    // Timer used later for walking animation.
    // It advances only while the player is moving.
    float WalkCycleTime = 0.0f;

    // Desired ground height under the player.
    // This is calculated from the player's X/Z position.
    float TargetGroundY = 0.0f;

    // How quickly the player visually moves toward TargetGroundY.
    // Higher = snappier. Lower = smoother/slower.
    float HeightFollowSpeed = 12.0f;
};