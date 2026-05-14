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


	App* mApp = nullptr;
};