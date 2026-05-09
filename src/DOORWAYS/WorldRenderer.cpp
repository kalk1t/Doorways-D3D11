#include "WorldRenderer.h"
#include "App.h"

#include <d3dcompiler.h>
#include <cstddef>
#include <cstdint>
#include <DirectXMath.h>
#include <string>
#include <chrono>
#include <cmath>

using namespace DirectX;


XMMATRIX MakeWorld(
    float scaleX,
    float scaleY,
    float scaleZ,
    float translateX,
    float translateY,
    float translateZ)
{
    return XMMatrixScaling(scaleX, scaleY, scaleZ) *
        XMMatrixTranslation(translateX, translateY, translateZ);
}

WorldRenderer::WorldRenderer(App* app)
    : mApp(app)
{
}

void WorldRenderer::DrawScene(const XMMATRIX& viewProjection)
{
    
    Material floorMaterial =
    {
        XMFLOAT4(0.45f, 0.35f, 0.25f, 1.0f),
        XMFLOAT4(6.0f, 4.0f, 0.0f, 0.0f),
        mApp->mRenderer.mDiffuseTextureView.Get()
    };

    Material leftDoorMaterial =
    {
        mApp->mWorld.ActiveEnvironment == EnvironmentId::Sunny
            ? XMFLOAT4(1.0f, 0.45f, 0.30f, 1.0f)
            : XMFLOAT4(0.55f, 0.15f, 0.12f, 1.0f),

        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        mApp->mRenderer.mDiffuseTextureView.Get()
    };

    Material middleDoorMaterial =
    {
        mApp->mWorld.ActiveEnvironment == EnvironmentId::Rainy
            ? XMFLOAT4(0.35f, 0.65f, 1.0f, 1.0f)
            : XMFLOAT4(0.12f, 0.32f, 0.65f, 1.0f),

        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        mApp->mRenderer.mDiffuseTextureView.Get()
    };

    Material rightDoorMaterial =
    {
        mApp->mWorld.ActiveEnvironment == EnvironmentId::Snowy
            ? XMFLOAT4(0.55f, 1.0f, 0.65f, 1.0f)
            : XMFLOAT4(0.15f, 0.50f, 0.25f, 1.0f),

        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        mApp->mRenderer.mDiffuseTextureView.Get()
    };


    Material sunnyLabelMaterial =
    {
        XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        mApp->mRenderer.mSunnyLabelTextureView.Get()
    };

    Material rainyLabelMaterial =
    {
        XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        mApp->mRenderer.mRainyLabelTextureView.Get()
    };

    Material snowyLabelMaterial =
    {
        XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        mApp->mRenderer.mSnowyLabelTextureView.Get()
    };

    XMMATRIX floorWorld = MakeWorld(
        5.0f, 0.1f, 4.0f,
        0.0f, -0.55f, 0.0f);

    mApp->mRenderer.DrawBox(floorWorld, viewProjection, floorMaterial);

    DrawDoorwayAtmosphere(viewProjection);


    XMMATRIX leftDoorWorld = MakeWorld(
        mApp->mDoorSystem.gDoorWidth, mApp->mDoorSystem.gDoorHeight, mApp->mDoorSystem.gDoorDepth,
        mApp->mDoorSystem.gSunnyDoorX, mApp->mDoorSystem.gDoorY, mApp->mDoorSystem.gDoorZ);
    mApp->mRenderer.DrawBox(leftDoorWorld, viewProjection, leftDoorMaterial);

    XMMATRIX middleDoorWorld = MakeWorld(
        mApp->mDoorSystem.gDoorWidth, mApp->mDoorSystem.gDoorHeight, mApp->mDoorSystem.gDoorDepth,
        mApp->mDoorSystem.gRainyDoorX, mApp->mDoorSystem.gDoorY, mApp->mDoorSystem.gDoorZ);

    mApp->mRenderer.DrawBox(middleDoorWorld, viewProjection, middleDoorMaterial);

    XMMATRIX rightDoorWorld = MakeWorld(
        mApp->mDoorSystem.gDoorWidth, mApp->mDoorSystem.gDoorHeight, mApp->mDoorSystem.gDoorDepth,
        mApp->mDoorSystem.gSnowyDoorX, mApp->mDoorSystem.gDoorY, mApp->mDoorSystem.gDoorZ);

    mApp->mRenderer.DrawBox(rightDoorWorld, viewProjection, rightDoorMaterial);

    XMMATRIX sunnyLabelWorld = MakeWorld(
        mApp->mDoorSystem.gLabelWidth, mApp->mDoorSystem.gLabelHeight, mApp->mDoorSystem.gLabelDepth,
        mApp->mDoorSystem.gSunnyDoorX, mApp->mDoorSystem.gLabelY, mApp->mDoorSystem.gLabelZ);

    mApp->mRenderer.DrawDoorLabel(sunnyLabelWorld, viewProjection, sunnyLabelMaterial);

    XMMATRIX rainyLabelWorld = MakeWorld(
        mApp->mDoorSystem.gLabelWidth, mApp->mDoorSystem.gLabelHeight, mApp->mDoorSystem.gLabelDepth,
        mApp->mDoorSystem.gRainyDoorX, mApp->mDoorSystem.gLabelY, mApp->mDoorSystem.gLabelZ);

    mApp->mRenderer.DrawDoorLabel(rainyLabelWorld, viewProjection, rainyLabelMaterial);

    XMMATRIX snowyLabelWorld = MakeWorld(
        mApp->mDoorSystem.gLabelWidth, mApp->mDoorSystem.gLabelHeight, mApp->mDoorSystem.gLabelDepth,
        mApp->mDoorSystem.gSnowyDoorX, mApp->mDoorSystem.gLabelY, mApp->mDoorSystem.gLabelZ);

    mApp->mRenderer.DrawDoorLabel(snowyLabelWorld, viewProjection, snowyLabelMaterial);



    DrawPlayer(viewProjection);

    DrawInteractionPrompt(viewProjection);

    DrawEnvironmentObjects(viewProjection);


}


void WorldRenderer::DrawPlayer(const XMMATRIX& viewProjection)
{
    Material playerMaterial =
    {
        XMFLOAT4(0.95f, 0.85f, 0.20f, 1.0f),
        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        mApp->mRenderer.mDiffuseTextureView.Get()
    };

    XMMATRIX scale = XMMatrixScaling(
        0.30f,
        0.60f,
        0.45f);

    XMMATRIX rotation = XMMatrixRotationY(mApp->mWorld.MainPlayer.Yaw);

    XMMATRIX translation = XMMatrixTranslation(
        mApp->mWorld.MainPlayer.Position.x,
        mApp->mWorld.MainPlayer.Position.y,
        mApp->mWorld.MainPlayer.Position.z);

    XMMATRIX playerWorld = scale * rotation * translation;

    mApp->mRenderer.DrawBox(playerWorld, viewProjection, playerMaterial);
}

void WorldRenderer::DrawInteractionPrompt(const XMMATRIX& viewProjection)
{
    if (mApp->mWorld.NearbyDoor == EnvironmentId::Porch)
    {
        return;
    }

    float promptX = mApp->mDoorSystem.GetDoorX(mApp->mWorld.NearbyDoor);

    Material promptMaterial =
    {
        XMFLOAT4(1.0f, 0.95f, 0.15f, 1.0f),
        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        mApp->mRenderer.mDiffuseTextureView.Get()
    };

    XMMATRIX promptWorld = MakeWorld(
        0.45f,
        0.08f,
        0.08f,
        promptX,
        1.95f,
        0.95f);

    mApp->mRenderer.DrawBox(
        promptWorld,
        viewProjection,
        promptMaterial);
}

void WorldRenderer::DrawEnvironmentObjects(const XMMATRIX& viewProjection)
{
    switch (mApp->mWorld.ActiveEnvironment)
    {
    case EnvironmentId::Sunny:
        DrawSunnyEnvironment(viewProjection);
        break;

    case EnvironmentId::Rainy:
        DrawRainyEnvironment(viewProjection);
        break;

    case EnvironmentId::Snowy:
        DrawSnowyEnvironment(viewProjection);
        break;

    case EnvironmentId::Porch:
    default:
        break;
    }
}

void WorldRenderer::DrawEnvironmentBackdrop(
    const XMMATRIX& viewProjection,
    const XMFLOAT4& color)
{
    Material backdropMaterial =
    {
        color,
        XMFLOAT4(2.0f, 2.0f, 0.0f, 0.0f),
        mApp->mRenderer.mDiffuseTextureView.Get()
    };

    XMMATRIX backdropWorld = MakeWorld(
        5.40f,
        2.60f,
        0.05f,
        0.0f,
        0.80f,
        1.55f);

    mApp->mRenderer.DrawBox(
        backdropWorld,
        viewProjection,
        backdropMaterial);
}

void WorldRenderer::DrawSunnyEnvironment(const XMMATRIX& viewProjection)
{
    DrawEnvironmentBackdrop(
        viewProjection,
        XMFLOAT4(0.95f, 0.68f, 0.28f, 1.0f));

    auto GetCloudX = [this](float startX, float speed) -> float
        {
            constexpr float minX = -2.70f;
            constexpr float maxX = 2.70f;
            constexpr float range = maxX - minX;

            float x = startX + mApp->mWorld.EnvironmentTime * speed;

            while (x > maxX)
            {
                x -= range;
            }

            while (x < minX)
            {
                x += range;
            }

            return x;
        };

    Material cloudMaterial =
    {
        XMFLOAT4(0.96f, 0.90f, 0.72f, 1.0f),
        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        mApp->mRenderer.mDiffuseTextureView.Get()
    };

    struct Cloud
    {
        float StartX;
        float Y;
        float Speed;
        float Scale;
    };

    const Cloud clouds[] =
    {
        { -2.20f, 2.70f, 0.18f, 1.00f },
        {  0.40f, 2.88f, 0.12f, 0.75f },
        {  1.80f, 2.20f, 0.15f, 0.85f }
    };

    for (const Cloud& cloud : clouds)
    {
        float cloudX = GetCloudX(cloud.StartX, cloud.Speed);

        XMMATRIX cloudCenterWorld = MakeWorld(
            0.55f * cloud.Scale,
            0.14f * cloud.Scale,
            0.04f,
            cloudX,
            cloud.Y,
            1.02f);

        mApp->mRenderer.DrawBox(
            cloudCenterWorld,
            viewProjection,
            cloudMaterial);

        XMMATRIX cloudLeftWorld = MakeWorld(
            0.28f * cloud.Scale,
            0.22f * cloud.Scale,
            0.04f,
            cloudX - 0.28f * cloud.Scale,
            cloud.Y + 0.05f * cloud.Scale,
            1.02f);

        mApp->mRenderer.DrawBox(
            cloudLeftWorld,
            viewProjection,
            cloudMaterial);

        XMMATRIX cloudRightWorld = MakeWorld(
            0.34f * cloud.Scale,
            0.20f * cloud.Scale,
            0.04f,
            cloudX + 0.32f * cloud.Scale,
            cloud.Y + 0.04f * cloud.Scale,
            1.02f);

        mApp->mRenderer.DrawBox(
            cloudRightWorld,
            viewProjection,
            cloudMaterial);
    }

    float pulse = 0.5f + 0.5f * sinf(mApp->mWorld.EnvironmentTime * 2.0f);

    float sunScale = 0.45f + pulse * 0.08f;
    float rayScale = 1.0f + pulse * 0.25f;

    Material sunMaterial =
    {
        XMFLOAT4(1.0f, 0.85f + pulse * 0.10f, 0.10f, 1.0f),
        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        mApp->mRenderer.mDiffuseTextureView.Get()
    };

    Material rayMaterial =
    {
        XMFLOAT4(1.0f, 0.70f + pulse * 0.15f, 0.15f, 1.0f),
        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        mApp->mRenderer.mDiffuseTextureView.Get()
    };

    XMMATRIX sunWorld = MakeWorld(
        sunScale,
        sunScale,
        0.08f,
        0.0f,
        2.45f,
        1.00f);

    mApp->mRenderer.DrawBox(
        sunWorld,
        viewProjection,
        sunMaterial);

    struct SunRay
    {
        float BaseScaleX;
        float BaseScaleY;
        float ScaleZ;
        float X;
        float Y;
        float Z;
        bool AnimateX;
        bool AnimateY;
    };

    const SunRay rays[] =
    {
        // Top ray
        { 0.10f, 0.35f, 0.06f,  0.00f, 2.95f, 1.00f, false, true },

        // Bottom ray
        { 0.10f, 0.35f, 0.06f,  0.00f, 1.95f, 1.00f, false, true },

        // Left ray
        { 0.35f, 0.10f, 0.06f, -0.50f, 2.45f, 1.00f, true, false },

        // Right ray
        { 0.35f, 0.10f, 0.06f,  0.50f, 2.45f, 1.00f, true, false }
    };

    for (const SunRay& ray : rays)
    {
        float scaleX = ray.AnimateX
            ? ray.BaseScaleX * rayScale
            : ray.BaseScaleX;

        float scaleY = ray.AnimateY
            ? ray.BaseScaleY * rayScale
            : ray.BaseScaleY;

        XMMATRIX rayWorld = MakeWorld(
            scaleX,
            scaleY,
            ray.ScaleZ,
            ray.X,
            ray.Y,
            ray.Z);

        mApp->mRenderer.DrawBox(
            rayWorld,
            viewProjection,
            rayMaterial);
    }
}

void WorldRenderer::DrawRainyEnvironment(const XMMATRIX& viewProjection)
{
    DrawEnvironmentBackdrop(
        viewProjection,
        XMFLOAT4(0.10f, 0.16f, 0.28f, 1.0f));

    Material cloudMaterial =
    {
        XMFLOAT4(0.06f, 0.08f, 0.12f, 1.0f),
        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        mApp->mRenderer.mDiffuseTextureView.Get()
    };

    auto GetStormCloudX = [this](float startX, float speed) -> float
        {
            constexpr float minX = -2.80f;
            constexpr float maxX = 2.80f;
            constexpr float range = maxX - minX;

            float x = startX + mApp->mWorld.EnvironmentTime * speed;

            while (x > maxX)
            {
                x -= range;
            }

            while (x < minX)
            {
                x += range;
            }

            return x;
        };

    struct StormCloud
    {
        float StartX;
        float Y;
        float Speed;
        float Scale;
    };

    const StormCloud stormClouds[] =
    {
        { -2.00f, 2.72f, 0.10f, 1.10f },
        { -0.30f, 2.88f, 0.07f, 0.95f },
        {  1.40f, 2.68f, 0.09f, 1.05f }
    };

    for (const StormCloud& cloud : stormClouds)
    {
        float cloudX = GetStormCloudX(cloud.StartX, cloud.Speed);

        XMMATRIX cloudBodyWorld = MakeWorld(
            0.85f * cloud.Scale,
            0.18f * cloud.Scale,
            0.04f,
            cloudX,
            cloud.Y,
            1.02f);

        mApp->mRenderer.DrawBox(
            cloudBodyWorld,
            viewProjection,
            cloudMaterial);

        XMMATRIX cloudTopWorld = MakeWorld(
            0.45f * cloud.Scale,
            0.22f * cloud.Scale,
            0.04f,
            cloudX - 0.20f * cloud.Scale,
            cloud.Y + 0.10f * cloud.Scale,
            1.02f);

        mApp->mRenderer.DrawBox(
            cloudTopWorld,
            viewProjection,
            cloudMaterial);
    }

    Material rainMaterial =
    {
        XMFLOAT4(0.25f, 0.55f, 1.0f, 1.0f),
        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        mApp->mRenderer.mDiffuseTextureView.Get()
    };

    auto GetRainY = [this](float startY, float speed) -> float
        {
            constexpr float minY = 1.45f;
            constexpr float maxY = 2.85f;
            constexpr float range = maxY - minY;

            float y = startY - mApp->mWorld.EnvironmentTime * speed;

            while (y < minY)
            {
                y += range;
            }

            while (y > maxY)
            {
                y -= range;
            }

            return y;
        };

    auto GetRainX = [this](float startX, float phase) -> float
        {
            constexpr float windAmount = 0.10f;
            constexpr float windSpeed = 4.0f;

            return startX + sinf(mApp->mWorld.EnvironmentTime * windSpeed + phase) * windAmount;
        };

    struct RainDrop
    {
        float X;
        float StartY;
        float Speed;
        float Length;
        float Phase;
    };

    const RainDrop rainDrops[] =
    {
        { -2.10f, 2.35f, 1.60f, 0.55f, 0.0f },
        { -1.70f, 2.70f, 1.35f, 0.45f, 0.7f },
        { -1.25f, 2.15f, 1.80f, 0.60f, 1.4f },
        { -0.85f, 2.60f, 1.45f, 0.50f, 2.1f },
        { -0.40f, 2.30f, 1.70f, 0.55f, 2.8f },
        {  0.00f, 2.80f, 1.30f, 0.45f, 3.5f },
        {  0.45f, 2.20f, 1.90f, 0.65f, 4.2f },
        {  0.85f, 2.65f, 1.50f, 0.50f, 4.9f },
        {  1.30f, 2.25f, 1.75f, 0.60f, 5.6f },
        {  1.75f, 2.75f, 1.40f, 0.45f, 6.3f },
        {  2.15f, 2.40f, 1.65f, 0.55f, 7.0f }
    };

    for (const RainDrop& drop : rainDrops)
    {
        float x = GetRainX(drop.X, drop.Phase);
        float y = GetRainY(drop.StartY, drop.Speed);

        XMMATRIX rainScale = XMMatrixScaling(
            0.04f,
            drop.Length,
            0.04f);

        XMMATRIX rainSlant = XMMatrixRotationZ(-0.18f);

        XMMATRIX rainTranslation = XMMatrixTranslation(
            x,
            y,
            1.00f);

        XMMATRIX rainWorld = rainScale * rainSlant * rainTranslation;

        mApp->mRenderer.DrawBox(
            rainWorld,
            viewProjection,
            rainMaterial);
    }

    float puddlePulse = 0.5f + 0.5f * sinf(mApp->mWorld.EnvironmentTime * 3.0f);

    Material puddleMaterial =
    {
        XMFLOAT4(0.10f, 0.22f + puddlePulse * 0.06f, 0.34f + puddlePulse * 0.08f, 1.0f),
        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        mApp->mRenderer.mDiffuseTextureView.Get()
    };

    struct Puddle
    {
        float ScaleX;
        float ScaleZ;
        float X;
        float Z;
    };

    const Puddle puddles[] =
    {
        { 0.70f, 0.20f, -1.30f, -0.90f },
        { 0.55f, 0.18f,  0.20f, -0.35f },
        { 0.80f, 0.22f,  1.30f, -1.10f }
    };

    for (const Puddle& puddle : puddles)
    {
        XMMATRIX puddleWorld = MakeWorld(
            puddle.ScaleX,
            0.025f,
            puddle.ScaleZ,
            puddle.X,
            -0.48f,
            puddle.Z);

        mApp->mRenderer.DrawBox(
            puddleWorld,
            viewProjection,
            puddleMaterial);
    }
}

void WorldRenderer::DrawSnowyEnvironment(const XMMATRIX& viewProjection)
{
    DrawEnvironmentBackdrop(
        viewProjection,
        XMFLOAT4(0.72f, 0.86f, 0.96f, 1.0f));

    Material mountainMaterial =
    {
        XMFLOAT4(0.50f, 0.62f, 0.72f, 1.0f),
        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        mApp->mRenderer.mDiffuseTextureView.Get()
    };

    Material mountainSnowMaterial =
    {
        XMFLOAT4(0.88f, 0.94f, 1.0f, 1.0f),
        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        mApp->mRenderer.mDiffuseTextureView.Get()
    };

    struct Mountain
    {
        float X;
        float BaseY;
        float ScaleX;
        float ScaleY;
    };

    const Mountain mountains[] =
    {
        { -1.70f, 1.10f, 1.00f, 1.40f },
        {  0.00f, 1.00f, 1.35f, 1.65f },
        {  1.65f, 1.12f, 0.95f, 1.30f }
    };

    for (const Mountain& mountain : mountains)
    {
        XMMATRIX mountainWorld = MakeWorld(
            mountain.ScaleX,
            mountain.ScaleY,
            0.04f,
            mountain.X,
            mountain.BaseY,
            1.03f);

        mApp->mRenderer.DrawBox(
            mountainWorld,
            viewProjection,
            mountainMaterial);

        XMMATRIX snowCapWorld = MakeWorld(
            mountain.ScaleX * 0.45f,
            mountain.ScaleY * 0.22f,
            0.04f,
            mountain.X,
            mountain.BaseY + mountain.ScaleY * 0.42f,
            1.01f);

        mApp->mRenderer.DrawBox(
            snowCapWorld,
            viewProjection,
            mountainSnowMaterial);
    }

    float mistOffset = sinf(mApp->mWorld.EnvironmentTime * 0.55f) * 0.15f;

    Material mistMaterial =
    {
        XMFLOAT4(0.82f, 0.92f, 1.0f, 1.0f),
        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        mApp->mRenderer.mDiffuseTextureView.Get()
    };

    XMMATRIX mistWorld = MakeWorld(
        4.80f,
        0.16f,
        0.035f,
        mistOffset,
        1.38f,
        0.99f);

    mApp->mRenderer.DrawBox(
        mistWorld,
        viewProjection,
        mistMaterial);

    Material snowMaterial =
    {
        XMFLOAT4(0.90f, 0.95f, 1.0f, 1.0f),
        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        mApp->mRenderer.mDiffuseTextureView.Get()
    };

    Material snowflakeMaterial =
    {
        XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        mApp->mRenderer.mDiffuseTextureView.Get()
    };

    struct SnowPatch
    {
        float ScaleX;
        float ScaleY;
        float ScaleZ;
        float X;
        float Y;
        float Z;
    };

    const SnowPatch snowPatches[] =
    {
        { 0.75f, 0.05f, 0.45f, -1.55f, -0.46f, -0.95f },
        { 0.90f, 0.05f, 0.45f, -0.55f, -0.45f,  0.05f },
        { 0.85f, 0.05f, 0.45f,  0.55f, -0.46f, -0.75f },
        { 0.70f, 0.05f, 0.45f,  1.55f, -0.45f,  0.05f }
    };

    for (const SnowPatch& patch : snowPatches)
    {
        XMMATRIX snowWorld = MakeWorld(
            patch.ScaleX,
            patch.ScaleY,
            patch.ScaleZ,
            patch.X,
            patch.Y,
            patch.Z);

        mApp->mRenderer.DrawBox(
            snowWorld,
            viewProjection,
            snowMaterial);
    }

    auto GetSnowY = [this](float startY, float speed) -> float
        {
            constexpr float minY = 1.25f;
            constexpr float maxY = 2.90f;
            constexpr float range = maxY - minY;

            float y = startY - mApp->mWorld.EnvironmentTime * speed;

            while (y < minY)
            {
                y += range;
            }

            while (y > maxY)
            {
                y -= range;
            }

            return y;
        };

    auto GetSnowX = [this](float startX, float phase, float driftAmount) -> float
        {
            constexpr float driftSpeed = 1.15f;

            float slowDrift = sinf(mApp->mWorld.EnvironmentTime * driftSpeed + phase) * driftAmount;
            float windPush = sinf(mApp->mWorld.EnvironmentTime * 0.35f) * 0.10f;

            return startX + slowDrift + windPush;
        };

    struct Snowflake
    {
        float StartX;
        float StartY;
        float Phase;
        float Speed;
        float Size;
        float Drift;
    };

    const Snowflake snowflakes[] =
    {
        { -2.10f, 2.35f, 0.0f, 0.26f, 0.08f, 0.10f },
        { -1.65f, 2.75f, 1.1f, 0.32f, 0.11f, 0.16f },
        { -1.15f, 2.45f, 2.2f, 0.22f, 0.07f, 0.12f },
        { -0.70f, 2.85f, 3.3f, 0.36f, 0.10f, 0.18f },
        { -0.20f, 2.30f, 4.4f, 0.28f, 0.09f, 0.14f },
        {  0.25f, 2.70f, 5.5f, 0.24f, 0.07f, 0.10f },
        {  0.75f, 2.40f, 6.6f, 0.34f, 0.12f, 0.18f },
        {  1.20f, 2.80f, 7.7f, 0.25f, 0.08f, 0.13f },
        {  1.70f, 2.50f, 8.8f, 0.30f, 0.10f, 0.16f },
        {  2.15f, 2.90f, 9.9f, 0.21f, 0.07f, 0.11f }
    };

    for (const Snowflake& flake : snowflakes)
    {
        float x = GetSnowX(
            flake.StartX,
            flake.Phase,
            flake.Drift);

        float y = GetSnowY(
            flake.StartY,
            flake.Speed);

        XMMATRIX flakeWorld = MakeWorld(
            flake.Size,
            flake.Size,
            0.04f,
            x,
            y,
            0.98f);

        mApp->mRenderer.DrawBox(
            flakeWorld,
            viewProjection,
            snowflakeMaterial);
    }
}

void WorldRenderer::DrawDoorwayAtmosphere(const XMMATRIX& viewProjection)
{
    struct DoorGlow
    {
        EnvironmentId Door;
        XMFLOAT4 BaseColor;
    };

    const DoorGlow glows[] =
    {
        { EnvironmentId::Sunny, XMFLOAT4(1.00f, 0.62f, 0.12f, 1.0f) },
        { EnvironmentId::Rainy, XMFLOAT4(0.18f, 0.45f, 1.00f, 1.0f) },
        { EnvironmentId::Snowy, XMFLOAT4(0.75f, 0.95f, 1.00f, 1.0f) }
    };

    float pulse = 0.5f + 0.5f * sinf(mApp->mWorld.EnvironmentTime * 3.0f);

    for (const DoorGlow& glow : glows)
    {
        float intensity = 0.35f;

        if (mApp->mWorld.NearbyDoor == glow.Door)
        {
            intensity = 0.65f + pulse * 0.25f;
        }

        if (mApp->mWorld.ActiveEnvironment == glow.Door)
        {
            intensity = 0.85f + pulse * 0.15f;
        }

        Material glowMaterial =
        {
            XMFLOAT4(
                glow.BaseColor.x * intensity,
                glow.BaseColor.y * intensity,
                glow.BaseColor.z * intensity,
                1.0f),
            XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
            mApp->mRenderer.mDiffuseTextureView.Get()
        };

        float doorX = mApp->mDoorSystem.GetDoorX(glow.Door);

        XMMATRIX glowWorld = MakeWorld(
            mApp->mDoorSystem.gDoorWidth + 0.35f,
            mApp->mDoorSystem.gDoorHeight + 0.45f,
            0.04f,
            doorX,
            mApp->mDoorSystem.gDoorY + 0.05f,
            mApp->mDoorSystem.gDoorZ + 0.04f);  
        mApp->mRenderer.DrawBox(
            glowWorld,
            viewProjection,
            glowMaterial);

        XMMATRIX smallTopGlowWorld = MakeWorld(
            mApp->mDoorSystem.gDoorWidth + 0.65f + pulse * 0.12f,
            0.08f,
            0.04f,
            doorX,
            mApp->mDoorSystem.gDoorY + mApp->mDoorSystem.gDoorHeight * 0.55f + 0.30f,
            mApp->mDoorSystem.gDoorZ + 0.02f);

        mApp->mRenderer.DrawBox(
            smallTopGlowWorld,
            viewProjection,
            glowMaterial);
    }
}