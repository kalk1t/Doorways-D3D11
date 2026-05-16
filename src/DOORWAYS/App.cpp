#include "App.h"
#include "RenderTypes.h"
#include "EnvironmentSettings.h"
#include "ImportedSceneSettings.h"

#include <d3dcompiler.h>
#include <cstddef>
#include <cstdint>
#include <DirectXMath.h>
#include <string>
#include <chrono>
#include <cmath>
#include <sstream>
#include <iomanip>

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

    void PrintImportedSceneTransform(
        const ImportedSceneSettings& sceneSettings)
    {
        OutputDebugStringA("\n");
        OutputDebugStringA("Imported scene transform:\n");

        OutputDebugStringA("Scale = XMFLOAT3(");
        OutputDebugStringA(std::to_string(sceneSettings.Scale.x).c_str());
        OutputDebugStringA("f, ");
        OutputDebugStringA(std::to_string(sceneSettings.Scale.y).c_str());
        OutputDebugStringA("f, ");
        OutputDebugStringA(std::to_string(sceneSettings.Scale.z).c_str());
        OutputDebugStringA("f);\n");

        OutputDebugStringA("Rotation = XMFLOAT3(");
        OutputDebugStringA(std::to_string(sceneSettings.Rotation.x).c_str());
        OutputDebugStringA("f, ");
        OutputDebugStringA(std::to_string(sceneSettings.Rotation.y).c_str());
        OutputDebugStringA("f, ");
        OutputDebugStringA(std::to_string(sceneSettings.Rotation.z).c_str());
        OutputDebugStringA("f);\n");

        OutputDebugStringA("Translation = XMFLOAT3(");
        OutputDebugStringA(std::to_string(sceneSettings.Translation.x).c_str());
        OutputDebugStringA("f, ");
        OutputDebugStringA(std::to_string(sceneSettings.Translation.y).c_str());
        OutputDebugStringA("f, ");
        OutputDebugStringA(std::to_string(sceneSettings.Translation.z).c_str());
        OutputDebugStringA("f);\n");

        OutputDebugStringA("\n");
    }

    std::wstring FormatFloatForTitle(float value)
    {
        std::wstringstream stream;
        stream << std::fixed << std::setprecision(2) << value;
        return stream.str();
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

    UpdateWindowTitle();

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
    const wchar_t* filterName =
        mRenderer.mUsePointSamplerForDebug
        ? L"Point Debug"
        : L"Anisotropic";

    const ImportedSceneSettings& sceneSettings =
        mWorld.PrimaryScene;

    std::wstring title = L"Doorways | Filter: ";
    title += filterName;

    title += L" | Pos: ";
    title += FormatFloatForTitle(sceneSettings.Translation.x);

    title += L", ";
    title += FormatFloatForTitle(sceneSettings.Translation.y);

    title += L", ";
    title += FormatFloatForTitle(sceneSettings.Translation.z);

    title += L" | Scale: ";
    title += FormatFloatForTitle(sceneSettings.Scale.x);

    title += L" | RotY: ";
    title += FormatFloatForTitle(sceneSettings.Rotation.y);

    SetWindowTextW(
        mMainWindow,
        title.c_str());
}

void App::Update(float deltaTime)
{

    /*

J / L = move scene left / right
U / O = move scene up / down
I / K = move scene toward temple / stairs
+ / - = scale scene bigger / smaller
Q / E = rotate scene around Y axis

Hold Shift = faster adjustment
Hold Ctrl  = slower fine adjustment

R = reset imported scene transform
P = print current transform to Output window
F = toggle texture filtering

    */






	mWorld.SceneTime += deltaTime;
	mPlayerController.Update(mWorld, deltaTime);

    if (mFilterToggleCooldown > 0.0f)
    {
        mFilterToggleCooldown -= deltaTime;

        if (mFilterToggleCooldown < 0.0f)
        {
            mFilterToggleCooldown = 0.0f;
        }
    }

    bool isFilterToggleKeyDown =
        (GetAsyncKeyState('F') & 0x8000) != 0;

    if (isFilterToggleKeyDown &&
        !mWasFilterToggleKeyDown &&
        mFilterToggleCooldown <= 0.0f)
    {
        mRenderer.mUsePointSamplerForDebug =
            !mRenderer.mUsePointSamplerForDebug;

        if (mRenderer.mUsePointSamplerForDebug)
        {
            OutputDebugStringA("Texture filtering: POINT debug sampler\n");
        }
        else
        {
            OutputDebugStringA("Texture filtering: ANISOTROPIC game sampler\n");
        }

        UpdateWindowTitle();

        mFilterToggleCooldown = 0.25f;
    }

    mWasFilterToggleKeyDown = isFilterToggleKeyDown;


	bool didSceneTransformChange = false;
// ------------------------------------------------------------
// Imported scene debug placement controls.
// These controls are temporary editor-style tools for tuning
// the imported Blender/OBJ scene position and scale.
// ------------------------------------------------------------

    ImportedSceneSettings& sceneSettings =
        mWorld.PrimaryScene;

    const float baseMoveSpeed = 4.0f;
    const float baseScaleSpeed = 0.75f;
    const float baseRotationSpeed = 1.25f;

    float speedMultiplier = 1.0f;

    // Hold Shift for faster scene placement.
    if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
    {
        speedMultiplier = 4.0f;
    }

    // Hold Ctrl for slower fine-tuning.
    if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
    {
        speedMultiplier = 0.25f;
    }

    const float moveAmount =
        baseMoveSpeed * speedMultiplier * deltaTime;

    const float scaleAmount =
        baseScaleSpeed * speedMultiplier * deltaTime;

    const float rotationAmount =
        baseRotationSpeed * speedMultiplier * deltaTime;

    // J / L = move left / right.
    if (GetAsyncKeyState('J') & 0x8000)
    {
        sceneSettings.Translation.x -= moveAmount;
        didSceneTransformChange = true;
    }

    if (GetAsyncKeyState('L') & 0x8000)
    {
        sceneSettings.Translation.x += moveAmount;
        didSceneTransformChange = true;
    }

    // U / O = move up / down.
    if (GetAsyncKeyState('U') & 0x8000)
    {
        sceneSettings.Translation.y += moveAmount;
        didSceneTransformChange = true;
    }

    if (GetAsyncKeyState('O') & 0x8000)
    {
        sceneSettings.Translation.y -= moveAmount;
        didSceneTransformChange = true;
    }

    // I / K = move toward temple / toward stairs.
    // In our world convention, -Z is temple side and +Z is stairs side.
    if (GetAsyncKeyState('I') & 0x8000)
    {
        sceneSettings.Translation.z -= moveAmount;
        didSceneTransformChange = true;
    }

    if (GetAsyncKeyState('K') & 0x8000)
    {
        sceneSettings.Translation.z += moveAmount;
        didSceneTransformChange = true;
    }

    // + / - = uniformly scale the imported scene.
    bool isScaleUpKeyDown =
        (GetAsyncKeyState(VK_OEM_PLUS) & 0x8000) ||
        (GetAsyncKeyState(VK_ADD) & 0x8000);

    bool isScaleDownKeyDown =
        (GetAsyncKeyState(VK_OEM_MINUS) & 0x8000) ||
        (GetAsyncKeyState(VK_SUBTRACT) & 0x8000);

    if (isScaleUpKeyDown)
    {
        sceneSettings.Scale.x += scaleAmount;
        sceneSettings.Scale.y += scaleAmount;
        sceneSettings.Scale.z += scaleAmount;
        didSceneTransformChange = true;
    }

    if (isScaleDownKeyDown)
    {
        sceneSettings.Scale.x -= scaleAmount;
        sceneSettings.Scale.y -= scaleAmount;
        sceneSettings.Scale.z -= scaleAmount;

        if (sceneSettings.Scale.x < 0.05f) sceneSettings.Scale.x = 0.05f;
        if (sceneSettings.Scale.y < 0.05f) sceneSettings.Scale.y = 0.05f;
        if (sceneSettings.Scale.z < 0.05f) sceneSettings.Scale.z = 0.05f;


        didSceneTransformChange = true;
    }

    // Q / E = rotate imported scene left / right around Y axis.
    if (GetAsyncKeyState('Q') & 0x8000)
    {
        sceneSettings.Rotation.y -= rotationAmount;
        didSceneTransformChange = true;
    }

    if (GetAsyncKeyState('E') & 0x8000)
    {
        sceneSettings.Rotation.y += rotationAmount;
        didSceneTransformChange = true;
    }

    // R = reset imported scene transform.
    bool isSceneResetKeyDown =
        (GetAsyncKeyState('R') & 0x8000) != 0;
        

    if (isSceneResetKeyDown && !mWasSceneResetKeyDown)
    {
        sceneSettings = ImportedSceneSettings();

        didSceneTransformChange = true;

        OutputDebugStringA("Imported scene transform reset.\n");
    }

    mWasSceneResetKeyDown = isSceneResetKeyDown;

    // P = print current imported scene transform to the Output window.
    bool isScenePrintKeyDown =
        (GetAsyncKeyState('P') & 0x8000) != 0;

    if (isScenePrintKeyDown && !mWasScenePrintKeyDown)
    {
        PrintImportedSceneTransform(sceneSettings);
    }

    mWasScenePrintKeyDown = isScenePrintKeyDown;


    if (didSceneTransformChange)
    {
        UpdateWindowTitle();
    }




    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
    {
        DestroyWindow(mMainWindow);
    }
}

void App::Render()
{
    EnvironmentSettings settings = 
        GetSceneSettings(mWorld.SceneTime);

    settings.Lighting.CameraPosition = XMFLOAT4(
        mWorld.MainCamera.Position.x,
        mWorld.MainCamera.Position.y,
        mWorld.MainCamera.Position.z,
        1.0f);

    mRenderer.mImmediateContext->UpdateSubresource(
        mRenderer.mPerFrameConstantBuffer.Get(),
        0,
        nullptr,
        &settings.Lighting,
        0,
        0);


    mRenderer.ClearFrame(settings.ClearColor);

    mRenderer.BindRenderPipeline();

    float aspectRatio = 
        static_cast<float>(mClientWidth) / static_cast<float>(mClientHeight);
    XMMATRIX viewProjection = 
        mWorld.MainCamera.BuildViewProjectionMatrix(aspectRatio);

    mWorldRenderer.DrawScene(viewProjection);

    mRenderer.Present();
}