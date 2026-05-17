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

    //Directional light
    settings.Lighting.LightColor = XMFLOAT4(
        0.81f + moonPulse * 0.36f,
        0.72f + moonPulse * 0.36f,
        0.99f + moonPulse * 0.18f,
        1.0f);

    //Ambient light
    settings.Lighting.AmbientColor = XMFLOAT4(
        0.90f,
        0.705f,
        0.660f,
        1.0f);

    return settings;
}