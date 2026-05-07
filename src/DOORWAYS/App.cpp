#include "App.h"

#include <d3dcompiler.h>
#include <cstddef>
#include <DirectXMath.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;

namespace
{
    const wchar_t* gWindowClassName = L"DoorwaysDemoWindowClass";

    struct Vertex
    {
		float Position[3];
		float Color[4];
    };

    struct ConstantBuffer
    {
        XMFLOAT4X4 WorldViewProj;
    };

    static_assert((sizeof(ConstantBuffer) % 16) == 0,
        "ConstantBuffer size must be a multiple of 16 bytes.");

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
            "COLOR",
            0,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            0,
            offsetof(Vertex, Color),
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
        // Back face corners, z = -0.5
        { { -0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
        { { -0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
        { {  0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
        { {  0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 0.0f, 1.0f } },

        // Front face corners, z = +0.5
        { { -0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f, 1.0f, 1.0f } },
        { { -0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
        { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f, 1.0f } },
        { {  0.5f, -0.5f,  0.5f }, { 0.2f, 0.2f, 0.2f, 1.0f } },
    };

    unsigned short indices[] =
    {
        // Front face
        0, 1, 2,
        0, 2, 3,

        // Back face
        4, 6, 5,
        4, 7, 6,

        // Left face
        4, 5, 1,
        4, 1, 0,

        // Right face
        3, 2, 6,
        3, 6, 7,

        // Top face
        1, 5, 6,
        1, 6, 2,

        // Bottom face
        4, 0, 3,
        4, 3, 7
    };

    mIndexCount = static_cast<UINT>(sizeof(indices) / sizeof(indices[0]));

    D3D11_BUFFER_DESC vertexBufferDesc = {};

    vertexBufferDesc.ByteWidth = sizeof(vertices);
    vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vertexInitData = {};

    vertexInitData.pSysMem = vertices;

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

    hr = mDevice->CreateBuffer(
        &indexBufferDesc,
        &indexInitData,
        mIndexBuffer.GetAddressOf());

    if (FAILED(hr))
    {
        return false;
    }

    D3D11_BUFFER_DESC constantBufferDesc = {};

    constantBufferDesc.ByteWidth = sizeof(ConstantBuffer);
    constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constantBufferDesc.CPUAccessFlags = 0;
    constantBufferDesc.MiscFlags = 0;
    constantBufferDesc.StructureByteStride = 0;

    hr = mDevice->CreateBuffer(
        &constantBufferDesc,
        nullptr,
        mConstantBuffer.GetAddressOf());

    if (FAILED(hr))
    {
        return false;
    }

    return true;
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

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	mImmediateContext->IASetInputLayout(mInputLayout.Get());

	mImmediateContext->IASetVertexBuffers(0, 1,
        mVertexBuffer.GetAddressOf(), &stride, &offset);

    mImmediateContext->IASetIndexBuffer(mIndexBuffer.Get(),
		DXGI_FORMAT_R16_UINT, 0);

    mImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	mImmediateContext->VSSetShader(mVertexShader.Get(), nullptr, 0);

	mImmediateContext->PSSetShader(mPixelShader.Get(), nullptr, 0);

	ID3D11Buffer* constantBuffers[] = { mConstantBuffer.Get() };

    mImmediateContext->VSSetConstantBuffers(0, 1, constantBuffers);


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

	XMVECTOR upDirection = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);


    XMMATRIX view = XMMatrixLookAtLH(
        cameraPosition,
        cameraTarget,
        upDirection);

    float aspectRatio = 
        static_cast<float>(mClientWidth)/static_cast<float>(mClientHeight);

    XMMATRIX projection = XMMatrixPerspectiveFovLH(
        0.25f * XM_PI,
        aspectRatio,
        0.1f,
        100.0f);

    XMMATRIX viewProjection = view * projection;

    auto DrawBox = [&](CXMMATRIX world)
        {
            ConstantBuffer constantBuffer = {};

            XMStoreFloat4x4(
                &constantBuffer.WorldViewProj,
                world * viewProjection);

            mImmediateContext->UpdateSubresource(
                mConstantBuffer.Get(),
                0,
                nullptr,
                &constantBuffer,
                0,
                0);

            mImmediateContext->DrawIndexed(
                mIndexCount,
                0,
                0);
        };

    // Floor
    XMMATRIX floorWorld =
        XMMatrixScaling(5.0f, 0.1f, 4.0f) *
        XMMatrixTranslation(0.0f, -0.55f, 0.0f);

    DrawBox(floorWorld);

    // Left door
    XMMATRIX leftDoorWorld =
        XMMatrixScaling(0.8f, 1.8f, 0.15f) *
        XMMatrixTranslation(-1.5f, 0.4f, 1.2f);

    DrawBox(leftDoorWorld);

    // Middle door
    XMMATRIX middleDoorWorld =
        XMMatrixScaling(0.8f, 1.8f, 0.15f) *
        XMMatrixTranslation(0.0f, 0.4f, 1.2f);

    DrawBox(middleDoorWorld);

    // Right door
    XMMATRIX rightDoorWorld =
        XMMatrixScaling(0.8f, 1.8f, 0.15f) *
        XMMatrixTranslation(1.5f, 0.4f, 1.2f);

    DrawBox(rightDoorWorld);



	mSwapChain->Present(1, 0);
}
