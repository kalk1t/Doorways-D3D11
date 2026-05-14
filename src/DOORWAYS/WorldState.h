#pragma once

#pragma once

#include "Camera.h"
#include "Player.h"

struct Camera;

struct WorldState
{
    Camera MainCamera;
    Player MainPlayer;


    float SceneTime = 0.0f;
};