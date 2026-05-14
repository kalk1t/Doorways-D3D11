#pragma once

#include "RenderTypes.h"

#include <DirectXMath.h>

struct EnvironmentSettings
{
    DirectX::XMFLOAT4 ClearColor;
    PerFrameConstantBuffer Lighting;
};

EnvironmentSettings GetSceneSettings(float sceneTime);
