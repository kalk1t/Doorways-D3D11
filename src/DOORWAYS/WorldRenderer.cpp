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

WorldRenderer::WorldRenderer(App* app)
    : mApp(app)
{
}






void WorldRenderer::DrawScene(const XMMATRIX& viewProjection)
{

    DrawNightSky(viewProjection);
    DrawMoon(viewProjection);

    DrawImportedScene(viewProjection);


    //DrawPlayer(viewProjection);


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
        1.0f
    };

    Material backSkyMaterial =
    {
        skyTint,
        XMFLOAT4(1.0f, 1.0f, 0.35f, 0.10f),
        mApp->mRenderer.mStarSkySRV.Get(),
        1.0f
    };

    Material leftSkyMaterial =
    {
        skyTint,
        XMFLOAT4(1.0f, 1.0f, 0.70f, 0.20f),
        mApp->mRenderer.mStarSkySRV.Get(),
        1.0f
    };

    Material rightSkyMaterial =
    {
        skyTint,
        XMFLOAT4(1.0f, 1.0f, 0.15f, 0.35f),
        mApp->mRenderer.mStarSkySRV.Get(),
        1.0f
    };

    Material topSkyMaterial =
    {
        skyTint,
        XMFLOAT4(1.25f, 1.25f, 0.25f, 0.15f),
        mApp->mRenderer.mStarSkySRV.Get(),
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
        XMFLOAT4(0.72f, 0.68f, 0.58f, 1.0f),
        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        mApp->mRenderer.mPrimarySceneSRV
                ? mApp->mRenderer.mPrimarySceneSRV.Get()
                : mApp->mRenderer.mWhiteTextureView.Get(),
        0.0f
    };

    XMMATRIX sceneWorld =
        XMMatrixScaling(1.0f, 1.0f, 1.0f) *
        XMMatrixTranslation(0.0f, 0.0f, 0.0f);

    mApp->mRenderer.DrawMesh(
        mApp->mRenderer.mPrimarySceneMesh,
        sceneWorld,
        viewProjection,
        sceneMaterial);
}










void WorldRenderer::DrawPlayer(const XMMATRIX& viewProjection)
{
    mApp->mWorld.MainPlayer.Position.y = 0.3f;
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
