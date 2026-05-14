#pragma once

struct WorldState;

class PlayerController
{
public:
    void Update(
        WorldState& world,
        float deltaTime);

private:
    void UpdateCameraRotation(WorldState& world,float deltaTime);

    void UpdatePlayerMovement(WorldState& world,float deltaTime);

    void UpdateCameraFollow(WorldState& world);

    void ClampCameraPitch(WorldState& world);

    void ResetPlayerAndCamera(WorldState& world);

};