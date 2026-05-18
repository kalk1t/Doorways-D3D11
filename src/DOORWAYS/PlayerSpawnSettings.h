#pragma once

#include <DirectXMath.h>

struct PlayerSpawnSettings
{
    DirectX::XMFLOAT3 Position =
        DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

    float Yaw = 0.0f;
};

inline PlayerSpawnSettings GetMainScenePlayerSpawn()
{
    PlayerSpawnSettings spawn = {};

    // Temporary main-scene spawn point.
    // Adjust these values later after you decide exactly where
    // the player should start in the Blender scene.
    spawn.Position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
    spawn.Yaw = 0.0f;

    return spawn;
}
