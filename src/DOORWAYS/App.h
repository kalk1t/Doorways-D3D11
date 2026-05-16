#pragma once

#include <windows.h>
#include "Renderer.h"
#include "WorldRenderer.h"
#include "WorldState.h"
#include "PlayerController.h"

using namespace DirectX;



class App
{
public:
    App(HINSTANCE hInstance);
    bool Initialize();
    int Run();


private:
    
    bool InitWindow();

    void Update(float deltaTime);
    void Render();


public:

    // Window Title
    //====================================================
    void UpdateWindowTitle();



private:


    HINSTANCE mAppInstance = nullptr;
    HWND mMainWindow = nullptr;

    int mClientWidth = 1280;
    int mClientHeight = 720;

    bool mWasFilterToggleKeyDown = false;
    float mFilterToggleCooldown = 0.0f;

    bool mWasSceneResetKeyDown = false;
    bool mWasScenePrintKeyDown = false;
public:

    Renderer mRenderer;
    WorldRenderer mWorldRenderer;
    WorldState mWorld;
	PlayerController mPlayerController;
};