#include "EnvironmentSettings.h"

#include <cmath>

using namespace DirectX;

EnvironmentSettings GetSceneSettings(float sceneTime)
{
    EnvironmentSettings settings = {};

    settings.ClearColor = XMFLOAT4(
        0.010f,
        0.018f,
        0.055f,
        1.0f);

    float moonPulse = 0.5f + 0.5f * sinf(sceneTime * 0.45f);

    settings.Lighting.LightDirection = XMFLOAT4(
        -0.35f,
        -0.65f,
        0.45f,
        0.0f);

    settings.Lighting.LightColor = XMFLOAT4(
        0.42f + moonPulse * 0.05f,
        0.48f + moonPulse * 0.05f,
        0.68f + moonPulse * 0.07f,
        1.0f);

    settings.Lighting.AmbientColor = XMFLOAT4(
        0.045f,
        0.055f,
        0.095f,
        1.0f);

    return settings;
}