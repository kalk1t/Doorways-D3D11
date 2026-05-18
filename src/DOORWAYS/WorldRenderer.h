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

private:
    void DrawNightSky(const XMMATRIX& viewProjection);
    void DrawMoon(const XMMATRIX& viewProjection);
    void DrawImportedScene(const XMMATRIX& viewProjection);


    void DrawPlayer(const XMMATRIX& viewProjection);
    void DrawPlayerBody(
        const XMMATRIX& playerWorld,
        const XMMATRIX& viewProjection,
        const Material& bodyMaterial);
    void DrawPlayerHead(
        const XMMATRIX& playerWorld,
        const XMMATRIX& viewProjection,
        const Material& headMaterial);
    void DrawPlayerArms(
        const XMMATRIX& playerWorld,
        const XMMATRIX& viewProjection,
        const Material& bodyMaterial);
    void DrawPlayerLegs(
        const XMMATRIX& playerWorld,
        const XMMATRIX& viewProjection,
        const Material& bodyMaterial);
    void DrawPlayerFrontMarker(
        const XMMATRIX& playerWorld,
        const XMMATRIX& viewProjection,
        const Material& frontMarkerMaterial);
    void DrawPlayerShadow(
        const XMMATRIX& viewProjection);
    void DrawPlayerBoundsDebug(
        const XMMATRIX& viewProjection);

	App* mApp = nullptr;
};