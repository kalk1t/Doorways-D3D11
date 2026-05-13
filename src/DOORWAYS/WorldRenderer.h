#pragma once
#include <DirectXMath.h>

#include "Renderer.h"
using namespace DirectX;

class App;

class WorldRenderer
{

public:

    WorldRenderer(App* app);

    void DrawScene(const XMMATRIX& viewProjection);

    void DrawPlayer(const XMMATRIX& viewProjection);


    void DrawNightSky(const XMMATRIX& viewProjection);
    void DrawMoon(const XMMATRIX& viewProjection);

    void DrawPorchEnvironment(const XMMATRIX& viewProjection);
    void DrawPorchFloor(const XMMATRIX& viewProjection);
    void DrawPorchBase(const XMMATRIX& viewProjection);
    void DrawPorchBorders(const XMMATRIX& viewProjection);
    void DrawPorchFoundationDetails(const XMMATRIX& viewProjection);
    void DrawPorchFloorSurfaceDetails(const XMMATRIX& viewProjection);

    void DrawStairLanding(const XMMATRIX& viewProjection);
    void DrawTempleThreshold(const XMMATRIX& viewProjection);

    void DrawPorchStairs(const XMMATRIX& viewProjection);
    void DrawStairSteps(const XMMATRIX& viewProjection);
    void DrawStairSideWalls(const XMMATRIX& viewProjection);

    void DrawTemplePlaceholder(const XMMATRIX& viewProjection);
    void DrawTempleStairs(const XMMATRIX& viewProjection);
    void DrawTempleWall(const XMMATRIX& viewProjection);
    void DrawTempleEntranceDepth(const XMMATRIX& viewProjection);
    void DrawTempleColumns(const XMMATRIX& viewProjection);
    void DrawTempleHeaderAndRoof(const XMMATRIX& viewProjection);

    void DrawFieldPlaceholder(const XMMATRIX& viewProjection);
    void DrawFieldBase(const XMMATRIX& viewProjection);
    void DrawFieldLanding(const XMMATRIX& viewProjection);
    void DrawFieldRocks(const XMMATRIX& viewProjection);
    void DrawFieldPatches(const XMMATRIX& viewProjection);



    void DrawPorchFence(const XMMATRIX& viewProjection);

    void DrawPorchDebugMarkers(const XMMATRIX& viewProjection);




	App* mApp = nullptr;
};