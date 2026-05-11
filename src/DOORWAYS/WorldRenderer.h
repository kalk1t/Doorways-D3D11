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
    void DrawInteractionPrompt(const XMMATRIX& viewProjection);
    void DrawEnvironmentObjects(const XMMATRIX& viewProjection);
    void DrawEnvironmentBackdrop(const XMMATRIX& viewProjection, const XMFLOAT4& color);

    void DrawDoorwayAtmosphere(const XMMATRIX& viewProjection);

    void DrawPorchEnvironment(const XMMATRIX& viewProjection);

    void DrawNightSky(const XMMATRIX& viewProjection);
    void DrawMoon(const XMMATRIX& viewProjection);
    void DrawStars(const XMMATRIX& viewProjection);

    void DrawPorchFloor(const XMMATRIX& viewProjection);
    void DrawPorchFence(const XMMATRIX& viewProjection);

    void DrawMountainRing(const XMMATRIX& viewProjection);
    void DrawWaterfall(const XMMATRIX& viewProjection);



    void DrawSunnyEnvironment(const XMMATRIX& viewProjection);
    void DrawRainyEnvironment(const XMMATRIX& viewProjection);
    void DrawSnowyEnvironment(const XMMATRIX& viewProjection);

	App* mApp = nullptr;
};