#pragma once

#include "Environment.h"
#include <DirectXMath.h>

using namespace DirectX;

struct EnvironmentLighting
{
    XMFLOAT4 LightDirection;
    XMFLOAT4 LightColor;
    XMFLOAT4 AmbientColor;
};

struct EnvironmentSettings
{
    XMFLOAT4 ClearColor;
    EnvironmentLighting Lighting;
};


EnvironmentSettings GetEnvironmentSettings(
    EnvironmentId environment,
    float environmentTime);
