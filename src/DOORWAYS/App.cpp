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

    struct DoorConfig
    {
        EnvironmentId Environment;
        float X;
        const char* LabelText;
    };
    constexpr DoorConfig gDoorConfigs[] =
    {
        { EnvironmentId::Sunny, -1.5f, "SUNNY" },
        { EnvironmentId::Rainy,  0.0f, "RAINY" },
        { EnvironmentId::Snowy,  1.5f, "SNOWY" }
    };


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
    mDoorSystem(this),
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
    std::wstring title = L"Doorways: ";

    if (mWorld.ActiveEnvironment == EnvironmentId::Porch)
    {
        title += L"Porch";
    }
    else
    {
        title += mDoorSystem.GetDoorDisplayName(mWorld.ActiveEnvironment);
        title += L" environment";
    }

    if (mWorld.NearbyDoor != EnvironmentId::Porch)
    {
        title += L" | Near ";
        title += mDoorSystem.GetDoorDisplayName(mWorld.NearbyDoor);
        title += L" door | Press E";
    }

    SetWindowTextW(
        mMainWindow,
        title.c_str());
}

void App::Update(float deltaTime)
{
    mWorld.EnvironmentTime += deltaTime;
    if (GetAsyncKeyState(VK_LEFT) & 0x8000)
    {
        mWorld.MainCamera.Yaw -= mWorld.MainCamera.TurnSpeed * deltaTime;
    }

    if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
    {
        mWorld.MainCamera.Yaw += mWorld.MainCamera.TurnSpeed * deltaTime;
    }

    if (GetAsyncKeyState(VK_UP) & 0x8000)
    {
        mWorld.MainCamera.Pitch += mWorld.MainCamera.TurnSpeed * deltaTime;
    }

    if (GetAsyncKeyState(VK_DOWN) & 0x8000)
    {
        mWorld.MainCamera.Pitch -= mWorld.MainCamera.TurnSpeed * deltaTime;
    }

    if(GetAsyncKeyState('R') & 0x8000)
    {
        mDoorSystem.ResetToPorch();
	}

    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
    {
        DestroyWindow(mMainWindow);
    }

    const float pitchLimit = 0.9f;

    if (mWorld.MainCamera.Pitch > pitchLimit)
    {
        mWorld.MainCamera.Pitch = pitchLimit;
    }

    if (mWorld.MainCamera.Pitch < -pitchLimit)
    {
        mWorld.MainCamera.Pitch = -pitchLimit;
    }

    XMFLOAT3 playerMovement = XMFLOAT3(0.0f, 0.0f, 0.0f);

    if (GetAsyncKeyState('W') & 0x8000)
    {
        playerMovement.z += 1.0f;
    }

    if (GetAsyncKeyState('S') & 0x8000)
    {
        playerMovement.z -= 1.0f;
    }

    if (GetAsyncKeyState('A') & 0x8000)
    {
        playerMovement.x -= 1.0f;
    }

    if (GetAsyncKeyState('D') & 0x8000)
    {
        playerMovement.x += 1.0f;
    }

    XMVECTOR movementVector = XMLoadFloat3(&playerMovement);

    if (XMVectorGetX(XMVector3LengthSq(movementVector)) > 0.0f)
    {
        mWorld.MainPlayer.Yaw = atan2f(playerMovement.x, playerMovement.z);

        movementVector = XMVector3Normalize(movementVector);
            
        movementVector *= mWorld.MainPlayer.MoveSpeed * deltaTime;
        
        XMVECTOR currentPosition = XMLoadFloat3(&mWorld.MainPlayer.Position);
        currentPosition += movementVector;

        XMStoreFloat3(&mWorld.MainPlayer.Position, currentPosition);
    }


    constexpr float porchMinX = -2.35f;
    constexpr float porchMaxX = 2.35f;
    constexpr float porchMinZ = -1.85f;
    constexpr float porchMaxZ = 1.85f;

    if (mWorld.MainPlayer.Position.x < porchMinX)
    {
        mWorld.MainPlayer.Position.x = porchMinX;
    }

    if (mWorld.MainPlayer.Position.x > porchMaxX)
    {
        mWorld.MainPlayer.Position.x = porchMaxX;
    }

    if (mWorld.MainPlayer.Position.z < porchMinZ)
    {
        mWorld.MainPlayer.Position.z = porchMinZ;
    }

    if (mWorld.MainPlayer.Position.z > porchMaxZ)
    {
        mWorld.MainPlayer.Position.z = porchMaxZ;
    }

    mDoorSystem.UpdateDoorInteractionFeedback();
    mDoorSystem.HandleDoorInteraction();
}

void App::Render()
{
    EnvironmentSettings settings = 
        GetEnvironmentSettings(mWorld.ActiveEnvironment, mWorld.EnvironmentTime);

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