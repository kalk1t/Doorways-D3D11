#include "EnvironmentSettings.h"

EnvironmentSettings GetEnvironmentSettings(
    EnvironmentId environment,
    float environmentTime) {

    EnvironmentSettings settings = {};

    switch (environment)
    {
    case EnvironmentId::Sunny:
        settings.ClearColor = XMFLOAT4(0.35f, 0.55f, 0.85f, 1.0f);
        settings.Lighting.LightDirection = XMFLOAT4(0.45f, -0.75f, 0.35f, 0.0f);
        settings.Lighting.LightColor = XMFLOAT4(1.00f, 0.90f, 0.65f, 1.0f);
        settings.Lighting.AmbientColor = XMFLOAT4(0.32f, 0.28f, 0.20f, 1.0f);
        break;
    case EnvironmentId::Rainy:
        settings.ClearColor = XMFLOAT4(0.14f, 0.17f, 0.22f, 1.0f);
        settings.Lighting.LightDirection = XMFLOAT4(0.20f, -0.65f, 0.55f, 0.0f);
        settings.Lighting.LightColor = XMFLOAT4(0.45f, 0.55f, 0.75f, 1.0f);
        settings.Lighting.AmbientColor = XMFLOAT4(0.10f, 0.13f, 0.18f, 1.0f);
        break;
    case EnvironmentId::Snowy:
        settings.ClearColor = XMFLOAT4(0.78f, 0.88f, 0.95f, 1.0f);
        settings.Lighting.LightDirection = XMFLOAT4(-0.35f, -0.80f, 0.25f, 0.0f);
        settings.Lighting.LightColor = XMFLOAT4(0.78f, 0.88f, 1.00f, 1.0f);
        settings.Lighting.AmbientColor = XMFLOAT4(0.35f, 0.40f, 0.45f, 1.0f);
        break;
    case EnvironmentId::Porch:
    default:
        settings.ClearColor = XMFLOAT4(0.05f, 0.08f, 0.12f, 1.0f);

        float porchPulse = 0.5f + 0.5f * sinf(environmentTime * 1.25f);

        float warmStrength = 0.78f + porchPulse * 0.12f;
        float ambientStrength = 0.18f + porchPulse * 0.05f;

        settings.Lighting.LightDirection =
            XMFLOAT4(0.577f, -0.577f, 0.577f, 0.0f);
        settings.Lighting.LightColor =
            XMFLOAT4(warmStrength, warmStrength * 0.92f, warmStrength * 0.78f, 1.0f);

        settings.Lighting.AmbientColor =
            XMFLOAT4(ambientStrength, ambientStrength, ambientStrength + 0.04f, 1.0f);
        break;
    }

    return settings;
}
