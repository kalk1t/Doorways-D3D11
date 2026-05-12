#include "EnvironmentSettings.h"
#include <cmath>

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
        
        settings.ClearColor = XMFLOAT4(0.010f, 0.018f, 0.055f, 1.0f);

        
        float moonPulse = 0.5f + 0.5f * sinf(environmentTime * 0.45f);

        float moonStrength = 0.48f + moonPulse * 0.06f;
        float ambientStrength = 0.095f + moonPulse * 0.015f;

        settings.Lighting.LightDirection =
            XMFLOAT4(-0.35f, -0.78f, 0.28f, 0.0f);

#if 0
        settings.Lighting.LightColor =
            XMFLOAT4(moonStrength * 0.72f,moonStrength * 0.82f,moonStrength,1.0f);

       
        settings.Lighting.AmbientColor =
            XMFLOAT4(ambientStrength * 0.75f,ambientStrength * 0.85f,ambientStrength,1.0f);
#else

        settings.Lighting.LightColor =
            XMFLOAT4(0.05f, 0.05f, 0.08f, 1.0f);

        settings.Lighting.AmbientColor =
            XMFLOAT4(0.01f, 0.01f, 0.015f, 1.0f);
#endif
        break;
    }

    return settings;
}
