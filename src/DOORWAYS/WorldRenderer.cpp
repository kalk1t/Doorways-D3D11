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


namespace
{
    struct MountainPeakSpec
    {
        float Position;
        float Width;
        float Height;
        float Depth;
        float BaseYOffset;
    };


    enum class MountainSide
    {
        Front,
        Back,
        Left,
        Right
    };

    constexpr float kMountainBaseY = -0.65f;

    constexpr float kForegroundMountainDistance = 42.0f;
    constexpr float kShoulderMountainDistance = 46.0f;
    constexpr float kBackgroundMountainDistance = 58.0f;

        
    constexpr MountainPeakSpec kFrontPeaks[] =
    {
        { -34.0f, 18.0f, 15.0f, 8.0f, -0.4f },
        { -20.0f, 14.0f, 19.0f, 9.0f,  0.1f },
        {  -8.0f, 20.0f, 13.0f, 7.0f, -0.2f },
        {   8.0f, 16.0f, 21.0f, 9.5f, 0.0f },
        {  23.0f, 19.0f, 16.0f, 8.0f, -0.3f },
        {  37.0f, 13.0f, 18.0f, 7.5f, 0.2f },
    };

    const MountainPeakSpec kBackPeaks[] =
    {
        { -38.0f, 20.0f, 17.0f, 9.0f, -0.2f },
        { -22.0f, 15.0f, 22.0f, 8.0f,  0.1f },
        {  -5.0f, 23.0f, 14.0f, 9.5f, -0.4f },
        {  13.0f, 17.0f, 20.0f, 8.5f, 0.2f },
        {  31.0f, 21.0f, 16.0f, 9.0f, -0.1f },
    };

    const MountainPeakSpec kLeftPeaks[] =
    {
        { -36.0f, 18.0f, 16.0f, 8.0f, -0.3f },
        { -19.0f, 14.0f, 21.0f, 9.0f,  0.1f },
        {  -2.0f, 22.0f, 15.0f, 8.5f, -0.2f },
        {  17.0f, 16.0f, 19.0f, 8.0f,  0.0f },
        {  34.0f, 19.0f, 14.0f, 7.5f, -0.1f },
    };

    const MountainPeakSpec kRightPeaks[] =
    {
        { -33.0f, 16.0f, 20.0f, 8.5f,  0.0f },
        { -16.0f, 21.0f, 15.0f, 7.5f, -0.3f },
        {   0.0f, 15.0f, 23.0f, 9.5f,  0.1f },
        {  18.0f, 20.0f, 17.0f, 8.0f, -0.2f },
        {  36.0f, 14.0f, 18.0f, 7.0f,  0.2f },
    };


    const MountainPeakSpec kShoulderFrontPeaks[] =
    {
        { -27.0f, 10.0f, 10.0f, 6.0f, -0.6f },
        { -14.0f, 12.0f, 12.5f, 6.5f, -0.4f },
        {   1.0f,  9.0f, 11.0f, 5.5f, -0.5f },
        {  16.0f, 11.0f, 13.0f, 6.0f, -0.7f },
        {  30.0f, 10.0f, 10.5f, 5.5f, -0.4f },
    };

    const MountainPeakSpec kShoulderBackPeaks[] =
    {
        { -30.0f, 11.0f, 11.5f, 6.0f, -0.5f },
        { -12.0f,  9.0f, 13.0f, 5.5f, -0.7f },
        {   4.0f, 12.0f, 10.0f, 6.5f, -0.4f },
        {  21.0f, 10.0f, 12.0f, 5.5f, -0.6f },
    };

    const MountainPeakSpec kShoulderLeftPeaks[] =
    {
        { -28.0f, 10.0f, 11.0f, 6.0f, -0.5f },
        { -10.0f, 12.0f, 13.0f, 6.5f, -0.6f },
        {   8.0f,  9.0f, 10.5f, 5.5f, -0.4f },
        {  25.0f, 11.0f, 12.0f, 6.0f, -0.7f },
    };

    const MountainPeakSpec kShoulderRightPeaks[] =
    {
        { -27.0f, 11.0f, 12.5f, 6.0f, -0.6f },
        {  -9.0f,  9.0f, 10.0f, 5.5f, -0.5f },
        {   9.0f, 12.0f, 13.5f, 6.5f, -0.7f },
        {  27.0f, 10.0f, 11.0f, 5.5f, -0.4f },
    };




    const MountainPeakSpec kBackgroundFrontPeaks[] =
    {
        { -45.0f, 28.0f, 19.0f, 10.0f, -1.1f },
        { -23.0f, 24.0f, 25.0f, 11.0f, -0.8f },
        {   1.0f, 32.0f, 21.0f, 10.5f, -1.3f },
        {  26.0f, 23.0f, 27.0f, 11.0f, -0.9f },
        {  48.0f, 29.0f, 20.0f, 10.0f, -1.2f },
    };

    const MountainPeakSpec kBackgroundBackPeaks[] =
    {
        { -46.0f, 30.0f, 20.0f, 10.5f, -1.0f },
        { -21.0f, 24.0f, 26.0f, 11.0f, -0.9f },
        {   5.0f, 33.0f, 18.0f, 10.0f, -1.3f },
        {  30.0f, 26.0f, 24.0f, 11.0f, -1.0f },
        {  50.0f, 23.0f, 22.0f, 10.0f, -1.1f },
    };

    const MountainPeakSpec kBackgroundLeftPeaks[] =
    {
        { -46.0f, 27.0f, 20.0f, 10.0f, -1.1f },
        { -23.0f, 23.0f, 27.0f, 11.0f, -0.8f },
        {   2.0f, 31.0f, 21.0f, 10.5f, -1.2f },
        {  28.0f, 25.0f, 24.0f, 11.0f, -0.9f },
        {  49.0f, 26.0f, 19.0f, 10.0f, -1.0f },
    };

    const MountainPeakSpec kBackgroundRightPeaks[] =
    {
        { -48.0f, 25.0f, 23.0f, 10.0f, -1.0f },
        { -25.0f, 30.0f, 19.0f, 10.5f, -1.2f },
        {  -1.0f, 24.0f, 28.0f, 11.0f, -0.8f },
        {  25.0f, 32.0f, 21.0f, 10.0f, -1.1f },
        {  48.0f, 23.0f, 25.0f, 10.5f, -0.9f },
    };

}

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


void WorldRenderer::DrawPorchEnvironment(const XMMATRIX& viewProjection)
{
    DrawNightSky(viewProjection);
    DrawMoon(viewProjection);

    //DrawMountains(viewProjection);
    //DrawWaterfall(viewProjection);

    DrawPorchFloor(viewProjection);
    //DrawPorchFence(viewProjection);
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

void WorldRenderer::DrawPorchFloor(const XMMATRIX& viewProjection)
{
    Material gapMaterial =
    {
        XMFLOAT4(0.075f, 0.045f, 0.025f, 1.0f),
        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        mApp->mRenderer.mDiffuseTextureView.Get()
    };

    Material beamMaterial =
    {
        XMFLOAT4(0.24f, 0.13f, 0.055f, 1.0f),
        XMFLOAT4(2.0f, 1.0f, 0.0f, 0.0f),
        mApp->mRenderer.mDiffuseTextureView.Get()
    };

    // Dark base under the planks.
    // This makes the spaces between planks appear as dark gaps.
    XMMATRIX baseWorld = MakeWorld(
        5.15f,
        0.06f,
        4.15f,
        0.0f,
        -0.565f,
        0.0f);

    mApp->mRenderer.DrawBox(
        baseWorld,
        viewProjection,
        gapMaterial);

    // Individual wooden floor planks.
    constexpr int plankCount = 10;
    constexpr float plankWidth = 0.42f;
    constexpr float plankGap = 0.06f;
    constexpr float plankLength = 4.00f;
    constexpr float plankHeight = 0.08f;

    const float totalStep = plankWidth + plankGap;
    const float firstPlankX =
        -static_cast<float>(plankCount - 1) * totalStep * 0.5f;

    for (int i = 0; i < plankCount; ++i)
    {
        float x = firstPlankX + static_cast<float>(i) * totalStep;

        XMFLOAT4 plankColor;

        if ((i % 3) == 0)
        {
            plankColor = XMFLOAT4(0.46f, 0.29f, 0.13f, 1.0f);
        }
        else if ((i % 3) == 1)
        {
            plankColor = XMFLOAT4(0.55f, 0.34f, 0.16f, 1.0f);
        }
        else
        {
            plankColor = XMFLOAT4(0.38f, 0.23f, 0.10f, 1.0f);
        }

        Material plankMaterial =
        {
            plankColor,

            // Stretch texture along the plank length.
            XMFLOAT4(1.0f, 5.0f, 0.0f, 0.0f),

            mApp->mRenderer.mDiffuseTextureView.Get()
        };

        XMMATRIX plankWorld = MakeWorld(
            plankWidth,
            plankHeight,
            plankLength,
            x,
            -0.54f,
            0.0f);

        mApp->mRenderer.DrawBox(
            plankWorld,
            viewProjection,
            plankMaterial);
    }

    // Front porch beam.
    XMMATRIX frontBeamWorld = MakeWorld(
        5.25f,
        0.16f,
        0.18f,
        0.0f,
        -0.58f,
        -2.10f);

    mApp->mRenderer.DrawBox(
        frontBeamWorld,
        viewProjection,
        beamMaterial);

    // Back porch beam.
    XMMATRIX backBeamWorld = MakeWorld(
        5.25f,
        0.16f,
        0.18f,
        0.0f,
        -0.58f,
        2.10f);

    mApp->mRenderer.DrawBox(
        backBeamWorld,
        viewProjection,
        beamMaterial);

    // Left porch beam.
    XMMATRIX leftBeamWorld = MakeWorld(
        0.18f,
        0.16f,
        4.15f,
        -2.62f,
        -0.58f,
        0.0f);

    mApp->mRenderer.DrawBox(
        leftBeamWorld,
        viewProjection,
        beamMaterial);

    // Right porch beam.
    XMMATRIX rightBeamWorld = MakeWorld(
        0.18f,
        0.16f,
        4.15f,
        2.62f,
        -0.58f,
        0.0f);

    mApp->mRenderer.DrawBox(
        rightBeamWorld,
        viewProjection,
        beamMaterial);

    Material supportMaterial =
    {
        XMFLOAT4(0.18f, 0.085f, 0.035f, 1.0f),
        XMFLOAT4(2.0f, 1.0f, 0.0f, 0.0f),
        mApp->mRenderer.mDiffuseTextureView.Get()
    };

    // Under-floor joists.
    // These run left-to-right under the planks.
    const float joistZPositions[] =
    {
        -1.45f,
        0.0f,
        1.45f
    };

    for (float z : joistZPositions)
    {
        XMMATRIX joistWorld = MakeWorld(
            5.05f,
            0.12f,
            0.14f,
            0.0f,
            -0.72f,
            z);

        mApp->mRenderer.DrawBox(
            joistWorld,
            viewProjection,
            supportMaterial);
    }

    // Vertical support posts below the porch.
    struct PorchSupportPost
    {
        float X;
        float Z;
        float Height;
    };

    const PorchSupportPost posts[] =
    {
        // Corners
        { -2.25f, -1.75f, 0.95f },
        {  2.25f, -1.75f, 0.95f },
        { -2.25f,  1.75f, 0.95f },
        {  2.25f,  1.75f, 0.95f },

        // Side support posts
        { -2.25f,  0.00f, 0.85f },
        {  2.25f,  0.00f, 0.85f },

        // Front/back middle support posts
        {  0.00f, -1.75f, 0.85f },
        {  0.00f,  1.75f, 0.85f }
    };

    for (const PorchSupportPost& post : posts)
    {
        float postCenterY = -0.72f - post.Height * 0.5f;

        XMMATRIX postWorld = MakeWorld(
            0.18f,
            post.Height,
            0.18f,
            post.X,
            postCenterY,
            post.Z);

        mApp->mRenderer.DrawBox(
            postWorld,
            viewProjection,
            supportMaterial);
    }

    // Small foot blocks at the bottom of posts.
    Material stoneFootMaterial =
    {
        XMFLOAT4(0.18f, 0.18f, 0.20f, 1.0f),
        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        mApp->mRenderer.mDiffuseTextureView.Get()
    };

    for (const PorchSupportPost& post : posts)
    {
        XMMATRIX footWorld = MakeWorld(
            0.34f,
            0.12f,
            0.34f,
            post.X,
            -0.72f - post.Height - 0.06f,
            post.Z);

        mApp->mRenderer.DrawBox(
            footWorld,
            viewProjection,
            stoneFootMaterial);
    }

}

void WorldRenderer::DrawPorchFence(const XMMATRIX& viewProjection)
{
    Material postMaterial =
    {
        XMFLOAT4(0.42f, 0.22f, 0.085f, 1.0f),
        XMFLOAT4(1.5f, 1.5f, 0.0f, 0.0f),
        mApp->mRenderer.mDiffuseTextureView.Get()
    };

    Material railMaterial =
    {
        XMFLOAT4(0.52f, 0.31f, 0.13f, 1.0f),
        XMFLOAT4(2.0f, 1.0f, 0.0f, 0.0f),
        mApp->mRenderer.mDiffuseTextureView.Get()
    };

    Material balusterMaterial =
    {
        XMFLOAT4(0.36f, 0.19f, 0.075f, 1.0f),
        XMFLOAT4(1.0f, 2.0f, 0.0f, 0.0f),
        mApp->mRenderer.mDiffuseTextureView.Get()
    };

    constexpr float porchHalfX = 2.62f;
    constexpr float porchHalfZ = 2.10f;

    constexpr float postWidth = 0.18f;
    constexpr float postHeight = 1.15f;
    constexpr float postY = 0.08f;

    constexpr float topRailY = 0.55f;
    constexpr float middleRailY = 0.12f;

    constexpr float railThickness = 0.12f;
    constexpr float railHeight = 0.12f;

    constexpr float balusterWidth = 0.075f;
    constexpr float balusterHeight = 0.62f;
    constexpr float balusterY = 0.12f;

    auto DrawPost =
        [this, &viewProjection, &postMaterial](
            float x,
            float z)
        {
            XMMATRIX postWorld = MakeWorld(
                0.18f,
                1.15f,
                0.18f,
                x,
                0.08f,
                z);

            mApp->mRenderer.DrawBox(
                postWorld,
                viewProjection,
                postMaterial);
        };

    auto DrawCap =
        [this, &viewProjection, &railMaterial](
            float x,
            float z)
        {
            XMMATRIX capWorld = MakeWorld(
                0.28f,
                0.12f,
                0.28f,
                x,
                0.72f,
                z);

            mApp->mRenderer.DrawBox(
                capWorld,
                viewProjection,
                railMaterial);
        };

    auto DrawHorizontalRailX =
        [this, &viewProjection, &railMaterial](
            float z,
            float y)
        {
            XMMATRIX railWorld = MakeWorld(
                5.05f,
                0.12f,
                0.12f,
                0.0f,
                y,
                z);

            mApp->mRenderer.DrawBox(
                railWorld,
                viewProjection,
                railMaterial);
        };

    auto DrawHorizontalRailZ =
        [this, &viewProjection, &railMaterial](
            float x,
            float y)
        {
            XMMATRIX railWorld = MakeWorld(
                0.12f,
                0.12f,
                4.00f,
                x,
                y,
                0.0f);

            mApp->mRenderer.DrawBox(
                railWorld,
                viewProjection,
                railMaterial);
        };

    auto DrawBaluster =
        [this, &viewProjection, &balusterMaterial](
            float x,
            float z)
        {
            XMMATRIX balusterWorld = MakeWorld(
                0.075f,
                0.62f,
                0.075f,
                x,
                0.12f,
                z);

            mApp->mRenderer.DrawBox(
                balusterWorld,
                viewProjection,
                balusterMaterial);
        };

    // Corner posts.
    DrawPost(-porchHalfX, -porchHalfZ);
    DrawPost(porchHalfX, -porchHalfZ);
    DrawPost(-porchHalfX, porchHalfZ);
    DrawPost(porchHalfX, porchHalfZ);

    // Extra side posts.
    DrawPost(0.0f, -porchHalfZ);
    DrawPost(0.0f, porchHalfZ);
    DrawPost(-porchHalfX, 0.0f);
    DrawPost(porchHalfX, 0.0f);

    // Post caps.
    DrawCap(-porchHalfX, -porchHalfZ);
    DrawCap(porchHalfX, -porchHalfZ);
    DrawCap(-porchHalfX, porchHalfZ);
    DrawCap(porchHalfX, porchHalfZ);
    DrawCap(0.0f, -porchHalfZ);
    DrawCap(0.0f, porchHalfZ);
    DrawCap(-porchHalfX, 0.0f);
    DrawCap(porchHalfX, 0.0f);

    // Front and back horizontal rails.
    DrawHorizontalRailX(-porchHalfZ, topRailY);
    DrawHorizontalRailX(-porchHalfZ, middleRailY);

    DrawHorizontalRailX(porchHalfZ, topRailY);
    DrawHorizontalRailX(porchHalfZ, middleRailY);

    // Left and right horizontal rails.
    DrawHorizontalRailZ(-porchHalfX, topRailY);
    DrawHorizontalRailZ(-porchHalfX, middleRailY);

    DrawHorizontalRailZ(porchHalfX, topRailY);
    DrawHorizontalRailZ(porchHalfX, middleRailY);

    // Front and back balusters.
    const float balusterXPositions[] =
    {
        -2.05f,
        -1.55f,
        -1.05f,
        -0.55f,
         0.55f,
         1.05f,
         1.55f,
         2.05f
    };

    for (float x : balusterXPositions)
    {
        DrawBaluster(x, -porchHalfZ);
        DrawBaluster(x, porchHalfZ);
    }

    // Left and right balusters.
    const float balusterZPositions[] =
    {
        -1.55f,
        -1.05f,
        -0.55f,
         0.55f,
         1.05f,
         1.55f
    };

    for (float z : balusterZPositions)
    {
        DrawBaluster(-porchHalfX, z);
        DrawBaluster(porchHalfX, z);
    }
}

void WorldRenderer::DrawMountains(const XMMATRIX& viewProjection)
{

    Material mainRockMaterial =
    {
        XMFLOAT4(0.58f, 0.58f, 0.64f, 1.0f),
        XMFLOAT4(3.0f, 3.0f, 0.0f, 0.0f),
        mApp->mRenderer.mMountainSRV.Get(),
        1.0f
    };

    Material shadowRockMaterial =
    {
        XMFLOAT4(0.34f, 0.36f, 0.44f, 1.0f),
        XMFLOAT4(3.0f, 3.0f, 0.0f, 0.0f),
        mApp->mRenderer.mMountainSRV.Get(),
        1.0f
    };

    Material baseRockMaterial =
    {
        XMFLOAT4(0.26f, 0.28f, 0.34f, 1.0f),
        XMFLOAT4(4.0f, 1.5f, 0.0f, 0.0f),
        mApp->mRenderer.mMountainSRV.Get(),
        1.0f
    };

    Material snowCapMaterial =
    {
        XMFLOAT4(0.76f, 0.82f, 0.94f, 1.0f),
        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        mApp->mRenderer.mWhiteTextureView.Get(),
        1.0f
    };

    auto MakeRepeatedMaterial =
        [](const Material& baseMaterial,
            float width,
            float height)
        {
            Material result = baseMaterial;

            float uRepeat = width / 8.0f;
            float vRepeat = height / 8.0f;

            if (uRepeat < 1.0f)
            {
                uRepeat = 1.0f;
            }

            if (vRepeat < 1.0f)
            {
                vRepeat = 1.0f;
            }

            result.TexTransform = XMFLOAT4(
                uRepeat,
                vRepeat,
                0.0f,
                0.0f);

            return result;
        };

    auto DrawPeak =
        [&](float x,
            float z,
            float width,
            float height,
            float depth,
            float baseY,
            float rotationY,
            const Material& material)
        {
            float centerY = baseY + height * 0.5f;

            XMMATRIX world =
                XMMatrixScaling(width, height, depth) *
                XMMatrixRotationY(rotationY) *
                XMMatrixTranslation(x, centerY, z);

            Material repeatedMaterial =
                MakeRepeatedMaterial(
                    material,
                    width,
                    height);

            mApp->mRenderer.DrawMountainPeak(
                world,
                viewProjection,
                repeatedMaterial);
        };

    auto DrawSnowCap =
        [&](float x,
            float z,
            float width,
            float height,
            float depth,
            float baseY,
            float rotationY)
        {
            // Main snow cap near the peak.
            {
                float capHeight = height * 0.22f;
                float capWidth = width * 0.34f;
                float capDepth = depth * 1.05f;

                float capBaseY = baseY + height - capHeight;
                float capCenterY = capBaseY + capHeight * 0.5f;

                XMMATRIX world =
                    XMMatrixScaling(capWidth, capHeight, capDepth) *
                    XMMatrixRotationY(rotationY) *
                    XMMatrixTranslation(x, capCenterY, z);

                mApp->mRenderer.DrawMountainPeak(
                    world,
                    viewProjection,
                    snowCapMaterial);
            }

            // Left snow streak running slightly down the mountain face.
            {
                float patchHeight = height * 0.16f;
                float patchWidth = width * 0.16f;
                float patchDepth = depth * 1.07f;

                float patchBaseY = baseY + height * 0.62f;
                float patchCenterY = patchBaseY + patchHeight * 0.5f;

                XMMATRIX world =
                    XMMatrixScaling(patchWidth, patchHeight, patchDepth) *
                    XMMatrixRotationY(rotationY - 0.10f) *
                    XMMatrixTranslation(x - width * 0.12f, patchCenterY, z - 0.15f);

                mApp->mRenderer.DrawMountainPeak(
                    world,
                    viewProjection,
                    snowCapMaterial);
            }

            // Smaller right snow patch.
            {
                float patchHeight = height * 0.12f;
                float patchWidth = width * 0.12f;
                float patchDepth = depth * 1.06f;

                float patchBaseY = baseY + height * 0.54f;
                float patchCenterY = patchBaseY + patchHeight * 0.5f;

                XMMATRIX world =
                    XMMatrixScaling(patchWidth, patchHeight, patchDepth) *
                    XMMatrixRotationY(rotationY + 0.12f) *
                    XMMatrixTranslation(x + width * 0.14f, patchCenterY, z - 0.20f);

                mApp->mRenderer.DrawMountainPeak(
                    world,
                    viewProjection,
                    snowCapMaterial);
            }
        };


    constexpr float mountainZ = 42.0f;
    constexpr float mountainBaseY = -0.80f;
    constexpr float mountainRotationY = 0.0f;

    // Rear dark mass behind the main peak.
// This makes the mountain feel thicker instead of like a flat triangle.
    DrawPeak(
        -2.0f,
        mountainZ + 2.0f,
        24.0f,
        19.0f,
        13.0f,
        mountainBaseY - 0.6f,
        mountainRotationY,
        shadowRockMaterial);

    // Main tall center peak.
    DrawPeak(
        0.0f,
        mountainZ,
        22.0f,
        24.0f,
        11.0f,
        mountainBaseY,
        mountainRotationY,
        mainRockMaterial);

    // Dark left shoulder.
    // Slight rotation breaks the artificial symmetry.
    DrawPeak(
        -9.5f,
        mountainZ - 1.5f,
        15.0f,
        15.0f,
        9.0f,
        mountainBaseY - 0.3f,
        -0.10f,
        shadowRockMaterial);

    // Dark right shoulder.
    // Slight opposite rotation gives the silhouette more natural variation.
    DrawPeak(
        10.5f,
        mountainZ - 0.5f,
        17.0f,
        17.0f,
        9.5f,
        mountainBaseY - 0.2f,
        0.12f,
        shadowRockMaterial);

    // Small front-left rocky ridge.
    DrawPeak(
        -5.0f,
        mountainZ - 4.0f,
        9.0f,
        10.0f,
        6.5f,
        mountainBaseY - 0.5f,
        -0.18f,
        baseRockMaterial);

    // Small front-right rocky ridge.
    DrawPeak(
        6.0f,
        mountainZ - 3.5f,
        10.0f,
        11.0f,
        6.5f,
        mountainBaseY - 0.45f,
        0.16f,
        baseRockMaterial);

    // Low front base ridge.
    // This hides the clean triangle bottoms and connects the pieces.
    XMMATRIX baseWorld =
        XMMatrixScaling(36.0f, 5.0f, 8.0f) *
        XMMatrixTranslation(0.0f, mountainBaseY + 2.5f, mountainZ - 2.5f);

    mApp->mRenderer.DrawBox(
        baseWorld,
        viewProjection,
        baseRockMaterial);

    // Snow on main peak only.
    DrawSnowCap(
        0.0f,
        mountainZ,
        22.0f,
        24.0f,
        11.0f,
        mountainBaseY,
        mountainRotationY);


    /*
    Material backgroundMountainMaterial =
    {
        XMFLOAT4(0.34f, 0.36f, 0.44f, 1.0f),
        XMFLOAT4(3.5f, 3.5f, 0.0f, 0.0f),
        mApp->mRenderer.mMountainSRV.Get(),
        1.0f
    };

    Material shoulderMountainMaterial =
    {
        XMFLOAT4(0.42f, 0.44f, 0.52f, 1.0f),
        XMFLOAT4(3.0f, 3.0f, 0.0f, 0.0f),
        mApp->mRenderer.mMountainSRV.Get(),
        1.0f
    };

    Material farMountainMaterial =
    {
        XMFLOAT4(0.46f, 0.48f, 0.56f, 1.0f),
        XMFLOAT4(3.0f, 3.0f, 0.0f, 0.0f),
        mApp->mRenderer.mMountainSRV.Get(),
        1.0f
    };

    Material midMountainMaterial =
    {
        XMFLOAT4(0.55f, 0.56f, 0.62f, 1.0f),
        XMFLOAT4(3.0f, 3.0f, 0.0f, 0.0f),
        mApp->mRenderer.mMountainSRV.Get(),
        1.0f
    };

    Material foothillMaterial =
    {
        XMFLOAT4(0.28f, 0.30f, 0.36f, 1.0f),
        XMFLOAT4(4.0f, 1.5f, 0.0f, 0.0f),
        mApp->mRenderer.mMountainSRV.Get(),
        1.0f
    };

    Material snowCapMaterial =
    {
        XMFLOAT4(0.70f, 0.76f, 0.88f, 1.0f),
        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        mApp->mRenderer.mWhiteTextureView.Get(),
        1.0f
    };

    Material mountainMistMaterial =
    {
        XMFLOAT4(0.18f, 0.22f, 0.34f, 0.18f),
        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        mApp->mRenderer.mWhiteTextureView.Get(),
        1.0f
    };

    auto DrawFrontBackMistBand =
        [&](float z,
            float centerY,
            float width,
            float height,
            float alpha)
        {
            Material mistMaterial =
            {
                XMFLOAT4(0.18f, 0.22f, 0.34f, alpha),
                XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
                mApp->mRenderer.mWhiteTextureView.Get(),
                1.0f
            };

            XMMATRIX mistWorld =
                XMMatrixScaling(width, height, 0.08f) *
                XMMatrixTranslation(0.0f, centerY, z);

            mApp->mRenderer.DrawBox(
                mistWorld,
                viewProjection,
                mistMaterial);
        };

    auto DrawLeftRightMistBand =
        [&](float x,
            float centerY,
            float depth,
            float height,
            float alpha)
        {
            Material mistMaterial =
            {
                XMFLOAT4(0.18f, 0.22f, 0.34f, alpha),
                XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
                mApp->mRenderer.mWhiteTextureView.Get(),
                1.0f
            };

            XMMATRIX mistWorld =
                XMMatrixScaling(0.08f, height, depth) *
                XMMatrixTranslation(x, centerY, 0.0f);

            mApp->mRenderer.DrawBox(
                mistWorld,
                viewProjection,
                mistMaterial);
        };


    auto DrawFrontBackFoothill =
        [&](float z,
            const Material& material)
        {
            constexpr float baseY = -0.70f;

            XMMATRIX foothillWorld =
                XMMatrixScaling(95.0f, 4.0f, 5.5f) *
                XMMatrixTranslation(0.0f, baseY + 2.0f, z);

            mApp->mRenderer.DrawBox(
                foothillWorld,
                viewProjection,
                material);
        };
    auto DrawLeftRightFoothill =
        [&](float x,
            const Material& material)
        {
            constexpr float baseY = -0.70f;

            XMMATRIX foothillWorld =
                XMMatrixScaling(5.5f, 4.0f, 95.0f) *
                XMMatrixTranslation(x, baseY + 2.0f, 0.0f);

            mApp->mRenderer.DrawBox(
                foothillWorld,
                viewProjection,
                material);
        };


    auto MakeMountainMaterial =
        [](const Material& baseMaterial,
            float width,
            float height)
        {
            Material result = baseMaterial;

            float uRepeat = width / 8.0f;
            float vRepeat = height / 8.0f;

            if (uRepeat < 1.0f)
            {
                uRepeat = 1.0f;
            }

            if (vRepeat < 1.0f)
            {
                vRepeat = 1.0f;
            }

            result.TexTransform = XMFLOAT4(
                uRepeat,
                vRepeat,
                0.0f,
                0.0f);

            return result;
        };

    auto DrawMountainSnowCap =
        [&](float x,
            float z,
            float width,
            float height,
            float depth,
            float baseYOffset,
            float rotationY)
        {

            float adjustedBaseY = kMountainBaseY + baseYOffset;

            float capHeight = height * 0.26f;
            float capWidth = width * 0.30f;
            float capDepth = depth * 1.04f;

            float capBaseY = adjustedBaseY + height - capHeight;
            float capCenterY = capBaseY + capHeight * 0.5f;

            XMMATRIX snowCapWorld =
                XMMatrixScaling(capWidth, capHeight, capDepth) *
                XMMatrixRotationY(rotationY) *
                XMMatrixTranslation(x, capCenterY, z);

            mApp->mRenderer.DrawMountainPeak(
                snowCapWorld,
                viewProjection,
                snowCapMaterial);
        };

    auto DrawMountainInstance =
        [&](MountainSide side,
            float fixedCoordinate,
            const MountainPeakSpec& peak,
            const Material& mountainMaterial,
            bool drawSnowCap)
        {
            float x = 0.0f;
            float z = 0.0f;
            float rotationY = 0.0f;

            switch (side)
            {
            case MountainSide::Front:
                x = peak.Position;
                z = -fixedCoordinate;
                rotationY = 0.0f;
                break;

            case MountainSide::Back:
                x = peak.Position;
                z = fixedCoordinate;
                rotationY = 0.0f;
                break;

            case MountainSide::Left:
                x = -fixedCoordinate;
                z = peak.Position;
                rotationY = XM_PIDIV2;
                break;

            case MountainSide::Right:
                x = fixedCoordinate;
                z = peak.Position;
                rotationY = -XM_PIDIV2;
                break;
            }

            float adjustedBaseY = kMountainBaseY + peak.BaseYOffset;
            float centerY = adjustedBaseY + peak.Height * 0.5f;

            XMMATRIX mountainWorld =
                XMMatrixScaling(peak.Width, peak.Height, peak.Depth) *
                XMMatrixRotationY(rotationY) *
                XMMatrixTranslation(x, centerY, z);

            Material repeatedMountainMaterial =
                MakeMountainMaterial(
                    mountainMaterial,
                    peak.Width,
                    peak.Height);

            mApp->mRenderer.DrawMountainPeak(
                mountainWorld,
                viewProjection,
                repeatedMountainMaterial);

            if (drawSnowCap)
            {
                DrawMountainSnowCap(
                    x,
                    z,
                    peak.Width,
                    peak.Height,
                    peak.Depth,
                    peak.BaseYOffset,
                    rotationY);
            }
        };

    for (const MountainPeakSpec& peak : kBackgroundFrontPeaks)
    {
        DrawMountainInstance(
            MountainSide::Front,
            kBackgroundMountainDistance,
            peak,
            backgroundMountainMaterial,
            false);
    }

    for (const MountainPeakSpec& peak : kBackgroundBackPeaks)
    {
        DrawMountainInstance(
            MountainSide::Back,
            kBackgroundMountainDistance,
            peak,
            backgroundMountainMaterial,
            false);
    }

    for (const MountainPeakSpec& peak : kBackgroundLeftPeaks)
    {
        DrawMountainInstance(
            MountainSide::Left,
            kBackgroundMountainDistance,
            peak,
            backgroundMountainMaterial,
            false);
    }

    for (const MountainPeakSpec& peak : kBackgroundRightPeaks)
    {
        DrawMountainInstance(
            MountainSide::Right,
            kBackgroundMountainDistance,
            peak,
            backgroundMountainMaterial,
            false);
    }


    // ============================================================
    // Shoulder / secondary mountain layer
    // ============================================================

    for (const MountainPeakSpec& peak : kShoulderFrontPeaks)
    {
        DrawMountainInstance(
            MountainSide::Front,
            kShoulderMountainDistance,
            peak,
            shoulderMountainMaterial,
            false);
    }

    for (const MountainPeakSpec& peak : kShoulderBackPeaks)
    {
        DrawMountainInstance(
            MountainSide::Back,
            kShoulderMountainDistance,
            peak,
            shoulderMountainMaterial,
            false);
    }

    for (const MountainPeakSpec& peak : kShoulderLeftPeaks)
    {
        DrawMountainInstance(
            MountainSide::Left,
            kShoulderMountainDistance,
            peak,
            shoulderMountainMaterial,
            false);
    }

    for (const MountainPeakSpec& peak : kShoulderRightPeaks)
    {
        DrawMountainInstance(
            MountainSide::Right,
            kShoulderMountainDistance,
            peak,
            shoulderMountainMaterial,
            false);
    }

    DrawFrontBackFoothill(
        -kForegroundMountainDistance + 1.5f,
        foothillMaterial);

    DrawFrontBackFoothill(
        kForegroundMountainDistance - 1.5f,
        foothillMaterial);

    DrawLeftRightFoothill(
        -kForegroundMountainDistance + 1.5f,
        foothillMaterial);

    DrawLeftRightFoothill(
        kForegroundMountainDistance - 1.5f,
        foothillMaterial);
    // ============================================================
    // Foreground main mountain layer
    // ============================================================

    for (const MountainPeakSpec& peak : kFrontPeaks)
    {
        DrawMountainInstance(
            MountainSide::Front,
            kForegroundMountainDistance,
            peak,
            midMountainMaterial,
            true);
    }

    for (const MountainPeakSpec& peak : kBackPeaks)
    {
        DrawMountainInstance(
            MountainSide::Back,
            kForegroundMountainDistance,
            peak,
            farMountainMaterial,
            true);
    }

    for (const MountainPeakSpec& peak : kLeftPeaks)
    {
        DrawMountainInstance(
            MountainSide::Left,
            kForegroundMountainDistance,
            peak,
            farMountainMaterial,
            true);
    }

    for (const MountainPeakSpec& peak : kRightPeaks)
    {
        DrawMountainInstance(
            MountainSide::Right,
            kForegroundMountainDistance,
            peak,
            farMountainMaterial,
            true);
    }
    mApp->mRenderer.SetAlphaBlendingEnabled(true);

    // Mist between foreground and background mountain layers.
    DrawFrontBackMistBand(
        -kShoulderMountainDistance,
        2.2f,
        105.0f,
        5.0f,
        0.18f);

    DrawFrontBackMistBand(
        kShoulderMountainDistance,
        2.2f,
        105.0f,
        5.0f,
        0.18f);

    DrawLeftRightMistBand(
        -kShoulderMountainDistance,
        2.2f,
        105.0f,
        5.0f,
        0.16f);

    DrawLeftRightMistBand(
        kShoulderMountainDistance,
        2.2f,
        105.0f,
        5.0f,
        0.16f);

    // Lower heavier mist near the mountain base.
    DrawFrontBackMistBand(
        -kForegroundMountainDistance + 2.0f,
        0.7f,
        100.0f,
        2.0f,
        0.14f);

    DrawFrontBackMistBand(
        kForegroundMountainDistance - 2.0f,
        0.7f,
        100.0f,
        2.0f,
        0.14f);

    DrawLeftRightMistBand(
        -kForegroundMountainDistance + 2.0f,
        0.7f,
        100.0f,
        2.0f,
        0.12f);

    DrawLeftRightMistBand(
        kForegroundMountainDistance - 2.0f,
        0.7f,
        100.0f,
        2.0f,
        0.12f);

    mApp->mRenderer.SetAlphaBlendingEnabled(false);
    */
}

void WorldRenderer::DrawWaterfall(const XMMATRIX& viewProjection)
{
    float waterPulse =
        0.5f + 0.5f * sinf(mApp->mWorld.EnvironmentTime * 2.2f);

    float mistDrift =
        sinf(mApp->mWorld.EnvironmentTime * 0.7f) * 0.18f;

    Material cliffMaterial =
    {
        XMFLOAT4(0.030f, 0.035f, 0.060f, 1.0f),
        XMFLOAT4(2.0f, 2.0f, 0.0f, 0.0f),
        mApp->mRenderer.mDiffuseTextureView.Get()
    };

    Material darkRockMaterial =
    {
        XMFLOAT4(0.045f, 0.050f, 0.075f, 1.0f),
        XMFLOAT4(2.0f, 2.0f, 0.0f, 0.0f),
        mApp->mRenderer.mDiffuseTextureView.Get()
    };

    Material waterMaterial =
    {
        XMFLOAT4(
            0.25f + waterPulse * 0.06f,
            0.48f + waterPulse * 0.10f,
            0.85f + waterPulse * 0.15f,
            1.0f),

        XMFLOAT4(1.0f, 3.0f, 0.0f, 0.0f),

        mApp->mRenderer.mDiffuseTextureView.Get()
    };

    Material waterHighlightMaterial =
    {
        XMFLOAT4(
            0.70f + waterPulse * 0.15f,
            0.88f + waterPulse * 0.12f,
            1.45f + waterPulse * 0.25f,
            1.0f),

        XMFLOAT4(1.0f, 2.0f, 0.0f, 0.0f),

        mApp->mRenderer.mDiffuseTextureView.Get()
    };

    Material mistMaterial =
    {
        XMFLOAT4(
            0.35f,
            0.48f + waterPulse * 0.05f,
            0.68f + waterPulse * 0.08f,
            1.0f),

        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),

        mApp->mRenderer.mDiffuseTextureView.Get()
    };

    Material poolMaterial =
    {
        XMFLOAT4(
            0.08f,
            0.18f + waterPulse * 0.05f,
            0.32f + waterPulse * 0.08f,
            1.0f),

        XMFLOAT4(2.0f, 1.0f, 0.0f, 0.0f),

        mApp->mRenderer.mDiffuseTextureView.Get()
    };

    // Position waterfall in the front mountain direction.
    constexpr float waterfallZ = 13.45f;

    // Dark cliff wall behind the waterfall.
    XMMATRIX cliffBackWorld = MakeWorld(
        4.20f,
        6.40f,
        0.34f,
        0.0f,
        2.35f,
        waterfallZ + 0.10f);

    mApp->mRenderer.DrawBox(
        cliffBackWorld,
        viewProjection,
        cliffMaterial);

    // Left cliff side.
    XMMATRIX leftCliffWorld = MakeWorld(
        1.05f,
        6.70f,
        0.42f,
        -2.20f,
        2.35f,
        waterfallZ);

    mApp->mRenderer.DrawBox(
        leftCliffWorld,
        viewProjection,
        darkRockMaterial);

    // Right cliff side.
    XMMATRIX rightCliffWorld = MakeWorld(
        1.05f,
        6.70f,
        0.42f,
        2.20f,
        2.35f,
        waterfallZ);

    mApp->mRenderer.DrawBox(
        rightCliffWorld,
        viewProjection,
        darkRockMaterial);

    // Top cliff cap.
    XMMATRIX topCliffWorld = MakeWorld(
        4.50f,
        0.70f,
        0.45f,
        0.0f,
        5.75f,
        waterfallZ);

    mApp->mRenderer.DrawBox(
        topCliffWorld,
        viewProjection,
        darkRockMaterial);

    // Main waterfall body.
    XMMATRIX waterfallWorld = MakeWorld(
        1.05f,
        5.30f,
        0.16f,
        0.0f,
        2.35f,
        waterfallZ - 0.25f);

    mApp->mRenderer.DrawBox(
        waterfallWorld,
        viewProjection,
        waterMaterial);

    // Animated falling highlight strips.
    auto GetFallingY =
        [this](float startY, float speed) -> float
        {
            constexpr float minY = 0.05f;
            constexpr float maxY = 4.90f;
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

    struct WaterStreak
    {
        float X;
        float StartY;
        float Speed;
        float Height;
        float Width;
    };

    const WaterStreak streaks[] =
    {
        { -0.34f, 4.40f, 1.35f, 1.20f, 0.10f },
        { -0.10f, 3.50f, 1.75f, 1.50f, 0.08f },
        {  0.18f, 4.80f, 1.55f, 1.10f, 0.09f },
        {  0.38f, 3.90f, 1.25f, 1.35f, 0.07f }
    };

    for (const WaterStreak& streak : streaks)
    {
        float y = GetFallingY(streak.StartY, streak.Speed);

        XMMATRIX streakWorld = MakeWorld(
            streak.Width,
            streak.Height,
            0.18f,
            streak.X,
            y,
            waterfallZ - 0.34f);

        mApp->mRenderer.DrawBox(
            streakWorld,
            viewProjection,
            waterHighlightMaterial);
    }

    // Mist clouds at the bottom of the waterfall.
    XMMATRIX mistMainWorld = MakeWorld(
        2.00f,
        0.35f,
        0.20f,
        mistDrift,
        -0.05f,
        waterfallZ - 0.45f);

    mApp->mRenderer.DrawBox(
        mistMainWorld,
        viewProjection,
        mistMaterial);

    XMMATRIX mistLeftWorld = MakeWorld(
        1.10f,
        0.25f,
        0.18f,
        -0.65f + mistDrift * 0.5f,
        0.08f,
        waterfallZ - 0.50f);

    mApp->mRenderer.DrawBox(
        mistLeftWorld,
        viewProjection,
        mistMaterial);

    XMMATRIX mistRightWorld = MakeWorld(
        1.10f,
        0.25f,
        0.18f,
        0.65f + mistDrift * 0.5f,
        0.02f,
        waterfallZ - 0.50f);

    mApp->mRenderer.DrawBox(
        mistRightWorld,
        viewProjection,
        mistMaterial);

    // Small pool/river reflection below the waterfall.
    XMMATRIX poolWorld = MakeWorld(
        2.70f,
        0.08f,
        1.05f,
        0.0f,
        -0.50f,
        waterfallZ - 1.10f);

    mApp->mRenderer.DrawBox(
        poolWorld,
        viewProjection,
        poolMaterial);
}





void WorldRenderer::DrawScene(const XMMATRIX& viewProjection)
{

    DrawPorchEnvironment(viewProjection);


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