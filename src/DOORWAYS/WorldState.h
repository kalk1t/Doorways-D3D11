#pragma once

#include "ImportedSceneSettings.h"
#include "Camera.h"
#include "Player.h"
#include "PlayerSpawnSettings.h"
#include "PlayerMovementBounds.h"

struct WorldState
{
    Camera MainCamera;
    Player MainPlayer;

    PlayerSpawnSettings MainPlayerSpawn = GetMainScenePlayerSpawn();

    PlayerMovementBounds MainPlayerBounds;

    ImportedSceneSettings PrimaryScene;

    bool ShowPlayerBoundsDebug = true;

    float SceneTime = 0.0f;
};