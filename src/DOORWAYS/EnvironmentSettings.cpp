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

    settings.Lighting.LightDirection = XMFLOAT4(-0.35f, -0.75f, 0.25f, 0.0f);

    //Directional light
    settings.Lighting.LightColor = XMFLOAT4(
        0.55f + moonPulse * 0.3f,
        0.65f + moonPulse * 0.3f,
        0.90f + moonPulse * 0.3f,
        1.0f);

    //Ambient light
    settings.Lighting.AmbientColor = XMFLOAT4(0.10f, 0.13f, 0.20f, 1.0f);

    return settings;
}