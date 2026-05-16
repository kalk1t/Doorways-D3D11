#pragma once

#include "ImportedSceneSettings.h"
#include "Camera.h"
#include "Player.h"


struct WorldState
{
    Camera MainCamera;
    Player MainPlayer;

    ImportedSceneSettings PrimaryScene;

    float SceneTime = 0.0f;
};