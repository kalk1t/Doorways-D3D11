#pragma once
#include <Windows.h>

struct WorldState;

class PlayerController
{
public:
    void Update(WorldState& world,float deltaTime,HWND windowHandle,
        int mouseWheelDelta);


private:
    void UpdateCameraRotation(WorldState& world,HWND windowHandle);
    void UpdateCameraZoom(WorldState& world,int mouseWheelDelta);
    void UpdateCameraFollow(WorldState& world);
    void ClampCameraPitch(WorldState& world);
    void SetMouseLookActive(HWND windowHandle,bool active);


    void UpdatePlayerMovement(WorldState& world, float deltaTime);
    void ResetPlayerAndCamera(WorldState& world);
    void ClampPlayerToPlayableArea(WorldState& world);

private:

    bool mIsMouseLookActive = false;
    bool mIsMouseLookInitialized = false;

    // Radians per mouse pixel.
    // Higher value = faster camera movement.
    float mMouseSensitivity = 0.0035f;

};