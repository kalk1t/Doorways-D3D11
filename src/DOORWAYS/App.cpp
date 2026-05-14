#include "App.h"
#include "RenderTypes.h"
#include "EnvironmentSettings.h"

#include <d3dcompiler.h>
#include <cstddef>
#include <cstdint>
#include <DirectXMath.h>
#include <string>
#include <chrono>
#include <cmath>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;



namespace
{
    const wchar_t* gWindowClassName = L"DoorwaysDemoWindowClass";

   
    LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE)
            {
                DestroyWindow(hwnd);
            }
            return 0;
        }

        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }


}

App::App(HINSTANCE hInstance)
    : mAppInstance(hInstance),
    mWorldRenderer(this)
{
}

bool App::Initialize()
{
    if (!InitWindow())
    {
        MessageBoxW(
            nullptr,
            L"Failed to create window.",
            L"Initialization Error",
            MB_OK | MB_ICONERROR);

        return false;
    }

    if (!mRenderer.Initialize(mMainWindow, mClientWidth, mClientHeight))
    {
        MessageBoxW(
            nullptr,
            L"Failed to initialize Direct3D 11.",
            L"Initialization Error",
            MB_OK | MB_ICONERROR);

        return false;
    }

    if (!mRenderer.BuildShaders())
    {
        MessageBoxW(
            nullptr,
            L"Failed to build shaders.",
            L"Initialization Error",
            MB_OK | MB_ICONERROR);

        return false;
    }

    if (!mRenderer.BuildGeometry())
    {
        MessageBoxW(
            nullptr,
            L"Failed to build geometry.",
            L"Initialization Error",
            MB_OK | MB_ICONERROR);

        return false;
    }

    if (!mRenderer.BuildTextures())
    {
        MessageBoxW(
            nullptr,
            L"Failed to build textures.",
            L"Initialization Error",
            MB_OK | MB_ICONERROR);

        return false;
    }

    if (!mRenderer.BuildSamplerState())
    {
        MessageBoxW(
            nullptr,
            L"Failed to build sampler state.",
            L"Initialization Error",
            MB_OK | MB_ICONERROR);

        return false;
    }

    if (!mRenderer.BuildBlendStates())
    {
        MessageBoxW(
            nullptr,
            L"Failed to build blend states.",
            L"Initialization Error",
            MB_OK | MB_ICONERROR);

        return false;
    }

    return true;
}

int App::Run()
{
    MSG msg = {};

	auto previousTime = std::chrono::high_resolution_clock::now();

    while (msg.message != WM_QUIT)
    {
        if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        else
        {

			auto currentTime = std::chrono::high_resolution_clock::now();

			float deltaTime =std::chrono::duration<float>(currentTime - previousTime).count();

			previousTime = currentTime;
            if(deltaTime>0.1f)
            {
                deltaTime = 0.1f;
			}
			Update(deltaTime);
            Render();
        }
    }

    return static_cast<int>(msg.wParam);
}

bool App::InitWindow()
{
    WNDCLASSW wc = {};

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = MainWndProc;
    wc.hInstance = mAppInstance;
    wc.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
    wc.lpszClassName = gWindowClassName;

    if (!RegisterClassW(&wc))
    {
        return false;
    }

    RECT windowRect = {
        0,
        0,
        mClientWidth,
        mClientHeight
    };

    DWORD windowStyle =
        WS_OVERLAPPED |
        WS_CAPTION |
        WS_SYSMENU |
        WS_MINIMIZEBOX;

    if (!AdjustWindowRect(&windowRect, windowStyle, false))
    {
        return false;
    }

    int windowWidth = windowRect.right - windowRect.left;
    int windowHeight = windowRect.bottom - windowRect.top;

	int x = (GetSystemMetrics(SM_CXSCREEN) - windowWidth) / 2;
	int y = (GetSystemMetrics(SM_CYSCREEN) - windowHeight) / 2;

    mMainWindow = CreateWindowW(
        gWindowClassName,
        L"Doorways: Direct3D11 Demo",
        windowStyle,
        x,
        y,
        windowWidth,
        windowHeight,
        nullptr,
        nullptr,
        mAppInstance,
        nullptr);

    if (!mMainWindow)
    {
        return false;
    }

    ShowWindow(mMainWindow, SW_SHOW);
    UpdateWindow(mMainWindow);

    return true;
}

void App::UpdateWindowTitle()
{
    SetWindowTextW(
        mMainWindow,
        L"Doorways: Playable 3D Scene");
}

void App::Update(float deltaTime)
{
	mWorld.SceneTime += deltaTime;
	mPlayerController.Update(mWorld, deltaTime);
    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
    {
        DestroyWindow(mMainWindow);
    }
}

void App::Render()
{
    EnvironmentSettings settings = 
        GetSceneSettings(mWorld.SceneTime);

    mRenderer.mImmediateContext->UpdateSubresource(
        mRenderer.mPerFrameConstantBuffer.Get(),
        0,
        nullptr,
        &settings.Lighting,
        0,
        0);


    mRenderer.ClearFrame(settings.ClearColor);

    mRenderer.BindRenderPipeline();

    XMVECTOR cameraPosition = XMLoadFloat3(&mWorld.MainCamera.Position);
    float aspectRatio = 
        static_cast<float>(mClientWidth) / static_cast<float>(mClientHeight);
    XMMATRIX viewProjection = 
        mWorld.MainCamera.BuildViewProjectionMatrix(aspectRatio);

    mWorldRenderer.DrawScene(viewProjection);

    mRenderer.Present();
}