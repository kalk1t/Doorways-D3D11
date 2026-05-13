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
    template <typename TRenderer>
    void DrawEnvironmentBox(
        TRenderer& renderer,
        const XMMATRIX& viewProjection,
        const Material& material,
        float width,
        float height,
        float depth,
        float x,
        float y,
        float z)
    {
       XMMATRIX world =
            XMMatrixScaling(width, height, depth) *
            XMMatrixTranslation(x, y, z);

        renderer.DrawBox(
            world,
            viewProjection,
            material);
    }

    Material MakeEnvironmentMaterial(
        XMFLOAT4 diffuse,
        float tileX,
        float tileY,
        ID3D11ShaderResourceView* diffuseMap)
    {
        return Material
        {
            diffuse,
            XMFLOAT4(tileX, tileY, 0.0f, 0.0f),
            diffuseMap,
            1.0f
        };
    }

    Material MakePorchFoundationMaterial(ID3D11ShaderResourceView* texture)
    {
        return MakeEnvironmentMaterial(
            XMFLOAT4(0.30f, 0.26f, 0.22f, 1.0f),
            5.0f,
            3.0f,
            texture);
    }

    Material MakePorchFloorMaterial(ID3D11ShaderResourceView* texture)
    {
        return MakeEnvironmentMaterial(
            XMFLOAT4(0.48f, 0.37f, 0.25f, 1.0f),
            8.0f,
            6.0f,
            texture);
    }

    Material MakePorchBorderMaterial(ID3D11ShaderResourceView* texture)
    {
        return MakeEnvironmentMaterial(
            XMFLOAT4(0.25f, 0.22f, 0.19f, 1.0f),
            4.0f,
            2.0f,
            texture);
    }

    Material MakePorchTrimMaterial(ID3D11ShaderResourceView* texture)
    {
        return MakeEnvironmentMaterial(
            XMFLOAT4(0.20f, 0.18f, 0.16f, 1.0f),
            3.0f,
            3.0f,
            texture);
    }

    Material MakePorchFloorLineMaterial(ID3D11ShaderResourceView* texture)
    {
        return MakeEnvironmentMaterial(
            XMFLOAT4(0.18f, 0.15f, 0.12f, 1.0f),
            2.0f,
            2.0f,
            texture);
    }


    Material MakeStairTreadMaterial(ID3D11ShaderResourceView* texture)
    {
        return MakeEnvironmentMaterial(
            XMFLOAT4(0.38f, 0.31f, 0.23f, 1.0f),
            3.0f,
            1.0f,
            texture);
    }

    Material MakeStairRiserMaterial(ID3D11ShaderResourceView* texture)
    {
        return MakeEnvironmentMaterial(
            XMFLOAT4(0.25f, 0.21f, 0.17f, 1.0f),
            2.0f,
            1.0f,
            texture);
    }

    Material MakeStairSideWallMaterial(ID3D11ShaderResourceView* texture)
    {
        return MakeEnvironmentMaterial(
            XMFLOAT4(0.22f, 0.19f, 0.16f, 1.0f),
            2.0f,
            1.0f,
            texture);
    }

    Material MakeTempleWallMaterial(ID3D11ShaderResourceView* texture)
    {
        return MakeEnvironmentMaterial(
            XMFLOAT4(0.48f, 0.45f, 0.38f, 1.0f),
            5.0f,
            3.0f,
            texture);
    }

    Material MakeTempleColumnMaterial(ID3D11ShaderResourceView* texture)
    {
        return MakeEnvironmentMaterial(
            XMFLOAT4(0.56f, 0.53f, 0.45f, 1.0f),
            2.0f,
            3.0f,
            texture);
    }

    Material MakeTempleHeaderMaterial(ID3D11ShaderResourceView* texture)
    {
        return MakeEnvironmentMaterial(
            XMFLOAT4(0.44f, 0.40f, 0.33f, 1.0f),
            6.0f,
            1.0f,
            texture);
    }

    Material MakeTempleRoofMaterial(ID3D11ShaderResourceView* texture)
    {
        return MakeEnvironmentMaterial(
            XMFLOAT4(0.30f, 0.28f, 0.25f, 1.0f),
            7.0f,
            2.0f,
            texture);
    }

    Material MakeTempleThresholdMaterial(ID3D11ShaderResourceView* texture)
    {
        return MakeEnvironmentMaterial(
            XMFLOAT4(0.42f, 0.35f, 0.28f, 1.0f),
            3.0f,
            1.0f,
            texture);
    }

    Material MakeTempleEntranceInteriorMaterial(ID3D11ShaderResourceView* texture)
    {
        return MakeEnvironmentMaterial(
            XMFLOAT4(0.30f, 0.27f, 0.23f, 1.0f),
            2.0f,
            3.0f,
            texture);
    }

    Material MakeTempleEntranceDarkMaterial(ID3D11ShaderResourceView* texture)
    {
        return MakeEnvironmentMaterial(
            XMFLOAT4(0.05f, 0.045f, 0.04f, 1.0f),
            1.0f,
            1.0f,
            texture);
    }

    Material MakeTempleStairMaterial(ID3D11ShaderResourceView* texture)
    {
        return MakeEnvironmentMaterial(
            XMFLOAT4(0.44f, 0.38f, 0.30f, 1.0f),
            3.0f,
            1.0f,
            texture);
    }

    Material MakeTempleStairRiserMaterial(ID3D11ShaderResourceView* texture)
    {
        return MakeEnvironmentMaterial(
            XMFLOAT4(0.28f, 0.24f, 0.20f, 1.0f),
            2.0f,
            1.0f,
            texture);
    }

    Material MakeTempleStairSideWallMaterial(ID3D11ShaderResourceView* texture)
    {
        return MakeEnvironmentMaterial(
            XMFLOAT4(0.24f, 0.21f, 0.18f, 1.0f),
            2.0f,
            1.0f,
            texture);
    }

    Material MakeTempleUpperLandingMaterial(ID3D11ShaderResourceView* texture)
    {
        return MakeEnvironmentMaterial(
            XMFLOAT4(0.40f, 0.34f, 0.27f, 1.0f),
            3.0f,
            1.0f,
            texture);
    }



    Material MakeFieldBaseMaterial(ID3D11ShaderResourceView* texture)
    {
        return MakeEnvironmentMaterial(
            XMFLOAT4(0.18f, 0.34f, 0.15f, 1.0f),
            12.0f,
            12.0f,
            texture);
    }

    Material MakeFieldLandingMaterial(ID3D11ShaderResourceView* texture)
    {
        return MakeEnvironmentMaterial(
            XMFLOAT4(0.30f, 0.27f, 0.21f, 1.0f),
            4.0f,
            1.0f,
            texture);
    }

    Material MakeFieldRockMaterial(ID3D11ShaderResourceView* texture)
    {
        return MakeEnvironmentMaterial(
            XMFLOAT4(0.18f, 0.19f, 0.17f, 1.0f),
            2.0f,
            2.0f,
            texture);
    }

    Material MakeFieldPatchMaterial(ID3D11ShaderResourceView* texture)
    {
        return MakeEnvironmentMaterial(
            XMFLOAT4(0.25f, 0.43f, 0.18f, 1.0f),
            3.0f,
            2.0f,
            texture);
    }

    constexpr float PorchWidth = 18.0f;
    constexpr float PorchDepth = 12.0f;

    constexpr int StairCount = 21;
    constexpr float StairRise = 0.18f;

    constexpr float PorchTopY = 0.0f;
    constexpr float PorchHeight = StairCount * StairRise;

    constexpr float PorchCenterX = 0.0f;
    constexpr float PorchCenterY = PorchTopY - PorchHeight * 0.5f;
    constexpr float PorchCenterZ = 0.0f;

    // Corrected world direction:
    // -Z = temple side
    // +Z = stairs / field side
    constexpr float PorchTempleSideZ = -PorchDepth * 0.5f;
    constexpr float PorchStairSideZ = PorchDepth * 0.5f;

    constexpr float FutureFieldY = PorchTopY - PorchHeight;

    constexpr float PorchBorderThickness = 0.35f;
    constexpr float PorchBorderHeight = 0.28f;

    constexpr float StairOpeningWidth = 7.0f;
    constexpr float TempleEntranceOpeningWidth = 6.0f;

    constexpr float PorchBorderCenterY =
        PorchTopY + PorchBorderHeight * 0.5f;


    constexpr float PorchFloorDetailInset =
        PorchBorderThickness + 0.10f;

    constexpr float PorchFloorLineHeight = 0.025f;
    constexpr float PorchFloorLineThickness = 0.045f;
    constexpr float PorchFloorLineLift = 0.004f;

    constexpr float PorchFloorLineCenterY =
        PorchTopY + PorchFloorLineLift + PorchFloorLineHeight * 0.5f;

    constexpr float PorchFloorDetailWidth =
        PorchWidth - PorchFloorDetailInset * 2.0f;

    constexpr float PorchFloorDetailDepth =
        PorchDepth - PorchFloorDetailInset * 2.0f;

    constexpr int PorchFloorTileCountX = 6;
    constexpr int PorchFloorTileCountZ = 4;



    constexpr float FoundationTrimHeight = 0.22f;
    constexpr float FoundationTrimDepth = 0.18f;

    constexpr float FoundationSupportWidth = 0.45f;
    constexpr float FoundationSupportDepth = 0.28f;

    constexpr float FoundationSupportHeight = PorchHeight;

    constexpr float FoundationSupportCenterY =
        PorchTopY - FoundationSupportHeight * 0.5f;

    constexpr float StairLandingWidth = StairOpeningWidth;
    constexpr float StairLandingDepth = 1.35f;
    constexpr float StairLandingThickness = 0.16f;

    constexpr float StairLandingCenterX = PorchCenterX;

    constexpr float StairLandingCenterY =
        PorchTopY - StairLandingThickness * 0.5f;

    constexpr float StairLandingCenterZ =
        PorchStairSideZ + StairLandingDepth * 0.5f;




    



    constexpr float TempleThresholdWidth = TempleEntranceOpeningWidth;
    constexpr float TempleThresholdDepth = 1.20f;
    constexpr float TempleThresholdThickness = 0.16f;

    constexpr float TempleThresholdCenterX = PorchCenterX;

    constexpr float TempleThresholdCenterY =
        PorchTopY - TempleThresholdThickness * 0.5f;

    constexpr float TempleThresholdCenterZ =
        PorchTempleSideZ - TempleThresholdDepth * 0.5f;

    constexpr float TempleThresholdSideBlockWidth = 0.35f;
    constexpr float TempleThresholdSideBlockHeight = 0.45f;
    constexpr float TempleThresholdSideBlockDepth = TempleThresholdDepth;



    constexpr float TempleWallWidth = 20.0f;
    constexpr float TempleWallHeight = 6.0f;
    constexpr float TempleWallThickness = 0.60f;

    constexpr float TempleEntranceWidth = TempleEntranceOpeningWidth;
    constexpr float TempleEntranceHeight = 3.6f;

    constexpr float TempleWallCenterZ =
        TempleThresholdCenterZ - TempleThresholdDepth * 0.5f - TempleWallThickness * 0.5f;

    constexpr float TempleWallCenterY =
        PorchTopY + TempleWallHeight * 0.5f;



    constexpr float TempleColumnWidth = 0.70f;
    constexpr float TempleColumnDepth = 0.70f;

    constexpr float TempleColumnBaseHeight = 0.35f;
    constexpr float TempleColumnShaftHeight = 4.55f;
    constexpr float TempleColumnCapitalHeight = 0.35f;

    constexpr float TempleColumnTotalHeight =
        TempleColumnBaseHeight +
        TempleColumnShaftHeight +
        TempleColumnCapitalHeight;

    constexpr float TempleColumnCenterZ =
        TempleWallCenterZ + TempleWallThickness * 0.5f + TempleColumnDepth * 0.5f;

    constexpr float TempleColumnBaseCenterY =
        PorchTopY + TempleColumnBaseHeight * 0.5f;

    constexpr float TempleColumnShaftCenterY =
        PorchTopY + TempleColumnBaseHeight + TempleColumnShaftHeight * 0.5f;

    constexpr float TempleColumnCapitalCenterY =
        PorchTopY +
        TempleColumnBaseHeight +
        TempleColumnShaftHeight +
        TempleColumnCapitalHeight * 0.5f;

    constexpr float TempleInnerColumnOffsetX =
        TempleEntranceWidth * 0.5f + TempleColumnWidth * 0.75f;

    constexpr float TempleOuterColumnOffsetX =
        TempleEntranceWidth * 0.5f + TempleColumnWidth * 2.25f;



    constexpr float TempleHeaderWidth = 21.0f;
    constexpr float TempleHeaderHeight = 0.55f;
    constexpr float TempleHeaderDepth = 1.05f;

    constexpr float TempleHeaderCenterY =
        PorchTopY + TempleColumnTotalHeight + TempleHeaderHeight * 0.5f;

    constexpr float TempleHeaderCenterZ =
        TempleColumnCenterZ;

    constexpr float TempleCapWidth = 22.0f;
    constexpr float TempleCapHeight = 0.32f;
    constexpr float TempleCapDepth = 1.25f;

    constexpr float TempleCapCenterY =
        PorchTopY + TempleColumnTotalHeight + TempleHeaderHeight + TempleCapHeight * 0.5f;

    constexpr float TempleCapCenterZ =
        TempleColumnCenterZ;

    constexpr float TempleRoofWidth = 23.0f;
    constexpr float TempleRoofHeight = 0.45f;
    constexpr float TempleRoofDepth = 2.40f;

    constexpr float TempleRoofCenterY =
        TempleWallHeight + TempleRoofHeight * 0.5f;

    constexpr float TempleRoofCenterZ =
        TempleWallCenterZ + 0.25f;

    constexpr float TempleEntranceDepth = 3.0f;

    constexpr float TempleEntranceSideThickness = 0.28f;
    constexpr float TempleEntranceCeilingThickness = 0.28f;
    constexpr float TempleEntranceFloorThickness = 0.12f;

    constexpr float TempleEntranceFullDepth =
        TempleWallThickness + TempleEntranceDepth;

    constexpr float TempleEntranceDepthCenterZ =
        TempleWallCenterZ - TempleEntranceDepth * 0.5f;

    constexpr float TempleEntranceLeftInnerWallX =
        -TempleEntranceWidth * 0.5f - TempleEntranceSideThickness * 0.5f;

    constexpr float TempleEntranceRightInnerWallX =
        TempleEntranceWidth * 0.5f + TempleEntranceSideThickness * 0.5f;

    constexpr float TempleEntranceInnerWallCenterY =
        PorchTopY + TempleEntranceHeight * 0.5f;

    constexpr float TempleEntranceCeilingCenterY =
        TempleEntranceHeight + TempleEntranceCeilingThickness * 0.5f;

    constexpr float TempleEntranceFloorCenterY =
        PorchTopY - TempleEntranceFloorThickness * 0.5f;

    constexpr float TempleEntranceBackPlaneThickness = 0.08f;

    constexpr float TempleEntranceBackPlaneZ =
        TempleWallCenterZ -
        TempleWallThickness * 0.5f -
        TempleEntranceDepth -
        TempleEntranceBackPlaneThickness * 0.5f;

    constexpr float TempleEntranceBackPlaneCenterY =
        PorchTopY + TempleEntranceHeight * 0.5f;

    constexpr int TempleStairCount = 9;
    constexpr float TempleStairRise = 0.12f;
    constexpr float TempleStairStepHeight = TempleStairRise;
    constexpr float TempleStairStepDepth = 0.36f;
    constexpr float TempleStairWidth = TempleEntranceOpeningWidth + 1.4f;

    constexpr float TempleStairTotalRise =
        TempleStairCount * TempleStairRise;

    constexpr float TempleFloorY =
        PorchTopY + TempleStairTotalRise;

    constexpr float TempleStairTotalDepth =
        TempleStairCount * TempleStairStepDepth;

    constexpr float TempleEntranceFarZ =
        TempleWallCenterZ -
        TempleWallThickness * 0.5f -
        TempleEntranceDepth;

    constexpr float TempleStairStartZ =
        TempleEntranceFarZ;

    constexpr float TempleStairEndZ =
        TempleStairStartZ - TempleStairTotalDepth;

    constexpr float TempleUpperLandingDepth = 1.20f;
    constexpr float TempleUpperLandingThickness = 0.16f;
    constexpr float TempleUpperLandingWidth = TempleStairWidth + 0.8f;

    constexpr float TempleUpperLandingCenterY =
        TempleFloorY - TempleUpperLandingThickness * 0.5f;

    constexpr float TempleUpperLandingCenterZ =
        TempleStairEndZ - TempleUpperLandingDepth * 0.5f;




    constexpr float StairLandingLipHeight = 0.20f;
    constexpr float StairLandingLipDepth = 0.18f;

    constexpr float StairStepDepth = 0.42f;
    constexpr float StairStepHeight = StairRise;
    constexpr float StairTotalDepth = StairCount * StairStepDepth;

    constexpr float StairWidth = StairOpeningWidth;

    constexpr float StairFirstStepStartZ =
        StairLandingCenterZ + StairLandingDepth * 0.5f + StairLandingLipDepth;

    constexpr float StairEndZ =
        StairFirstStepStartZ + StairTotalDepth;

    constexpr float StairGuideThickness = 0.08f;

    constexpr float StairGuideCenterZ =
        StairFirstStepStartZ + StairTotalDepth * 0.5f;

    constexpr float StairGuideCenterY =
        PorchTopY - PorchHeight * 0.5f;

    constexpr float StairGuideDepth = StairTotalDepth;


    constexpr float StairSideWallWidth = 0.28f;
    constexpr float StairSideWallHeight = 0.42f;
    constexpr float StairSideWallDepth = StairStepDepth;

    constexpr float StairLeftSideWallCenterX =
        -StairWidth * 0.5f - StairSideWallWidth * 0.5f;

    constexpr float StairRightSideWallCenterX =
        StairWidth * 0.5f + StairSideWallWidth * 0.5f;


    constexpr float TempleStairSideWallWidth = 0.24f;
    constexpr float TempleStairSideWallHeight = 0.36f;
    constexpr float TempleStairSideWallDepth = TempleStairStepDepth;

    constexpr float TempleStairLeftSideWallCenterX =
        -TempleStairWidth * 0.5f - TempleStairSideWallWidth * 0.5f;

    constexpr float TempleStairRightSideWallCenterX =
        TempleStairWidth * 0.5f + TempleStairSideWallWidth * 0.5f;


    constexpr float FieldWidth = 36.0f;
    constexpr float FieldDepth = 28.0f;
    constexpr float FieldThickness = 0.18f;

    constexpr float FieldStartZ = StairEndZ;

    constexpr float FieldCenterX = PorchCenterX;

    constexpr float FieldCenterY =
        FutureFieldY - FieldThickness * 0.5f;

    constexpr float FieldCenterZ =
        FieldStartZ + FieldDepth * 0.5f;



    constexpr float FieldRockHeight = 0.28f;
    constexpr float FieldRockDepth = 0.75f;
    constexpr float FieldRockWidth = 2.4f;

    constexpr float FieldRockCenterY =
        FutureFieldY + FieldRockHeight * 0.5f;

    constexpr float FieldLeftEdgeX =
        -FieldWidth * 0.5f;

    constexpr float FieldRightEdgeX =
        FieldWidth * 0.5f;

    constexpr float FieldNearEdgeZ =
        FieldStartZ;

    constexpr float FieldFarEdgeZ =
        FieldStartZ + FieldDepth;





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






void WorldRenderer::DrawScene(const XMMATRIX& viewProjection)
{

    DrawNightSky(viewProjection);
    DrawMoon(viewProjection);

    DrawPorchEnvironment(viewProjection);
    DrawPlayer(viewProjection);


}

void WorldRenderer::DrawPlayer(const XMMATRIX& viewProjection)
{
    mApp->mWorld.MainPlayer.Position.y = PorchTopY + 0.3f;
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



void WorldRenderer::DrawPorchEnvironment(const XMMATRIX& viewProjection)
{
    DrawPorchFloor(viewProjection);
    DrawPorchStairs(viewProjection);
	DrawFieldPlaceholder(viewProjection);
    DrawTemplePlaceholder(viewProjection);

	//DrawPorchDebugMarkers(viewProjection);
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
    DrawPorchBase(viewProjection);
    DrawPorchFloorSurfaceDetails(viewProjection);
    DrawPorchBorders(viewProjection);
    DrawPorchFoundationDetails(viewProjection);
    DrawStairLanding(viewProjection);
    DrawTempleThreshold(viewProjection);
}


void WorldRenderer::DrawPorchBase(const XMMATRIX& viewProjection)
{
    //
    // Main raised porch foundation.
    // This block goes from Y = 0.0 down to Y = -3.78.
    //
    ID3D11ShaderResourceView* texture =
        mApp->mRenderer.mMountainSRV.Get();

    Material porchBaseMaterial =
		MakePorchFoundationMaterial(texture);

    DrawEnvironmentBox(
        mApp->mRenderer,
        viewProjection,
        porchBaseMaterial,
        PorchWidth,
        PorchHeight,
        PorchDepth,
        PorchCenterX,
        PorchCenterY,
        PorchCenterZ);

    //
    // Walkable top slab.
    // Its top face is exactly at Y = 0.0.
    //


    Material porchTopMaterial =
		MakePorchFloorMaterial(texture);

    constexpr float PorchTopSlabThickness = 0.12f;

    DrawEnvironmentBox(
        mApp->mRenderer,
        viewProjection,
        porchTopMaterial,
        PorchWidth,
        PorchTopSlabThickness,
        PorchDepth,
        PorchCenterX,
        PorchTopY - PorchTopSlabThickness * 0.5f,
        PorchCenterZ);
}

void WorldRenderer::DrawPorchFloorSurfaceDetails(const XMMATRIX& viewProjection)
{
    ID3D11ShaderResourceView* texture =
        mApp->mRenderer.mMountainSRV.Get();

    Material porchFloorLineMaterial =
        MakePorchFloorLineMaterial(texture);

    const float minX =
        -PorchWidth * 0.5f + PorchFloorDetailInset;

    const float minZ =
        -PorchDepth * 0.5f + PorchFloorDetailInset;

    const float tileWidth =
        PorchFloorDetailWidth / static_cast<float>(PorchFloorTileCountX);

    const float tileDepth =
        PorchFloorDetailDepth / static_cast<float>(PorchFloorTileCountZ);

    //
    // Lines running in the Z direction.
    // These divide the porch floor across X.
    //
    for (int lineIndex = 1; lineIndex < PorchFloorTileCountX; ++lineIndex)
    {
        const float x =
            minX + tileWidth * static_cast<float>(lineIndex);

        DrawEnvironmentBox(
            mApp->mRenderer,
            viewProjection,
            porchFloorLineMaterial,
            PorchFloorLineThickness,
            PorchFloorLineHeight,
            PorchFloorDetailDepth,
            x,
            PorchFloorLineCenterY,
            PorchCenterZ);
    }

    //
    // Lines running in the X direction.
    // These divide the porch floor across Z.
    //
    for (int lineIndex = 1; lineIndex < PorchFloorTileCountZ; ++lineIndex)
    {
        const float z =
            minZ + tileDepth * static_cast<float>(lineIndex);

        DrawEnvironmentBox(
            mApp->mRenderer,
            viewProjection,
            porchFloorLineMaterial,
            PorchFloorDetailWidth,
            PorchFloorLineHeight,
            PorchFloorLineThickness,
            PorchCenterX,
            PorchFloorLineCenterY,
            z);
    }
}

void WorldRenderer::DrawPorchBorders(const XMMATRIX& viewProjection)
{
    ID3D11ShaderResourceView* texture =
        mApp->mRenderer.mMountainSRV.Get();

    Material porchBorderMaterial =
		MakePorchBorderMaterial(texture);

    auto drawPorchBlock =
        [&](float width, float height, float depth, float x, float y, float z)
        {
            DrawEnvironmentBox(
                mApp->mRenderer,
                viewProjection,
                porchBorderMaterial,
                width,
                height,
                depth,
                x,
                y,
                z);
        };

    // Left side border.
    drawPorchBlock(
        PorchBorderThickness,
        PorchBorderHeight,
        PorchDepth,
        -PorchWidth * 0.5f + PorchBorderThickness * 0.5f,
        PorchBorderCenterY,
        PorchCenterZ);

    // Right side border.
    drawPorchBlock(
        PorchBorderThickness,
        PorchBorderHeight,
        PorchDepth,
        PorchWidth * 0.5f - PorchBorderThickness * 0.5f,
        PorchBorderCenterY,
        PorchCenterZ);

    //
    // Stair-side border: +Z side.
    //
    const float stairSideBorderPieceWidth =
        (PorchWidth - StairOpeningWidth) * 0.5f;

    const float stairSideBorderZ =
        PorchStairSideZ - PorchBorderThickness * 0.5f;

    const float leftStairSideBorderX =
        -StairOpeningWidth * 0.5f - stairSideBorderPieceWidth * 0.5f;

    const float rightStairSideBorderX =
        StairOpeningWidth * 0.5f + stairSideBorderPieceWidth * 0.5f;

    drawPorchBlock(
        stairSideBorderPieceWidth,
        PorchBorderHeight,
        PorchBorderThickness,
        leftStairSideBorderX,
        PorchBorderCenterY,
        stairSideBorderZ);

    drawPorchBlock(
        stairSideBorderPieceWidth,
        PorchBorderHeight,
        PorchBorderThickness,
        rightStairSideBorderX,
        PorchBorderCenterY,
        stairSideBorderZ);

    //
    // Temple-side border: -Z side.
    //
    const float templeSideBorderPieceWidth =
        (PorchWidth - TempleEntranceOpeningWidth) * 0.5f;

    const float templeSideBorderZ =
        PorchTempleSideZ + PorchBorderThickness * 0.5f;

    const float leftTempleSideBorderX =
        -TempleEntranceOpeningWidth * 0.5f - templeSideBorderPieceWidth * 0.5f;

    const float rightTempleSideBorderX =
        TempleEntranceOpeningWidth * 0.5f + templeSideBorderPieceWidth * 0.5f;

    drawPorchBlock(
        templeSideBorderPieceWidth,
        PorchBorderHeight,
        PorchBorderThickness,
        leftTempleSideBorderX,
        PorchBorderCenterY,
        templeSideBorderZ);

    drawPorchBlock(
        templeSideBorderPieceWidth,
        PorchBorderHeight,
        PorchBorderThickness,
        rightTempleSideBorderX,
        PorchBorderCenterY,
        templeSideBorderZ);
}

void WorldRenderer::DrawPorchFoundationDetails(const XMMATRIX& viewProjection)
{
    ID3D11ShaderResourceView* texture =
        mApp->mRenderer.mMountainSRV.Get();

    Material foundationDetailMaterial =
		MakePorchTrimMaterial(texture);

    auto drawFoundationDetail =
        [&](float width, float height, float depth, float x, float y, float z)
        {
            XMMATRIX world =
                XMMatrixScaling(width, height, depth) *
                XMMatrixTranslation(x, y, z);

            mApp->mRenderer.DrawBox(
                world,
                viewProjection,
                foundationDetailMaterial);
        };

    const float topTrimY =
        PorchTopY - FoundationTrimHeight * 0.5f;

    const float bottomTrimY =
        FutureFieldY + FoundationTrimHeight * 0.5f;

    const float stairSideTrimZ =
        PorchStairSideZ + FoundationTrimDepth * 0.5f;

    const float templeSideTrimZ =
        PorchTempleSideZ - FoundationTrimDepth * 0.5f;

    const float leftTrimX =
        -PorchWidth * 0.5f - FoundationTrimDepth * 0.5f;

    const float rightTrimX =
        PorchWidth * 0.5f + FoundationTrimDepth * 0.5f;

    // Upper trim.
    drawFoundationDetail(PorchWidth, FoundationTrimHeight, FoundationTrimDepth, PorchCenterX, topTrimY, stairSideTrimZ);
    drawFoundationDetail(PorchWidth, FoundationTrimHeight, FoundationTrimDepth, PorchCenterX, topTrimY, templeSideTrimZ);
    drawFoundationDetail(FoundationTrimDepth, FoundationTrimHeight, PorchDepth, leftTrimX, topTrimY, PorchCenterZ);
    drawFoundationDetail(FoundationTrimDepth, FoundationTrimHeight, PorchDepth, rightTrimX, topTrimY, PorchCenterZ);

    // Bottom trim.
    drawFoundationDetail(PorchWidth, FoundationTrimHeight, FoundationTrimDepth, PorchCenterX, bottomTrimY, stairSideTrimZ);
    drawFoundationDetail(PorchWidth, FoundationTrimHeight, FoundationTrimDepth, PorchCenterX, bottomTrimY, templeSideTrimZ);
    drawFoundationDetail(FoundationTrimDepth, FoundationTrimHeight, PorchDepth, leftTrimX, bottomTrimY, PorchCenterZ);
    drawFoundationDetail(FoundationTrimDepth, FoundationTrimHeight, PorchDepth, rightTrimX, bottomTrimY, PorchCenterZ);

    const float supportLeftX =
        -PorchWidth * 0.5f + FoundationSupportWidth * 0.5f;

    const float supportRightX =
        PorchWidth * 0.5f - FoundationSupportWidth * 0.5f;

    const float supportStairSideZ =
        PorchStairSideZ + FoundationSupportDepth * 0.5f;

    const float supportTempleSideZ =
        PorchTempleSideZ - FoundationSupportDepth * 0.5f;

    // Corner supports.
    drawFoundationDetail(FoundationSupportWidth, FoundationSupportHeight, FoundationSupportDepth, supportLeftX, FoundationSupportCenterY, supportStairSideZ);
    drawFoundationDetail(FoundationSupportWidth, FoundationSupportHeight, FoundationSupportDepth, supportRightX, FoundationSupportCenterY, supportStairSideZ);
    drawFoundationDetail(FoundationSupportWidth, FoundationSupportHeight, FoundationSupportDepth, supportLeftX, FoundationSupportCenterY, supportTempleSideZ);
    drawFoundationDetail(FoundationSupportWidth, FoundationSupportHeight, FoundationSupportDepth, supportRightX, FoundationSupportCenterY, supportTempleSideZ);

    // Extra supports around the future stair opening.
    const float stairSupportOffsetX =
        StairOpeningWidth * 0.5f + FoundationSupportWidth * 0.5f;

    const float stairSupportLeftX = -stairSupportOffsetX;
    const float stairSupportRightX = stairSupportOffsetX;

    drawFoundationDetail(
        FoundationSupportWidth,
        FoundationSupportHeight,
        FoundationSupportDepth,
        stairSupportLeftX,
        FoundationSupportCenterY,
        supportStairSideZ);

    drawFoundationDetail(
        FoundationSupportWidth,
        FoundationSupportHeight,
        FoundationSupportDepth,
        stairSupportRightX,
        FoundationSupportCenterY,
        supportStairSideZ);
}

void WorldRenderer::DrawStairLanding(const XMMATRIX& viewProjection)
{
    //
    // Stair landing marker on +Z side.
    // This is where the 21 stairs begin.
    //

    ID3D11ShaderResourceView* texture =
        mApp->mRenderer.mMountainSRV.Get();

    Material stairLandingMaterial =
		MakeTempleThresholdMaterial(texture);

    XMMATRIX stairLandingWorld =
        XMMatrixScaling(
            StairLandingWidth,
            StairLandingThickness,
            StairLandingDepth) *
        XMMatrixTranslation(
            StairLandingCenterX,
            StairLandingCenterY,
            StairLandingCenterZ);

    mApp->mRenderer.DrawBox(
        stairLandingWorld,
        viewProjection,
        stairLandingMaterial);

    //
    // Small lip at the far +Z edge of the landing.
    //
    Material stairLandingLipMaterial =
		MakePorchTrimMaterial(texture);

    const float stairLandingFarZ =
        StairLandingCenterZ + StairLandingDepth * 0.5f;

    XMMATRIX stairLandingLipWorld =
        XMMatrixScaling(
            StairLandingWidth,
            StairLandingLipHeight,
            StairLandingLipDepth) *
        XMMatrixTranslation(
            StairLandingCenterX,
            PorchTopY - StairLandingLipHeight * 0.5f,
            stairLandingFarZ + StairLandingLipDepth * 0.5f);

    mApp->mRenderer.DrawBox(
        stairLandingLipWorld,
        viewProjection,
        stairLandingLipMaterial);
}

void WorldRenderer::DrawTempleThreshold(const XMMATRIX& viewProjection)
{
    //
    // Temple threshold marker on -Z side.
    // This is where the future temple entrance connects.
    //
    ID3D11ShaderResourceView* texture =
        mApp->mRenderer.mMountainSRV.Get();

    Material templeThresholdMaterial =
		MakeTempleThresholdMaterial(texture);

    XMMATRIX templeThresholdWorld =
        XMMatrixScaling(
            TempleThresholdWidth,
            TempleThresholdThickness,
            TempleThresholdDepth) *
        XMMatrixTranslation(
            TempleThresholdCenterX,
            TempleThresholdCenterY,
            TempleThresholdCenterZ);

    mApp->mRenderer.DrawBox(
        templeThresholdWorld,
        viewProjection,
        templeThresholdMaterial);

    //
    // Low side blocks marking the future temple entrance edges.
    //
    Material templeThresholdSideMaterial =
		MakePorchTrimMaterial(texture);

    const float templeThresholdSideBlockCenterY =
        PorchTopY + TempleThresholdSideBlockHeight * 0.5f;

    const float templeThresholdLeftBlockX =
        -TempleEntranceOpeningWidth * 0.5f -
        TempleThresholdSideBlockWidth * 0.5f;

    const float templeThresholdRightBlockX =
        TempleEntranceOpeningWidth * 0.5f +
        TempleThresholdSideBlockWidth * 0.5f;

    XMMATRIX templeThresholdLeftBlockWorld =
        XMMatrixScaling(
            TempleThresholdSideBlockWidth,
            TempleThresholdSideBlockHeight,
            TempleThresholdSideBlockDepth) *
        XMMatrixTranslation(
            templeThresholdLeftBlockX,
            templeThresholdSideBlockCenterY,
            TempleThresholdCenterZ);

    mApp->mRenderer.DrawBox(
        templeThresholdLeftBlockWorld,
        viewProjection,
        templeThresholdSideMaterial);

    XMMATRIX templeThresholdRightBlockWorld =
        XMMatrixScaling(
            TempleThresholdSideBlockWidth,
            TempleThresholdSideBlockHeight,
            TempleThresholdSideBlockDepth) *
        XMMatrixTranslation(
            templeThresholdRightBlockX,
            templeThresholdSideBlockCenterY,
            TempleThresholdCenterZ);

    mApp->mRenderer.DrawBox(
        templeThresholdRightBlockWorld,
        viewProjection,
        templeThresholdSideMaterial);
}


void WorldRenderer::DrawTemplePlaceholder(const XMMATRIX& viewProjection)
{
    DrawTempleWall(viewProjection);
    DrawTempleEntranceDepth(viewProjection);
    DrawTempleStairs(viewProjection);
    DrawTempleColumns(viewProjection);
    DrawTempleHeaderAndRoof(viewProjection);
}

void WorldRenderer::DrawTempleStairs(const XMMATRIX& viewProjection)
{
    ID3D11ShaderResourceView* texture =
        mApp->mRenderer.mMountainSRV.Get();

    Material templeStairMaterial =
        MakeTempleStairMaterial(texture);

    Material templeStairRiserMaterial =
        MakeTempleStairRiserMaterial(texture);

    Material templeUpperLandingMaterial =
        MakeTempleUpperLandingMaterial(texture);

    Material templeStairSideWallMaterial =
        MakeTempleStairSideWallMaterial(texture);

    constexpr float TempleStairRiserThickness = 0.035f;

    for (int stairIndex = 0; stairIndex < TempleStairCount; ++stairIndex)
    {
        const float stepTopY =
            PorchTopY + static_cast<float>(stairIndex + 1) * TempleStairRise;

        const float stepCenterY =
            stepTopY - TempleStairStepHeight * 0.5f;

        const float stepCenterZ =
            TempleStairStartZ -
            static_cast<float>(stairIndex) * TempleStairStepDepth -
            TempleStairStepDepth * 0.5f;

        DrawEnvironmentBox(
            mApp->mRenderer,
            viewProjection,
            templeStairMaterial,
            TempleStairWidth,
            TempleStairStepHeight,
            TempleStairStepDepth,
            PorchCenterX,
            stepCenterY,
            stepCenterZ);

        //
        // Because these stairs go toward -Z,
        // the visible riser face is on the +Z side of each step.
        //
        const float riserFaceZ =
            stepCenterZ + TempleStairStepDepth * 0.5f -
            TempleStairRiserThickness * 0.5f;

        DrawEnvironmentBox(
            mApp->mRenderer,
            viewProjection,
            templeStairRiserMaterial,
            TempleStairWidth,
            TempleStairStepHeight,
            TempleStairRiserThickness,
            PorchCenterX,
            stepCenterY,
            riserFaceZ);

        //
// Low side wall / rail base for the temple stair.
// These follow the 9 stairs upward toward -Z.
//
        const float templeSideWallCenterY =
            stepTopY + TempleStairSideWallHeight * 0.5f;

        DrawEnvironmentBox(
            mApp->mRenderer,
            viewProjection,
            templeStairSideWallMaterial,
            TempleStairSideWallWidth,
            TempleStairSideWallHeight,
            TempleStairSideWallDepth,
            TempleStairLeftSideWallCenterX,
            templeSideWallCenterY,
            stepCenterZ);

        DrawEnvironmentBox(
            mApp->mRenderer,
            viewProjection,
            templeStairSideWallMaterial,
            TempleStairSideWallWidth,
            TempleStairSideWallHeight,
            TempleStairSideWallDepth,
            TempleStairRightSideWallCenterX,
            templeSideWallCenterY,
            stepCenterZ);


    }

    //
    // Upper landing after the 9 stairs.
    // This is where the raised temple area begins.
    //
    DrawEnvironmentBox(
        mApp->mRenderer,
        viewProjection,
        templeUpperLandingMaterial,
        TempleUpperLandingWidth,
        TempleUpperLandingThickness,
        TempleUpperLandingDepth,
        PorchCenterX,
        TempleUpperLandingCenterY,
        TempleUpperLandingCenterZ);
}

void WorldRenderer::DrawTempleWall(const XMMATRIX& viewProjection)
{
    ID3D11ShaderResourceView* texture =
        mApp->mRenderer.mMountainSRV.Get();
    Material templeWallMaterial =
		MakeTempleWallMaterial(texture);

    auto drawTempleBlock =
        [&](float width, float height, float depth, float x, float y, float z)
        {
            DrawEnvironmentBox(
                mApp->mRenderer,
                viewProjection,
                templeWallMaterial,
                width,
                height,
                depth,
                x,
                y,
				z);
        };

    const float sideWallWidth =
        (TempleWallWidth - TempleEntranceWidth) * 0.5f;

    const float leftWallCenterX =
        -TempleEntranceWidth * 0.5f - sideWallWidth * 0.5f;

    const float rightWallCenterX =
        TempleEntranceWidth * 0.5f + sideWallWidth * 0.5f;

    // Left wall block.
    drawTempleBlock(
        sideWallWidth,
        TempleWallHeight,
        TempleWallThickness,
        leftWallCenterX,
        TempleWallCenterY,
        TempleWallCenterZ);

    // Right wall block.
    drawTempleBlock(
        sideWallWidth,
        TempleWallHeight,
        TempleWallThickness,
        rightWallCenterX,
        TempleWallCenterY,
        TempleWallCenterZ);

    //
    // Top lintel block above the entrance.
    //
    const float lintelHeight =
        TempleWallHeight - TempleEntranceHeight;

    const float lintelCenterY =
        TempleEntranceHeight + lintelHeight * 0.5f;

    drawTempleBlock(
        TempleEntranceWidth,
        lintelHeight,
        TempleWallThickness,
        PorchCenterX,
        lintelCenterY,
        TempleWallCenterZ);
}

void WorldRenderer::DrawTempleEntranceDepth(const XMMATRIX& viewProjection)
{
    ID3D11ShaderResourceView* texture =
        mApp->mRenderer.mMountainSRV.Get();

    Material entranceInteriorMaterial =
        MakeTempleEntranceInteriorMaterial(texture);

    Material entranceDarkMaterial =
        MakeTempleEntranceDarkMaterial(texture);

    //
    // Left inner doorway wall.
    // This creates visible depth on the left side of the entrance.
    //
    DrawEnvironmentBox(
        mApp->mRenderer,
        viewProjection,
        entranceInteriorMaterial,
        TempleEntranceSideThickness,
        TempleEntranceHeight,
        TempleEntranceFullDepth,
        TempleEntranceLeftInnerWallX,
        TempleEntranceInnerWallCenterY,
        TempleEntranceDepthCenterZ);

    //
    // Right inner doorway wall.
    //
    DrawEnvironmentBox(
        mApp->mRenderer,
        viewProjection,
        entranceInteriorMaterial,
        TempleEntranceSideThickness,
        TempleEntranceHeight,
        TempleEntranceFullDepth,
        TempleEntranceRightInnerWallX,
        TempleEntranceInnerWallCenterY,
        TempleEntranceDepthCenterZ);

    //
    // Inner ceiling slab above the doorway.
    //
    DrawEnvironmentBox(
        mApp->mRenderer,
        viewProjection,
        entranceInteriorMaterial,
        TempleEntranceWidth,
        TempleEntranceCeilingThickness,
        TempleEntranceFullDepth,
        PorchCenterX,
        TempleEntranceCeilingCenterY,
        TempleEntranceDepthCenterZ);

    //
    // Inner floor slab.
    // Its top face is level with PorchTopY.
    //
    DrawEnvironmentBox(
        mApp->mRenderer,
        viewProjection,
        entranceInteriorMaterial,
        TempleEntranceWidth,
        TempleEntranceFloorThickness,
        TempleEntranceFullDepth,
        PorchCenterX,
        TempleEntranceFloorCenterY,
        TempleEntranceDepthCenterZ);

    /*
    //
    // Dark back plane deeper inside the temple.
    // This creates the feeling of an interior passage.
    //
    DrawEnvironmentBox(
        mApp->mRenderer,
        viewProjection,
        entranceDarkMaterial,
        TempleEntranceWidth,
        TempleEntranceHeight,
        TempleEntranceBackPlaneThickness,
        PorchCenterX,
        TempleEntranceBackPlaneCenterY,
        TempleEntranceBackPlaneZ);
        */
}

void WorldRenderer::DrawTempleColumns(const XMMATRIX& viewProjection)
{
    //
    // Simple temple columns.
    // These sit on the +Z/front face of the temple wall.
    //
    ID3D11ShaderResourceView* texture =
        mApp->mRenderer.mMountainSRV.Get();

    Material templeColumnMaterial =
		MakeTempleColumnMaterial(texture);

    auto drawTempleColumnPart =
        [&](float width, float height, float depth, float x, float y, float z)
        {

            DrawEnvironmentBox(
                mApp->mRenderer,
                viewProjection,
                templeColumnMaterial,
                width,
                height,
                depth,
                x,
                y,
				z);
        };

    auto drawTempleColumn =
        [&](float x)
        {
            //
            // Base block.
            //
            drawTempleColumnPart(
                TempleColumnWidth * 1.35f,
                TempleColumnBaseHeight,
                TempleColumnDepth * 1.25f,
                x,
                TempleColumnBaseCenterY,
                TempleColumnCenterZ);

            //
            // Main vertical shaft.
            //
            drawTempleColumnPart(
                TempleColumnWidth,
                TempleColumnShaftHeight,
                TempleColumnDepth,
                x,
                TempleColumnShaftCenterY,
                TempleColumnCenterZ);

            //
            // Top capital block.
            //
            drawTempleColumnPart(
                TempleColumnWidth * 1.45f,
                TempleColumnCapitalHeight,
                TempleColumnDepth * 1.30f,
                x,
                TempleColumnCapitalCenterY,
                TempleColumnCenterZ);
        };

    //
    // Two inner columns around the entrance.
    //
    drawTempleColumn(-TempleInnerColumnOffsetX);
    drawTempleColumn(TempleInnerColumnOffsetX);

    //
    // Two outer columns to make the temple wall feel wider and more monumental.
    //
    drawTempleColumn(-TempleOuterColumnOffsetX);
    drawTempleColumn(TempleOuterColumnOffsetX);
}

void WorldRenderer::DrawTempleHeaderAndRoof(const XMMATRIX& viewProjection)
{
    //
    // Temple header and roof mass.
    // This gives the temple a stronger architectural silhouette.
    //
    ID3D11ShaderResourceView* texture =
        mApp->mRenderer.mMountainSRV.Get();

    Material templeHeaderMaterial =
		MakeTempleHeaderMaterial(texture);

    auto drawTempleHeaderBlock =
        [&](float width, float height, float depth, float x, float y, float z)
        {
            DrawEnvironmentBox(
                mApp->mRenderer,
                viewProjection,
                templeHeaderMaterial,
                width,
                height,
                depth,
                x,
                y,
				z);
        };

    //
    // Main horizontal header beam.
    // This rests above the columns.
    //
    drawTempleHeaderBlock(
        TempleHeaderWidth,
        TempleHeaderHeight,
        TempleHeaderDepth,
        PorchCenterX,
        TempleHeaderCenterY,
        TempleHeaderCenterZ);

    //
    // Thinner cap beam above the main header.
    // Slightly wider and deeper to create an overhang.
    //
    drawTempleHeaderBlock(
        TempleCapWidth,
        TempleCapHeight,
        TempleCapDepth,
        PorchCenterX,
        TempleCapCenterY,
        TempleCapCenterZ);

    //
    // Main roof slab.
    //
    Material templeRoofMaterial =
		MakeTempleRoofMaterial(texture);


    DrawEnvironmentBox(
        mApp->mRenderer,
        viewProjection,
        templeHeaderMaterial,
        TempleRoofWidth,
        TempleRoofHeight,
        TempleRoofDepth,
        PorchCenterX,
        TempleRoofCenterY,
        TempleRoofCenterZ);
}

void WorldRenderer::DrawPorchStairs(const XMMATRIX& viewProjection)
{
    DrawStairSteps(viewProjection);
    DrawStairSideWalls(viewProjection);
}

void WorldRenderer::DrawStairSteps(const XMMATRIX& viewProjection)
{
    ID3D11ShaderResourceView* texture =
        mApp->mRenderer.mMountainSRV.Get();

    Material stairMaterial =
		MakeStairTreadMaterial(texture);

    Material stairRiserMaterial =
		MakeStairRiserMaterial(texture);

    constexpr float RiserFaceThickness = 0.035f;

    for (int stairIndex = 0; stairIndex < StairCount; ++stairIndex)
    {
        const float stepTopY =
            PorchTopY - static_cast<float>(stairIndex + 1) * StairRise;

        const float stepCenterY =
            stepTopY - StairStepHeight * 0.5f;

        const float stepCenterZ =
            StairFirstStepStartZ +
            static_cast<float>(stairIndex) * StairStepDepth +
            StairStepDepth * 0.5f;

        //
        // Main stair block.
        //
        DrawEnvironmentBox(
            mApp->mRenderer,
            viewProjection,
            stairMaterial,
            StairWidth,
            StairStepHeight,
            StairStepDepth,
            PorchCenterX,
            stepCenterY,
            stepCenterZ);

        //
        // Slight darker riser face.
        // This makes each stair visually readable.
        //
        const float riserFaceZ =
            stepCenterZ - StairStepDepth * 0.5f + RiserFaceThickness * 0.5f;

        DrawEnvironmentBox(
            mApp->mRenderer,
            viewProjection,
            stairRiserMaterial,
            StairWidth,
            StairStepHeight,
            RiserFaceThickness,
            PorchCenterX,
            stepCenterY,
            riserFaceZ);
    }
}

void WorldRenderer::DrawStairSideWalls(const XMMATRIX& viewProjection)
{
    ID3D11ShaderResourceView* texture =
        mApp->mRenderer.mMountainSRV.Get();

    Material stairSideWallMaterial =
		MakeStairSideWallMaterial(texture);

    for (int stairIndex = 0; stairIndex < StairCount; ++stairIndex)
    {
        const float stepTopY =
            PorchTopY - static_cast<float>(stairIndex + 1) * StairRise;

        const float stepCenterZ =
            StairFirstStepStartZ +
            static_cast<float>(stairIndex) * StairStepDepth +
            StairStepDepth * 0.5f;

        const float sideWallCenterY =
            stepTopY + StairSideWallHeight * 0.5f;

        //
        // Left stair side wall.
        //
        DrawEnvironmentBox(
            mApp->mRenderer,
            viewProjection,
            stairSideWallMaterial,
            StairSideWallWidth,
            StairSideWallHeight,
            StairSideWallDepth,
            StairLeftSideWallCenterX,
            sideWallCenterY,
            stepCenterZ);

        //
        // Right stair side wall.
        //
        DrawEnvironmentBox(
            mApp->mRenderer,
            viewProjection,
            stairSideWallMaterial,
            StairSideWallWidth,
            StairSideWallHeight,
            StairSideWallDepth,
            StairRightSideWallCenterX,
            sideWallCenterY,
            stepCenterZ);
    }
}

void WorldRenderer::DrawFieldPlaceholder(const XMMATRIX& viewProjection)
{
    DrawFieldBase(viewProjection);
    DrawFieldLanding(viewProjection);
    DrawFieldRocks(viewProjection);
    DrawFieldPatches(viewProjection);
}


void WorldRenderer::DrawFieldBase(const XMMATRIX& viewProjection)
{
    //
    // First field platform at the bottom of the stairs.
    // Top face is exactly at FutureFieldY.
    //
    ID3D11ShaderResourceView* texture =
        mApp->mRenderer.mMountainSRV.Get();

    Material fieldMaterial =
		MakeFieldBaseMaterial(texture);

    DrawEnvironmentBox(
        mApp->mRenderer,
        viewProjection,
        fieldMaterial,
        FieldWidth,
        FieldThickness,
        FieldDepth,
        FieldCenterX,
        FieldCenterY,
		FieldCenterZ);

}

void WorldRenderer::DrawFieldLanding(const XMMATRIX& viewProjection)
{
    //
    // Small landing strip where the stairs meet the field.
    // This visually locks the last stair into the field surface.
    //
    ID3D11ShaderResourceView* texture =
        mApp->mRenderer.mMountainSRV.Get();

    Material fieldLandingMaterial =
		MakeFieldLandingMaterial(texture);

    constexpr float FieldLandingWidth = StairWidth + 1.2f;
    constexpr float FieldLandingDepth = 1.0f;
    constexpr float FieldLandingThickness = 0.16f;

    const float fieldLandingCenterY =
        FutureFieldY - FieldLandingThickness * 0.5f;

    const float fieldLandingCenterZ =
        StairEndZ + FieldLandingDepth * 0.5f;

    DrawEnvironmentBox(
        mApp->mRenderer,
        viewProjection,
        fieldLandingMaterial,
        FieldLandingWidth,
        FieldLandingThickness,
        FieldLandingDepth,
        PorchCenterX,
        fieldLandingCenterY,
		fieldLandingCenterZ);
}

void WorldRenderer::DrawFieldRocks(const XMMATRIX& viewProjection)
{
    //
    // Rocky field edge details.
    // These break the perfect rectangular silhouette of the field.
    //
    ID3D11ShaderResourceView* texture =
        mApp->mRenderer.mMountainSRV.Get();

    Material fieldRockMaterial =
		MakeFieldRockMaterial(texture);

    auto drawFieldRock =
        [&](float width, float height, float depth, float x, float y, float z)
        {

            DrawEnvironmentBox(
                mApp->mRenderer,
                viewProjection,
                fieldRockMaterial,
                width,
                height,
                depth,
                x,
                y,
				z);
        };

    //
    // Left-side rocky edge pieces.
    //
    drawFieldRock(
        FieldRockWidth,
        FieldRockHeight,
        FieldRockDepth,
        FieldLeftEdgeX + 2.0f,
        FieldRockCenterY,
        FieldNearEdgeZ + 3.0f);

    drawFieldRock(
        FieldRockWidth * 1.3f,
        FieldRockHeight * 1.2f,
        FieldRockDepth,
        FieldLeftEdgeX + 1.4f,
        FieldRockCenterY + 0.04f,
        FieldNearEdgeZ + 9.0f);

    drawFieldRock(
        FieldRockWidth * 0.9f,
        FieldRockHeight,
        FieldRockDepth * 1.2f,
        FieldLeftEdgeX + 2.6f,
        FieldRockCenterY,
        FieldNearEdgeZ + 17.0f);

    //
    // Right-side rocky edge pieces.
    //
    drawFieldRock(
        FieldRockWidth * 1.1f,
        FieldRockHeight,
        FieldRockDepth,
        FieldRightEdgeX - 2.2f,
        FieldRockCenterY,
        FieldNearEdgeZ + 5.0f);

    drawFieldRock(
        FieldRockWidth,
        FieldRockHeight * 1.35f,
        FieldRockDepth * 1.1f,
        FieldRightEdgeX - 1.5f,
        FieldRockCenterY + 0.05f,
        FieldNearEdgeZ + 13.0f);

    drawFieldRock(
        FieldRockWidth * 1.4f,
        FieldRockHeight,
        FieldRockDepth,
        FieldRightEdgeX - 2.8f,
        FieldRockCenterY,
        FieldNearEdgeZ + 22.0f);

    //
    // Far +Z rocky edge pieces.
    //
    drawFieldRock(
        FieldRockWidth * 1.5f,
        FieldRockHeight,
        FieldRockDepth,
        -6.0f,
        FieldRockCenterY,
        FieldFarEdgeZ - 1.2f);

    drawFieldRock(
        FieldRockWidth * 1.2f,
        FieldRockHeight * 1.25f,
        FieldRockDepth,
        0.0f,
        FieldRockCenterY + 0.04f,
        FieldFarEdgeZ - 0.9f);

    drawFieldRock(
        FieldRockWidth * 1.6f,
        FieldRockHeight,
        FieldRockDepth * 1.2f,
        6.4f,
        FieldRockCenterY,
        FieldFarEdgeZ - 1.4f);
}

void WorldRenderer::DrawFieldPatches(const XMMATRIX& viewProjection)
{
    //
    // Raised ground patches.
    // These are subtle height variations on the field surface.
    //
    ID3D11ShaderResourceView* texture =
        mApp->mRenderer.mMountainSRV.Get();

    Material fieldPatchMaterial =
		MakeFieldPatchMaterial(texture);

    auto drawFieldPatch =
        [&](float width, float height, float depth, float x, float z)
        {
            const float y =
                FutureFieldY + height * 0.5f;


            DrawEnvironmentBox(
                mApp->mRenderer,
                viewProjection,
                fieldPatchMaterial,
                width,
                height,
                depth,
                x,
				y,
				z);

        };

    drawFieldPatch(
        4.5f,
        0.06f,
        2.0f,
        -5.0f,
        FieldNearEdgeZ + 6.0f);

    drawFieldPatch(
        3.2f,
        0.05f,
        2.8f,
        4.0f,
        FieldNearEdgeZ + 10.0f);

    drawFieldPatch(
        5.0f,
        0.07f,
        2.4f,
        1.0f,
        FieldNearEdgeZ + 18.0f);

    drawFieldPatch(
        3.8f,
        0.05f,
        2.2f,
        -7.0f,
        FieldNearEdgeZ + 21.5f);
}

void WorldRenderer::DrawPorchDebugMarkers(const XMMATRIX& viewProjection)
{
    Material templeMarkerMaterial =
    {
        XMFLOAT4(0.50f, 0.30f, 0.25f, 1.0f),
        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        mApp->mRenderer.mMountainSRV.Get(),
        1.0f
    };

    Material stairMarkerMaterial =
    {
        XMFLOAT4(0.25f, 0.35f, 0.55f, 1.0f),
        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        mApp->mRenderer.mMountainSRV.Get(),
        1.0f
    };

    XMMATRIX templeMarkerWorld =
        XMMatrixScaling(1.0f, 1.0f, 1.0f) *
        XMMatrixTranslation(
            -PorchWidth * 0.5f - 1.5f,
            PorchTopY + 0.5f,
            PorchTempleSideZ);

    mApp->mRenderer.DrawBox(
        templeMarkerWorld,
        viewProjection,
        templeMarkerMaterial);

    XMMATRIX stairMarkerWorld =
        XMMatrixScaling(1.0f, 1.0f, 1.0f) *
        XMMatrixTranslation(
            PorchWidth * 0.5f + 1.5f,
            PorchTopY + 0.5f,
            PorchStairSideZ);

    mApp->mRenderer.DrawBox(
        stairMarkerWorld,
        viewProjection,
        stairMarkerMaterial);
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

