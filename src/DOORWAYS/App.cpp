#include "App.h"

#include <d3dcompiler.h>
#include <cstddef>
#include <DirectXMath.h>
#include <string>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;

namespace
{
    const wchar_t* gWindowClassName = L"DoorwaysDemoWindowClass";

    struct Vertex
    {
		XMFLOAT3 Position;
		XMFLOAT3 Normal;
    };

    struct PerObjectConstantBuffer
    {
        XMFLOAT4X4 WorldViewProj;
        XMFLOAT4X4 World;
        XMFLOAT4X4 WorldInvTranspose;
        XMFLOAT4 MaterialDiffuse;
    };

    struct PerFrameConstantBuffer
    {
        XMFLOAT4 LightDirection;
        XMFLOAT4 LightColor;
        XMFLOAT4 AmbientColor;
    };

    static_assert((sizeof(PerObjectConstantBuffer) % 16) == 0,
        "PerObjectConstantBuffer size must be a multiple of 16 bytes.");

    static_assert((sizeof(PerFrameConstantBuffer) % 16) == 0,
        "PerFrameConstantBuffer size must be a multiple of 16 bytes.");

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
    : mAppInstance(hInstance)
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

    if (!InitDirect3D())
    {
        MessageBoxW(
            nullptr,
            L"Failed to initialize Direct3D 11.",
            L"Initialization Error",
            MB_OK | MB_ICONERROR);

        return false;
    }

    if (!BuildShaders())
    {
        MessageBoxW(
            nullptr,
            L"Failed to build shaders.",
            L"Initialization Error",
            MB_OK | MB_ICONERROR);

        return false;
    }

    if (!BuildGeometry())
    {
        MessageBoxW(
            nullptr,
            L"Failed to build geometry.",
            L"Initialization Error",
            MB_OK | MB_ICONERROR);

        return false;
    }

    return true;
}

int App::Run()
{
    MSG msg = {};

    while (msg.message != WM_QUIT)
    {
        if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        else
        {
			Update();
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

bool App::InitDirect3D()
{
    UINT createDeviceFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0
    };

    D3D_FEATURE_LEVEL createdFeatureLevel = D3D_FEATURE_LEVEL_11_0;

    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};

    swapChainDesc.BufferDesc.Width = mClientWidth;
    swapChainDesc.BufferDesc.Height = mClientHeight;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 1;
    swapChainDesc.OutputWindow = mMainWindow;
    swapChainDesc.Windowed = true;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Flags = 0;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        createDeviceFlags,
        featureLevels,
        1,
        D3D11_SDK_VERSION,
        &swapChainDesc,
        mSwapChain.GetAddressOf(),
        mDevice.GetAddressOf(),
        &createdFeatureLevel,
        mImmediateContext.GetAddressOf());

#if defined(DEBUG) || defined(_DEBUG)
    if (FAILED(hr))
    {
        mSwapChain.Reset();
        mDevice.Reset();
        mImmediateContext.Reset();

        createDeviceFlags &= ~D3D11_CREATE_DEVICE_DEBUG;

        hr = D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            createDeviceFlags,
            featureLevels,
            1,
            D3D11_SDK_VERSION,
            &swapChainDesc,
            mSwapChain.GetAddressOf(),
            mDevice.GetAddressOf(),
            &createdFeatureLevel,
            mImmediateContext.GetAddressOf());
    }
#endif

    if (FAILED(hr))
    {
        return false;
    }


    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;

    hr = mSwapChain->GetBuffer(
        0,
        IID_PPV_ARGS(backBuffer.GetAddressOf()));

    if (FAILED(hr))
    {
        return false;
    }

    hr = mDevice->CreateRenderTargetView(
        backBuffer.Get(),
        nullptr,
        mRenderTargetView.GetAddressOf());

    if (FAILED(hr))
    {
        return false;
    }


    D3D11_TEXTURE2D_DESC depthStencilDesc = {};

    depthStencilDesc.Width = mClientWidth;
    depthStencilDesc.Height = mClientHeight;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;

    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;

    hr = mDevice->CreateTexture2D(
        &depthStencilDesc,
        nullptr,
        mDepthStencilBuffer.GetAddressOf());

    if (FAILED(hr))
    {
        return false;
    }

    hr = mDevice->CreateDepthStencilView(
        mDepthStencilBuffer.Get(),
        nullptr,
        mDepthStencilView.GetAddressOf());

    if (FAILED(hr))
    {
        return false;
    }


    ID3D11RenderTargetView* renderTargets[] =
    {
        mRenderTargetView.Get()
    };

    mImmediateContext->OMSetRenderTargets(
        1,
        renderTargets,
        mDepthStencilView.Get());


    D3D11_VIEWPORT viewport = {};

    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = static_cast<float>(mClientWidth);
    viewport.Height = static_cast<float>(mClientHeight);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    mImmediateContext->RSSetViewports(1, &viewport);

    return true;
}

bool App::BuildShaders()
{
    UINT compileFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
    compileFlags |= D3DCOMPILE_DEBUG;
    compileFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    Microsoft::WRL::ComPtr<ID3DBlob> vertexShaderBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> pixelShaderBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

    HRESULT hr = D3DCompileFromFile(
        L"Shaders\\Color.hlsl",
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "VS",
        "vs_5_0",
        compileFlags,
        0,
        vertexShaderBlob.GetAddressOf(),
        errorBlob.GetAddressOf());

    if (FAILED(hr))
    {
        if (errorBlob)
        {
            OutputDebugStringA(
                static_cast<const char*>(errorBlob->GetBufferPointer()));
        }

        return false;
    }

    hr = mDevice->CreateVertexShader(
        vertexShaderBlob->GetBufferPointer(),
        vertexShaderBlob->GetBufferSize(),
        nullptr,
        mVertexShader.GetAddressOf());

    if (FAILED(hr))
    {
        return false;
    }

    errorBlob.Reset();

    hr = D3DCompileFromFile(
        L"Shaders\\Color.hlsl",
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "PS",
        "ps_5_0",
        compileFlags,
        0,
        pixelShaderBlob.GetAddressOf(),
        errorBlob.GetAddressOf());

    if (FAILED(hr))
    {
        if (errorBlob)
        {
            OutputDebugStringA(
                static_cast<const char*>(errorBlob->GetBufferPointer()));
        }

        return false;
    }

    hr = mDevice->CreatePixelShader(
        pixelShaderBlob->GetBufferPointer(),
        pixelShaderBlob->GetBufferSize(),
        nullptr,
        mPixelShader.GetAddressOf());

    if (FAILED(hr))
    {
        return false;
    }

    D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[] =
    {
        {
            "POSITION",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            offsetof(Vertex, Position),
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        },
        {
            "NORMAL",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            offsetof(Vertex, Normal),
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        }
    };

    hr = mDevice->CreateInputLayout(
        inputLayoutDesc,
        2,
        vertexShaderBlob->GetBufferPointer(),
        vertexShaderBlob->GetBufferSize(),
        mInputLayout.GetAddressOf());

    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

bool App::BuildGeometry()
{
    Vertex vertices[] =
    {
        // Near face, z = -0.5
        { XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
        { XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
        { XMFLOAT3(0.5f,  0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
        { XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, -1.0f) },

        // Far face, z = +0.5
        { XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
        { XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
        { XMFLOAT3(0.5f,  0.5f,  0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
        { XMFLOAT3(0.5f, -0.5f,  0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f) },

        // Left face, x = -0.5
        { XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
        { XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
        { XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
        { XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },

        // Right face, x = +0.5
        { XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
        { XMFLOAT3(0.5f,  0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
        { XMFLOAT3(0.5f,  0.5f,  0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
        { XMFLOAT3(0.5f, -0.5f,  0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f) },

        // Top face, y = +0.5
        { XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
        { XMFLOAT3(-0.5f, 0.5f,  0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
        { XMFLOAT3(0.5f, 0.5f,  0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
        { XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f) },

        // Bottom face, y = -0.5
        { XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
        { XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
        { XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
        { XMFLOAT3(0.5f, -0.5f,  0.5f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
    };

    unsigned short indices[] =
    {
        // Near face
        0, 1, 2,
        0, 2, 3,

        // Far face
        4, 6, 5,
        4, 7, 6,

        // Left face
        8, 9, 10,
        8, 10, 11,

        // Right face
        12, 13, 14,
        12, 14, 15,

        // Top face
        16, 17, 18,
        16, 18, 19,

        // Bottom face
        20, 21, 22,
        20, 22, 23
    };

    mIndexCount = static_cast<UINT>(sizeof(indices) / sizeof(indices[0]));
    
    /*
    wchar_t msg[128];
    swprintf_s(msg, L"Index count = %u", mIndexCount);
    MessageBoxW(nullptr, msg, L"Debug", MB_OK);
    */

    D3D11_BUFFER_DESC vertexBufferDesc = {};

    vertexBufferDesc.ByteWidth = sizeof(vertices);
    vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vertexInitData = {};

    vertexInitData.pSysMem = vertices;
    vertexInitData.SysMemPitch = 0;
    vertexInitData.SysMemSlicePitch = 0;

    HRESULT hr = mDevice->CreateBuffer(
        &vertexBufferDesc,
        &vertexInitData,
        mVertexBuffer.GetAddressOf());

    if (FAILED(hr))
    {
        return false;
    }

    D3D11_BUFFER_DESC indexBufferDesc = {};

    indexBufferDesc.ByteWidth = sizeof(indices);
    indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA indexInitData = {};

    indexInitData.pSysMem = indices;
    indexInitData.SysMemPitch = 0;
    indexInitData.SysMemSlicePitch = 0;

    hr = mDevice->CreateBuffer(
        &indexBufferDesc,
        &indexInitData,
        mIndexBuffer.GetAddressOf());

    if (FAILED(hr))
    {
        return false;
    }

    D3D11_BUFFER_DESC perObjectBufferDesc = {};

    perObjectBufferDesc.ByteWidth = sizeof(PerObjectConstantBuffer);
    perObjectBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    perObjectBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    perObjectBufferDesc.CPUAccessFlags = 0;
    perObjectBufferDesc.MiscFlags = 0;
    perObjectBufferDesc.StructureByteStride = 0;

    hr = mDevice->CreateBuffer(
        &perObjectBufferDesc,
        nullptr,
        mPerObjectConstantBuffer.GetAddressOf());

    if (FAILED(hr))
    {
        return false;
    }

    D3D11_BUFFER_DESC perFrameBufferDesc = {};

    perFrameBufferDesc.ByteWidth = sizeof(PerFrameConstantBuffer);
    perFrameBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    perFrameBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    perFrameBufferDesc.CPUAccessFlags = 0;
    perFrameBufferDesc.MiscFlags = 0;
    perFrameBufferDesc.StructureByteStride = 0;

    hr = mDevice->CreateBuffer(
        &perFrameBufferDesc,
        nullptr,
        mPerFrameConstantBuffer.GetAddressOf());

    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

void App::ClearFrame()
{
    const float clearColor[4] =
    {
        0.05f,
        0.08f,
        0.12f,
        1.0f
    };

    mImmediateContext->ClearRenderTargetView(
        mRenderTargetView.Get(),
        clearColor);

    mImmediateContext->ClearDepthStencilView(
        mDepthStencilView.Get(),
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
        1.0f,
        0);
}

void App::BindRenderPipeline()
{
    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    mImmediateContext->IASetInputLayout(mInputLayout.Get());

    mImmediateContext->IASetVertexBuffers(
        0,
        1,
        mVertexBuffer.GetAddressOf(),
        &stride,
        &offset);

    mImmediateContext->IASetIndexBuffer(
        mIndexBuffer.Get(),
        DXGI_FORMAT_R16_UINT,
        0);

    mImmediateContext->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    mImmediateContext->VSSetShader(
        mVertexShader.Get(),
        nullptr,
        0);

    mImmediateContext->PSSetShader(
        mPixelShader.Get(),
        nullptr,
        0);


    ID3D11Buffer* perObjectBuffers[] =
    {
        mPerObjectConstantBuffer.Get()
    };

    mImmediateContext->VSSetConstantBuffers(
        0,
        1,
        perObjectBuffers);

    mImmediateContext->PSSetConstantBuffers(
        0,
        1,
        perObjectBuffers);

    ID3D11Buffer* perFrameBuffers[] =
    {
        mPerFrameConstantBuffer.Get()
    };

    mImmediateContext->PSSetConstantBuffers(
        1,
        1,
        perFrameBuffers);
}

void App::UpdateLightingConstants()
{
    PerFrameConstantBuffer perFrameData = {};

    perFrameData.LightDirection = XMFLOAT4(0.577f, -0.577f, 0.577f, 0.0f);
    perFrameData.LightColor = XMFLOAT4(0.85f, 0.85f, 0.80f, 1.0f);
    perFrameData.AmbientColor = XMFLOAT4(0.20f, 0.20f, 0.25f, 1.0f);

    mImmediateContext->UpdateSubresource(
        mPerFrameConstantBuffer.Get(),
        0,
        nullptr,
        &perFrameData,
        0,
        0);
}

XMMATRIX App::BuildViewProjectionMatrix() const
{
    XMVECTOR cameraPosition = XMLoadFloat3(&mCameraPosition);

    float cosYaw = cosf(mCameraYaw);
    float sinYaw = sinf(mCameraYaw);
    float cosPitch = cosf(mCameraPitch);
    float sinPitch = sinf(mCameraPitch);

    XMVECTOR lookDirection = XMVectorSet(
        cosPitch * sinYaw,
        sinPitch,
        cosPitch * cosYaw,
        0.0f);

    XMVECTOR cameraTarget = cameraPosition + lookDirection;

    XMVECTOR upDirection = XMVectorSet(
        0.0f,
        1.0f,
        0.0f,
        0.0f);

    XMMATRIX view = XMMatrixLookAtLH(
        cameraPosition,
        cameraTarget,
        upDirection);

    float aspectRatio =
        static_cast<float>(mClientWidth) /
        static_cast<float>(mClientHeight);

    XMMATRIX projection = XMMatrixPerspectiveFovLH(
        0.25f * XM_PI,
        aspectRatio,
        0.1f,
        100.0f);

    return view * projection;
}

void App::DrawBox(
    const XMMATRIX& world,
    const XMMATRIX& viewProjection,
    const Material& material)
{
    PerObjectConstantBuffer perObjectData = {};

    XMMATRIX worldViewProjection = world * viewProjection;

    XMMATRIX worldInverseTranspose =
        XMMatrixTranspose(XMMatrixInverse(nullptr, world));

    XMStoreFloat4x4(
        &perObjectData.WorldViewProj,
        worldViewProjection);

    XMStoreFloat4x4(
        &perObjectData.World,
        world);

    XMStoreFloat4x4(
        &perObjectData.WorldInvTranspose,
        worldInverseTranspose);

    perObjectData.MaterialDiffuse = material.Diffuse;

    mImmediateContext->UpdateSubresource(
        mPerObjectConstantBuffer.Get(),
        0,
        nullptr,
        &perObjectData,
        0,
        0);

    mImmediateContext->DrawIndexed(
        mIndexCount,
        0,
        0);
}

void App::DrawScene(const XMMATRIX& viewProjection)
{
    Material floorMaterial =
    {
        XMFLOAT4(0.45f, 0.35f, 0.25f, 1.0f)
    };

    Material leftDoorMaterial =
    {
        XMFLOAT4(0.55f, 0.15f, 0.12f, 1.0f)
    };

    Material middleDoorMaterial =
    {
        XMFLOAT4(0.12f, 0.32f, 0.65f, 1.0f)
    };

    Material rightDoorMaterial =
    {
        XMFLOAT4(0.15f, 0.50f, 0.25f, 1.0f)
    };

    XMMATRIX floorWorld =
        XMMatrixScaling(5.0f, 0.1f, 4.0f) *
        XMMatrixTranslation(0.0f, -0.55f, 0.0f);

    DrawBox(floorWorld, viewProjection, floorMaterial);

    XMMATRIX leftDoorWorld =
        XMMatrixScaling(0.8f, 1.8f, 0.15f) *
        XMMatrixTranslation(-1.5f, 0.4f, 1.2f);

    DrawBox(leftDoorWorld, viewProjection, leftDoorMaterial);

    XMMATRIX middleDoorWorld =
        XMMatrixScaling(0.8f, 1.8f, 0.15f) *
        XMMatrixTranslation(0.0f, 0.4f, 1.2f);

    DrawBox(middleDoorWorld, viewProjection, middleDoorMaterial);

    XMMATRIX rightDoorWorld =
        XMMatrixScaling(0.8f, 1.8f, 0.15f) *
        XMMatrixTranslation(1.5f, 0.4f, 1.2f);

    DrawBox(rightDoorWorld, viewProjection, rightDoorMaterial);
}

void App::Update()
{

    if(GetAsyncKeyState(VK_ESCAPE) & 0x8000)
    {
        DestroyWindow(mMainWindow);
	}

    if (GetAsyncKeyState('A') & 0x8000 || GetAsyncKeyState(VK_LEFT) & 0x8000)
    {
        mCameraYaw -= mTurnSpeed;
    }

    if (GetAsyncKeyState('D') & 0x8000 || GetAsyncKeyState(VK_RIGHT) & 0x8000)
    {
        mCameraYaw += mTurnSpeed;
    }

    if (GetAsyncKeyState('W') & 0x8000 || GetAsyncKeyState(VK_UP) & 0x8000  )
    {
        mCameraPitch += mTurnSpeed;
    }

    if (GetAsyncKeyState('S') & 0x8000 || GetAsyncKeyState(VK_DOWN) & 0x8000)
    {
        mCameraPitch -= mTurnSpeed;
    }

    if(GetAsyncKeyState('R') & 0x8000)
    {
        mCameraYaw = 0.0f;
        mCameraPitch = -0.25f;
	}


    const float pitchLimit = 0.9f;

    if (mCameraPitch > pitchLimit)
    {
        mCameraPitch = pitchLimit;
    }

    if (mCameraPitch < -pitchLimit)
    {
        mCameraPitch = -pitchLimit;
    }


}

void App::Render()
{
    ClearFrame();

    BindRenderPipeline();

	UpdateLightingConstants();

    XMMATRIX viewProjection = BuildViewProjectionMatrix();

    DrawScene(viewProjection);

    mSwapChain->Present(1, 0);
}