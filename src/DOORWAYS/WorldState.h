#pragma once

#pragma once

#include "Camera.h"
#include "Player.h"
#include "Environment.h"

class Camera;

struct WorldState
{
    Camera MainCamera;
    Player MainPlayer;

    EnvironmentId NearbyDoor = EnvironmentId::Porch;
    EnvironmentId ActiveEnvironment = EnvironmentId::Porch;

    float EnvironmentTime = 0.0f;
    bool WasInteractKeyDown = false;
};