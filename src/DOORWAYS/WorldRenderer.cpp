#include "WorldRenderer.h"
#include "App.h"

#include "ImportedSceneSettings.h"


#include <d3dcompiler.h>
#include <cstddef>
#include <cstdint>
#include <DirectXMath.h>
#include <string>
#include <chrono>
#include <cmath>

using namespace DirectX;

XMMATRIX MakeWorld(
    float scaleX,float scaleY,float scaleZ,
    float translateX,float translateY,float translateZ)
{
    return XMMatrixScaling(scaleX, scaleY, scaleZ) *
        XMMatrixTranslation(translateX, translateY, translateZ);
}

XMMATRIX MakeWorldSRT(
    float scaleX,float scaleY,float scaleZ,
    float rotX,float rotY,float rotZ,
    float translateX,float translateY,float translateZ)
{
    return XMMatrixScaling(scaleX, scaleY, scaleZ) *
        XMMatrixRotationRollPitchYaw(rotX, rotY, rotZ) *
        XMMatrixTranslation(translateX, translateY, translateZ);
}


XMMATRIX BuildImportedSceneWorldMatrix(
    const ImportedSceneSettings& sceneSettings)
{
    return
        XMMatrixScaling(
            sceneSettings.Scale.x,
            sceneSettings.Scale.y,
            sceneSettings.Scale.z) *

        XMMatrixRotationRollPitchYaw(
            sceneSettings.Rotation.x,
            sceneSettings.Rotation.y,
            sceneSettings.Rotation.z) *

        XMMatrixTranslation(
            sceneSettings.Translation.x,
            sceneSettings.Translation.y,
            sceneSettings.Translation.z);
}


WorldRenderer::WorldRenderer(App* app)
    : mApp(app)
{
}






void WorldRenderer::DrawScene(const XMMATRIX& viewProjection)
{

    DrawNightSky(viewProjection);
    DrawMoon(viewProjection);

    DrawImportedScene(viewProjection);


    DrawPlayerShadow(viewProjection);
    DrawPlayer(viewProjection);
    DrawPlayerBoundsDebug(viewProjection);

}

void WorldRenderer::DrawNightSky(const XMMATRIX& viewProjection)
{
    const XMFLOAT4 skyTint =
        XMFLOAT4(1.10f, 1.14f, 1.28f, 1.0f);

    Material frontSkyMaterial =
    {
        skyTint,
        XMFLOAT4(1.0f, 1.0f, 0.00f, 0.00f),
        mApp->mRenderer.mStarSkySRV.Get(),
        XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
    1.0f,
    1.0f
    };

    Material backSkyMaterial =
    {
        skyTint,
        XMFLOAT4(1.0f, 1.0f, 0.35f, 0.10f),
        mApp->mRenderer.mStarSkySRV.Get(),
       XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
    1.0f,
    1.0f
    };

    Material leftSkyMaterial =
    {
        skyTint,
        XMFLOAT4(1.0f, 1.0f, 0.70f, 0.20f),
        mApp->mRenderer.mStarSkySRV.Get(),
        XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
    1.0f,
    1.0f
    };

    Material rightSkyMaterial =
    {
        skyTint,
        XMFLOAT4(1.0f, 1.0f, 0.15f, 0.35f),
        mApp->mRenderer.mStarSkySRV.Get(),
        XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
    1.0f,
    1.0f
    };

    Material topSkyMaterial =
    {
        skyTint,
        XMFLOAT4(1.25f, 1.25f, 0.25f, 0.15f),
        mApp->mRenderer.mStarSkySRV.Get(),
        XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
    1.0f,
    1.0f
    };

    constexpr float skyDistance = 90.0f;
    constexpr float skyHeight = 60.0f;
    constexpr float skyPanelSize = 180.0f;
    constexpr float skyThickness = 0.10f;
    constexpr float skyCenterY = 25.0f;
    constexpr float skyTopY = 55.0f;

    // Front sky wall.
    XMMATRIX frontSkyWorld = MakeWorld(
        skyPanelSize,
        skyHeight,
        skyThickness,
        0.0f,
        skyCenterY,
        skyDistance);

    mApp->mRenderer.DrawBox(
        frontSkyWorld,
        viewProjection,
        frontSkyMaterial);

    // Back sky wall.
    XMMATRIX backSkyWorld = MakeWorld(
        skyPanelSize,
        skyHeight,
        skyThickness,
        0.0f,
        skyCenterY,
        -skyDistance);

    mApp->mRenderer.DrawBox(
        backSkyWorld,
        viewProjection,
        backSkyMaterial);

    // Left sky wall.
    XMMATRIX leftSkyWorld = MakeWorld(
        skyThickness,
        skyHeight,
        skyPanelSize,
        -skyDistance,
        skyCenterY,
        0.0f);

    mApp->mRenderer.DrawBox(
        leftSkyWorld,
        viewProjection,
        leftSkyMaterial);

    // Right sky wall.
    XMMATRIX rightSkyWorld = MakeWorld(
        skyThickness,
        skyHeight,
        skyPanelSize,
        skyDistance,
        skyCenterY,
        0.0f);

    mApp->mRenderer.DrawBox(
        rightSkyWorld,
        viewProjection,
        rightSkyMaterial);

    // Top sky ceiling.
    XMMATRIX topSkyWorld = MakeWorld(
        skyPanelSize,
        skyThickness,
        skyPanelSize,
        0.0f,
        skyTopY,
        0.0f);

    mApp->mRenderer.DrawBox(
        topSkyWorld,
        viewProjection,
        topSkyMaterial);



    // ------------------------------------------------------------
    // Subtle horizon darkening.
    // This helps the lower sky blend with future mountains/terrain.
    // ------------------------------------------------------------

    Material horizonDarkMaterial =
    {
        XMFLOAT4(0.005f, 0.008f, 0.020f, 0.42f),
        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        mApp->mRenderer.mWhiteTextureView.Get(),
       XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
    1.0f,
    1.0f
    };

    mApp->mRenderer.SetAlphaBlendingEnabled(true);

    // Front lower sky darkening.
    XMMATRIX frontHorizonWorld = MakeWorld(
        skyPanelSize,
        12.0f,
        0.06f,
        0.0f,
        0.2f,
        skyDistance - 0.08f);

    mApp->mRenderer.DrawBox(
        frontHorizonWorld,
        viewProjection,
        horizonDarkMaterial);

    // Back lower sky darkening.
    XMMATRIX backHorizonWorld = MakeWorld(
        skyPanelSize,
        5.0f,
        0.06f,
        0.0f,
        0.2f,
        -skyDistance + 0.08f);

    mApp->mRenderer.DrawBox(
        backHorizonWorld,
        viewProjection,
        horizonDarkMaterial);

    // Left lower sky darkening.
    XMMATRIX leftHorizonWorld = MakeWorld(
        0.06f,
        5.0f,
        skyPanelSize,
        -skyDistance + 0.08f,
        0.2f,
        0.0f);

    mApp->mRenderer.DrawBox(
        leftHorizonWorld,
        viewProjection,
        horizonDarkMaterial);

    // Right lower sky darkening.
    XMMATRIX rightHorizonWorld = MakeWorld(
        0.06f,
        5.0f,
        skyPanelSize,
        skyDistance - 0.08f,
        0.2f,
        0.0f);

    mApp->mRenderer.DrawBox(
        rightHorizonWorld,
        viewProjection,
        horizonDarkMaterial);

    mApp->mRenderer.SetAlphaBlendingEnabled(false);


}

void WorldRenderer::DrawMoon(const XMMATRIX& viewProjection)
{


    Material moonMaterial =
    {
        XMFLOAT4(
            1.35f,
            1.35f,
            1.35f,
            1.00f),

        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),

        mApp->mRenderer.mMoonSRV.Get(),

    XMFLOAT4(0.10f, 0.10f, 0.12f, 0.3f),
    32.0f,
    0.81f
    };

    Material moonGlowMaterial =
    {
        XMFLOAT4(
            1.50f,
            1.65f,
            2.20f,
            1.0f),

        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),

        mApp->mRenderer.mSoftGlowSRV.Get(),
            XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
    1.0f,
    1.0f
    };

	constexpr float moonScaleX = 2.05f;
    constexpr float moonScaleY = 2.05f;
    constexpr float moonScaleZ = 2.08f;

	constexpr float moonRotX = 0.0f;
	constexpr float moonRotY = -XM_PIDIV2;
    constexpr float moonRotZ = 0.0f;

    constexpr float moonX = 7.5f;
    constexpr float moonY = 33.0f;
    constexpr float moonZ = 26.80f;

    

    // Main moon body.
    XMMATRIX moonWorld = MakeWorldSRT(
        moonScaleX,moonScaleY,moonScaleZ,
        moonRotX,moonRotY,moonRotZ,
        moonX,moonY,moonZ);

    mApp->mRenderer.DrawSphere(
        moonWorld,
        viewProjection,
        moonMaterial);

    XMMATRIX moonGlowWorld = MakeWorldSRT(
        3.20f,3.20f,0.03f,
        0.0f,0.0f,0.0f,
        moonX,moonY,moonZ + 0.75f);

    mApp->mRenderer.SetAlphaBlendingEnabled(true);

    mApp->mRenderer.DrawBox(
        moonGlowWorld,
        viewProjection,
        moonGlowMaterial);

    mApp->mRenderer.SetAlphaBlendingEnabled(false);
}


void WorldRenderer::DrawImportedScene(const XMMATRIX& viewProjection)
{
    Material sceneMaterial =
    {
        XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        nullptr,
        XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        32.0f,
        0.0f
    };

    const ImportedSceneSettings& sceneSettings =
        mApp->mWorld.PrimaryScene;

	XMMATRIX sceneWorld = BuildImportedSceneWorldMatrix(sceneSettings);

    mApp->mRenderer.DrawMesh(
        mApp->mRenderer.mPrimarySceneMesh,
        sceneWorld,
        viewProjection,
        sceneMaterial);
}



void WorldRenderer::DrawPlayer(const XMMATRIX& viewProjection)
{
    const Player& player =
        mApp->mWorld.MainPlayer;

    XMMATRIX playerWorld =
        XMMatrixRotationY(player.Yaw) *
        XMMatrixTranslation(
            player.Position.x,
            player.Position.y,
            player.Position.z);

    Material bodyMaterial =
    {
        XMFLOAT4(0.95f, 0.78f, 0.22f, 1.0f),
        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        mApp->mRenderer.mDiffuseTextureView.Get(),
        XMFLOAT4(0.20f, 0.18f, 0.12f, 1.0f),
        32.0f,
        0.0f
    };

    Material headMaterial =
    {
        XMFLOAT4(0.95f, 0.86f, 0.58f, 1.0f),
        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        mApp->mRenderer.mWhiteTextureView.Get(),
        XMFLOAT4(0.20f, 0.18f, 0.12f, 1.0f),
        32.0f,
        0.0f
    };

    Material frontMarkerMaterial =
    {
        XMFLOAT4(0.08f, 0.08f, 0.10f, 1.0f),
        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        mApp->mRenderer.mWhiteTextureView.Get(),
        XMFLOAT4(0.10f, 0.10f, 0.10f, 1.0f),
        16.0f,
        0.0f
    };

    DrawPlayerBody(
        playerWorld,
        viewProjection,
        bodyMaterial);

    DrawPlayerHead(
        playerWorld,
        viewProjection,
        headMaterial);

    DrawPlayerArms(
        playerWorld,
        viewProjection,
        bodyMaterial);

    DrawPlayerLegs(
        playerWorld,
        viewProjection,
        bodyMaterial);

    DrawPlayerFrontMarker(
        playerWorld,
        viewProjection,
        frontMarkerMaterial);
}

void WorldRenderer::DrawPlayerBody(
    const XMMATRIX& playerWorld,
    const XMMATRIX& viewProjection,
    const Material& bodyMaterial)
{
    const Player& player =
        mApp->mWorld.MainPlayer;

    float idleBreathOffset = 0.0f;
    float idleBreathScale = 1.0f;

    if (!player.IsMoving)
    {
        float breath =
            sinf(mApp->mWorld.SceneTime * 2.0f);

        idleBreathOffset = breath * 0.025f;
        idleBreathScale = 1.0f + breath * 0.025f;
    }

    XMMATRIX bodyWorld =
        XMMatrixScaling(
            0.45f,
            0.75f * idleBreathScale,
            0.28f) *
        XMMatrixTranslation(
            0.0f,
            0.95f + idleBreathOffset,
            0.0f) *
        playerWorld;

    mApp->mRenderer.DrawBox(
        bodyWorld,
        viewProjection,
        bodyMaterial);
}

void WorldRenderer::DrawPlayerHead(
    const XMMATRIX& playerWorld,
    const XMMATRIX& viewProjection,
    const Material& headMaterial)
{
    const Player& player =
        mApp->mWorld.MainPlayer;

    float idleBreathOffset = 0.0f;

    if (!player.IsMoving)
    {
        idleBreathOffset =
            sinf(mApp->mWorld.SceneTime * 2.0f) * 0.025f;
    }

    XMMATRIX headWorld =
        XMMatrixScaling(0.28f, 0.28f, 0.28f) *
        XMMatrixTranslation(
            0.0f,
            1.50f + idleBreathOffset,
            0.0f) *
        playerWorld;

    mApp->mRenderer.DrawSphere(
        headWorld,
        viewProjection,
        headMaterial);
}

void WorldRenderer::DrawPlayerArms(
    const XMMATRIX& playerWorld,
    const XMMATRIX& viewProjection,
    const Material& bodyMaterial)
{
    const Player& player =
        mApp->mWorld.MainPlayer;

    float armSwing = 0.0f;

    if (player.IsMoving)
    {
        armSwing =
            sinf(player.WalkCycleTime * 8.0f) * 0.45f;
    }

    XMMATRIX leftArmWorld =
        XMMatrixScaling(0.12f, 0.60f, 0.12f) *
        XMMatrixRotationX(armSwing) *
        XMMatrixTranslation(-0.36f, 0.95f, 0.0f) *
        playerWorld;

    mApp->mRenderer.DrawBox(
        leftArmWorld,
        viewProjection,
        bodyMaterial);

    XMMATRIX rightArmWorld =
        XMMatrixScaling(0.12f, 0.60f, 0.12f) *
        XMMatrixRotationX(-armSwing) *
        XMMatrixTranslation(0.36f, 0.95f, 0.0f) *
        playerWorld;

    mApp->mRenderer.DrawBox(
        rightArmWorld,
        viewProjection,
        bodyMaterial);
}

void WorldRenderer::DrawPlayerLegs(
    const XMMATRIX& playerWorld,
    const XMMATRIX& viewProjection,
    const Material& bodyMaterial)
{
    const Player& player =
        mApp->mWorld.MainPlayer;

    float legSwing = 0.0f;

    if (player.IsMoving)
    {
        legSwing =
            sinf(player.WalkCycleTime * 8.0f) * 0.35f;
    }

    XMMATRIX leftLegWorld =
        XMMatrixScaling(0.15f, 0.70f, 0.15f) *
        XMMatrixRotationX(-legSwing) *
        XMMatrixTranslation(-0.13f, 0.35f, 0.0f) *
        playerWorld;

    mApp->mRenderer.DrawBox(
        leftLegWorld,
        viewProjection,
        bodyMaterial);

    XMMATRIX rightLegWorld =
        XMMatrixScaling(0.15f, 0.70f, 0.15f) *
        XMMatrixRotationX(legSwing) *
        XMMatrixTranslation(0.13f, 0.35f, 0.0f) *
        playerWorld;

    mApp->mRenderer.DrawBox(
        rightLegWorld,
        viewProjection,
        bodyMaterial);
}

void WorldRenderer::DrawPlayerShadow(
    const XMMATRIX& viewProjection)
{
    const Player& player =
        mApp->mWorld.MainPlayer;

    float shadowY =
        player.TargetGroundY + 0.02f;

    float shadowScale = 1.0f;

    if (player.IsMoving)
    {
        shadowScale = 1.08f;
    }

    Material shadowMaterial =
    {
        XMFLOAT4(0.0f, 0.0f, 0.0f, 0.28f),
        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        mApp->mRenderer.mWhiteTextureView.Get(),
        XMFLOAT4(0.02f, 0.02f, 0.02f, 1.0f),
        8.0f,
        0.0f
    };

    XMMATRIX shadowWorld =
        XMMatrixScaling(
            0.70f * shadowScale,
            0.015f,
            0.55f * shadowScale) *
        XMMatrixTranslation(
            player.Position.x,
            shadowY,
            player.Position.z);

    mApp->mRenderer.SetAlphaBlendingEnabled(true);

    mApp->mRenderer.DrawBox(
        shadowWorld,
        viewProjection,
        shadowMaterial);

    mApp->mRenderer.SetAlphaBlendingEnabled(false);
}

void WorldRenderer::DrawPlayerFrontMarker(
    const XMMATRIX& playerWorld,
    const XMMATRIX& viewProjection,
    const Material& frontMarkerMaterial)
{
    const Player& player =
        mApp->mWorld.MainPlayer;

    float idleBreathOffset = 0.0f;

    if (!player.IsMoving)
    {
        idleBreathOffset =
            sinf(mApp->mWorld.SceneTime * 2.0f) * 0.025f;
    }

    XMMATRIX frontMarkerWorld =
        XMMatrixScaling(0.18f, 0.18f, 0.04f) *
        XMMatrixTranslation(
            0.0f,
            1.05f + idleBreathOffset,
            0.17f) *
        playerWorld;

    mApp->mRenderer.DrawBox(
        frontMarkerWorld,
        viewProjection,
        frontMarkerMaterial);
}

void WorldRenderer::DrawPlayerBoundsDebug(
    const XMMATRIX& viewProjection)
{
    if (!mApp->mWorld.ShowPlayerBoundsDebug)
    {
        return;
    }

    const PlayerMovementBounds& bounds =
        mApp->mWorld.MainPlayerBounds;

    const float minX = bounds.MinX;
    const float maxX = bounds.MaxX;
    const float minZ = bounds.MinZ;
    const float maxZ = bounds.MaxZ;

    const float centerX = (minX + maxX) * 0.5f;
    const float centerZ = (minZ + maxZ) * 0.5f;

    const float widthX = maxX - minX;
    const float widthZ = maxZ - minZ;

    const float wallThickness = 0.08f;
    const float wallHeight = 0.25f;
    const float wallY = 0.05f;

    Material debugMaterial =
    {
        XMFLOAT4(1.0f, 0.05f, 0.05f, 0.35f),
        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        mApp->mRenderer.mWhiteTextureView.Get(),
        XMFLOAT4(0.05f, 0.05f, 0.05f, 1.0f),
        8.0f,
        0.0f
    };

    mApp->mRenderer.SetAlphaBlendingEnabled(true);

    // Front boundary: MinZ side.
    XMMATRIX frontWall =
        XMMatrixScaling(widthX, wallHeight, wallThickness) *
        XMMatrixTranslation(centerX, wallY, minZ);

    mApp->mRenderer.DrawBox(
        frontWall,
        viewProjection,
        debugMaterial);

    // Back boundary: MaxZ side.
    XMMATRIX backWall =
        XMMatrixScaling(widthX, wallHeight, wallThickness) *
        XMMatrixTranslation(centerX, wallY, maxZ);

    mApp->mRenderer.DrawBox(
        backWall,
        viewProjection,
        debugMaterial);

    // Left boundary: MinX side.
    XMMATRIX leftWall =
        XMMatrixScaling(wallThickness, wallHeight, widthZ) *
        XMMatrixTranslation(minX, wallY, centerZ);

    mApp->mRenderer.DrawBox(
        leftWall,
        viewProjection,
        debugMaterial);

    // Right boundary: MaxX side.
    XMMATRIX rightWall =
        XMMatrixScaling(wallThickness, wallHeight, widthZ) *
        XMMatrixTranslation(maxX, wallY, centerZ);

    mApp->mRenderer.DrawBox(
        rightWall,
        viewProjection,
        debugMaterial);

    mApp->mRenderer.SetAlphaBlendingEnabled(false);
}