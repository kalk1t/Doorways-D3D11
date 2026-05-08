#include "App.h"

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

    constexpr float gSunnyDoorX = -1.5f;
    constexpr float gRainyDoorX = 0.0f;
    constexpr float gSnowyDoorX = 1.5f;

    constexpr float gDoorY = 0.4f;
    constexpr float gDoorZ = 1.2f;

    constexpr float gLabelY = 1.55f;
    constexpr float gLabelZ = 1.05f;

    constexpr float gDoorWidth = 0.8f;
    constexpr float gDoorHeight = 1.8f;
    constexpr float gDoorDepth = 0.15f;

    constexpr float gLabelWidth = 1.0f;
    constexpr float gLabelHeight = 0.25f;
    constexpr float gLabelDepth = 0.05f;

    constexpr float gDoorInteractionHalfWidth = 0.55f;
    constexpr float gDoorInteractionDepth = 0.55f;


    struct Vertex
    {
		XMFLOAT3 Position;
		XMFLOAT3 Normal;

		XMFLOAT2 TexCoord;
    };

    struct PerObjectConstantBuffer
    {
        XMFLOAT4X4 WorldViewProj;
        XMFLOAT4X4 World;
        XMFLOAT4X4 WorldInvTranspose;
        XMFLOAT4 MaterialDiffuse;
        XMFLOAT4 TexTransform;
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

    XMMATRIX MakeWorld(
        float scaleX,
        float scaleY,
        float scaleZ,
        float translateX,
        float translateY,
        float translateZ)
    {
        return XMMatrixScaling(scaleX, scaleY, scaleZ) *
            XMMatrixTranslation(translateX, translateY, translateZ);
    }

    const char* const* GetGlyphRows(char c)
    {
        static const char* Blank[7] =
        {
            ".....",
            ".....",
            ".....",
            ".....",
            ".....",
            ".....",
            "....."
        };

        static const char* A[7] =
        {
            ".###.",
            "#...#",
            "#...#",
            "#####",
            "#...#",
            "#...#",
            "#...#"
        };

        static const char* I[7] =
        {
            "#####",
            "..#..",
            "..#..",
            "..#..",
            "..#..",
            "..#..",
            "#####"
        };

        static const char* N[7] =
        {
            "#...#",
            "##..#",
            "#.#.#",
            "#..##",
            "#...#",
            "#...#",
            "#...#"
        };

        static const char* O[7] =
        {
            ".###.",
            "#...#",
            "#...#",
            "#...#",
            "#...#",
            "#...#",
            ".###."
        };

        static const char* R[7] =
        {
            "####.",
            "#...#",
            "#...#",
            "####.",
            "#.#..",
            "#..#.",
            "#...#"
        };

        static const char* S[7] =
        {
            ".####",
            "#....",
            "#....",
            ".###.",
            "....#",
            "....#",
            "####."
        };

        static const char* U[7] =
        {
            "#...#",
            "#...#",
            "#...#",
            "#...#",
            "#...#",
            "#...#",
            ".###."
        };

        static const char* W[7] =
        {
            "#...#",
            "#...#",
            "#...#",
            "#.#.#",
            "#.#.#",
            "##.##",
            "#...#"
        };

        static const char* Y[7] =
        {
            "#...#",
            "#...#",
            ".#.#.",
            "..#..",
            "..#..",
            "..#..",
            "..#.."
        };

        switch (c)
        {
        case 'A': return A;
        case 'I': return I;
        case 'N': return N;
        case 'O': return O;
        case 'R': return R;
        case 'S': return S;
        case 'U': return U;
        case 'W': return W;
        case 'Y': return Y;
        default:  return Blank;
        }
    }

    bool CreateTextLabelTexture(
        ID3D11Device* device,
        const std::string& text,
        std::uint8_t bgR,
        std::uint8_t bgG,
        std::uint8_t bgB,
        std::uint8_t bgA,
        std::uint8_t textR,
        std::uint8_t textG,
        std::uint8_t textB,
        std::uint8_t textA,
        ID3D11Texture2D** outTexture,
        ID3D11ShaderResourceView** outTextureView)
    {
        constexpr UINT textureWidth = 128;
        constexpr UINT textureHeight = 32;
        constexpr UINT bytesPerPixel = 4;

        std::uint8_t pixels[textureWidth * textureHeight * bytesPerPixel] = {};

        for (UINT y = 0; y < textureHeight; ++y)
        {
            for (UINT x = 0; x < textureWidth; ++x)
            {
                UINT index = (y * textureWidth + x) * bytesPerPixel;

                pixels[index + 0] = bgR;
                pixels[index + 1] = bgG;
                pixels[index + 2] = bgB;
                pixels[index + 3] = bgA;
            }
        }

        constexpr int glyphWidth = 5;
        constexpr int glyphHeight = 7;
        constexpr int glyphSpacing = 1;
        constexpr int scale = 3;

        int textWidth = 0;

        if (!text.empty())
        {
            textWidth =
                static_cast<int>(text.size()) * glyphWidth * scale +
                static_cast<int>(text.size() - 1) * glyphSpacing * scale;
        }

        int textHeight = glyphHeight * scale;

        int startX = (static_cast<int>(textureWidth) - textWidth) / 2;
        int startY = (static_cast<int>(textureHeight) - textHeight) / 2;

        int cursorX = startX;

        for (char c : text)
        {
            const char* const* glyphRows = GetGlyphRows(c);

            for (int row = 0; row < glyphHeight; ++row)
            {
                for (int col = 0; col < glyphWidth; ++col)
                {
                    if (glyphRows[row][col] != '#')
                    {
                        continue;
                    }

                    for (int sy = 0; sy < scale; ++sy)
                    {
                        for (int sx = 0; sx < scale; ++sx)
                        {
                            int pixelX = cursorX + col * scale + sx;
                            int pixelY = startY + row * scale + sy;

                            if (pixelX < 0 ||
                                pixelX >= static_cast<int>(textureWidth) ||
                                pixelY < 0 ||
                                pixelY >= static_cast<int>(textureHeight))
                            {
                                continue;
                            }

                            UINT index =
                                (static_cast<UINT>(pixelY) * textureWidth +
                                    static_cast<UINT>(pixelX)) * bytesPerPixel;

                            pixels[index + 0] = textR;
                            pixels[index + 1] = textG;
                            pixels[index + 2] = textB;
                            pixels[index + 3] = textA;
                        }
                    }
                }
            }

            cursorX += (glyphWidth + glyphSpacing) * scale;
        }

        D3D11_TEXTURE2D_DESC textureDesc = {};

        textureDesc.Width = textureWidth;
        textureDesc.Height = textureHeight;
        textureDesc.MipLevels = 1;
        textureDesc.ArraySize = 1;
        textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

        textureDesc.SampleDesc.Count = 1;
        textureDesc.SampleDesc.Quality = 0;

        textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
        textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        textureDesc.CPUAccessFlags = 0;
        textureDesc.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA textureInitData = {};

        textureInitData.pSysMem = pixels;
        textureInitData.SysMemPitch = textureWidth * bytesPerPixel;
        textureInitData.SysMemSlicePitch = 0;

        HRESULT hr = device->CreateTexture2D(
            &textureDesc,
            &textureInitData,
            outTexture);

        if (FAILED(hr))
        {
            return false;
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

        srvDesc.Format = textureDesc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = 1;

        hr = device->CreateShaderResourceView(
            *outTexture,
            &srvDesc,
            outTextureView);

        if (FAILED(hr))
        {
            return false;
        }

        return true;
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

    if (!BuildTextures())
    {
        MessageBoxW(
            nullptr,
            L"Failed to build textures.",
            L"Initialization Error",
            MB_OK | MB_ICONERROR);

        return false;
    }

    if (!BuildSamplerState())
    {
        MessageBoxW(
            nullptr,
            L"Failed to build sampler state.",
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
        },
        {
            "TEXCOORD",
            0,
            DXGI_FORMAT_R32G32_FLOAT,
            0,
            offsetof(Vertex, TexCoord),
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        }
	};

    hr = mDevice->CreateInputLayout(
        inputLayoutDesc,
        3,
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
        { XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(0.5f,  0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },

        // Far face, z = +0.5
        { XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(0.5f,  0.5f,  0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(0.5f, -0.5f,  0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },

        // Left face, x = -0.5
        { XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },

        // Right face, x = +0.5
        { XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(0.5f,  0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(0.5f,  0.5f,  0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(0.5f, -0.5f,  0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },

        // Top face, y = +0.5
        { XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(-0.5f, 0.5f,  0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(0.5f, 0.5f,  0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },

        // Bottom face, y = -0.5
        { XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(0.5f, -0.5f,  0.5f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
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

bool App::BuildTextures()
{
    constexpr UINT textureWidth = 4;
    constexpr UINT textureHeight = 4;
    constexpr UINT bytesPerPixel = 4;

    const std::uint8_t pixels[textureWidth * textureHeight * bytesPerPixel] =
    {
        // Row 0
        220, 220, 220, 255,   80,  80,  80, 255,  220, 220, 220, 255,   80,  80,  80, 255,

        // Row 1
         80,  80,  80, 255,  220, 220, 220, 255,   80,  80,  80, 255,  220, 220, 220, 255,

         // Row 2
         220, 220, 220, 255,   80,  80,  80, 255,  220, 220, 220, 255,   80,  80,  80, 255,

         // Row 3
          80,  80,  80, 255,  220, 220, 220, 255,   80,  80,  80, 255,  220, 220, 220, 255
    };

    D3D11_TEXTURE2D_DESC textureDesc = {};

    textureDesc.Width = textureWidth;
    textureDesc.Height = textureHeight;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;

    textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA textureInitData = {};

    textureInitData.pSysMem = pixels;
    textureInitData.SysMemPitch = textureWidth * bytesPerPixel;
    textureInitData.SysMemSlicePitch = 0;

    HRESULT hr = mDevice->CreateTexture2D(
        &textureDesc,
        &textureInitData,
        mDiffuseTexture.GetAddressOf());

    if (FAILED(hr))
    {
        return false;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

    srvDesc.Format = textureDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    hr = mDevice->CreateShaderResourceView(
        mDiffuseTexture.Get(),
        &srvDesc,
        mDiffuseTextureView.GetAddressOf());

    if (FAILED(hr))
    {
        return false;
    }

    if (!CreateTextLabelTexture(
        mDevice.Get(),
        "SUNNY",
        245, 190, 45, 255,
        20, 20, 20, 255,
        mSunnyLabelTexture.GetAddressOf(),
        mSunnyLabelTextureView.GetAddressOf()))
    {
        return false;
    }

    if (!CreateTextLabelTexture(
        mDevice.Get(),
        "RAINY",
        45, 110, 220, 255,
        245, 245, 245, 255,
        mRainyLabelTexture.GetAddressOf(),
        mRainyLabelTextureView.GetAddressOf()))
    {
        return false;
    }

    if (!CreateTextLabelTexture(
        mDevice.Get(),
        "SNOWY",
        230, 245, 255, 255,
        20, 20, 20, 255,
        mSnowyLabelTexture.GetAddressOf(),
        mSnowyLabelTextureView.GetAddressOf()))
    {
        return false;
    }


    return true;
}

bool App::BuildSamplerState()
{
    D3D11_SAMPLER_DESC samplerDesc = {};

    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;

    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;

    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

    samplerDesc.BorderColor[0] = 0.0f;
    samplerDesc.BorderColor[1] = 0.0f;
    samplerDesc.BorderColor[2] = 0.0f;
    samplerDesc.BorderColor[3] = 0.0f;

    samplerDesc.MinLOD = 0.0f;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    HRESULT hr = mDevice->CreateSamplerState(
        &samplerDesc,
        mSamplerState.GetAddressOf());

    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

XMFLOAT4 App::GetEnvironmentClearColor() const
{
    switch (mActiveDoor)
    {
    case DoorId::Sunny:
        return XMFLOAT4(0.35f, 0.55f, 0.85f, 1.0f);

    case DoorId::Rainy:
        return XMFLOAT4(0.14f, 0.17f, 0.22f, 1.0f);

    case DoorId::Snowy:
        return XMFLOAT4(0.78f, 0.88f, 0.95f, 1.0f);

    default:
        return XMFLOAT4(0.05f, 0.08f, 0.12f, 1.0f);
    }
}

void App::ClearFrame()
{
    XMFLOAT4 environmentColor = GetEnvironmentClearColor();

    const float clearColor[4] =
    {
        environmentColor.x,
        environmentColor.y,
        environmentColor.z,
        environmentColor.w
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

    ID3D11SamplerState* samplers[] =
    {
        mSamplerState.Get()
    };

    mImmediateContext->PSSetSamplers(
        0,
        1,
        samplers);

   

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
    perObjectData.TexTransform = material.TexTransform;

    mImmediateContext->UpdateSubresource(
        mPerObjectConstantBuffer.Get(),
        0,
        nullptr,
        &perObjectData,
        0,
        0);

    ID3D11ShaderResourceView* shaderResources[] =
    {
        material.DiffuseMap
    };

    mImmediateContext->PSSetShaderResources(
        0,
        1,
        shaderResources);

    mImmediateContext->DrawIndexed(
        mIndexCount,
        0,
        0);
}

void App::DrawScene(const XMMATRIX& viewProjection)
{

    Material floorMaterial =
    {
        XMFLOAT4(0.45f, 0.35f, 0.25f, 1.0f),
        XMFLOAT4(6.0f, 4.0f, 0.0f, 0.0f),
        mDiffuseTextureView.Get()
    };

    Material leftDoorMaterial =
    {
        mActiveDoor == DoorId::Sunny
            ? XMFLOAT4(1.0f, 0.45f, 0.30f, 1.0f)
            : XMFLOAT4(0.55f, 0.15f, 0.12f, 1.0f),

        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        mDiffuseTextureView.Get()
    };

    Material middleDoorMaterial =
    {
        mActiveDoor == DoorId::Rainy
            ? XMFLOAT4(0.35f, 0.65f, 1.0f, 1.0f)
            : XMFLOAT4(0.12f, 0.32f, 0.65f, 1.0f),

        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        mDiffuseTextureView.Get()
    };

    Material rightDoorMaterial =
    {
        mActiveDoor == DoorId::Snowy
            ? XMFLOAT4(0.55f, 1.0f, 0.65f, 1.0f)
            : XMFLOAT4(0.15f, 0.50f, 0.25f, 1.0f),

        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        mDiffuseTextureView.Get()
    };


    Material sunnyLabelMaterial =
    {
        XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        mSunnyLabelTextureView.Get()
    };

    Material rainyLabelMaterial =
    {
        XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        mRainyLabelTextureView.Get()
    };

    Material snowyLabelMaterial =
    {
        XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        mSnowyLabelTextureView.Get()
    };

    XMMATRIX floorWorld = MakeWorld(
        5.0f, 0.1f, 4.0f,
        0.0f, -0.55f, 0.0f);

    DrawBox(floorWorld, viewProjection, floorMaterial);

    XMMATRIX leftDoorWorld = MakeWorld(
        gDoorWidth, gDoorHeight, gDoorDepth,
        gSunnyDoorX, gDoorY, gDoorZ);

    DrawBox(leftDoorWorld, viewProjection, leftDoorMaterial);

    XMMATRIX middleDoorWorld = MakeWorld(
        gDoorWidth, gDoorHeight, gDoorDepth,
        gRainyDoorX, gDoorY, gDoorZ);

    DrawBox(middleDoorWorld, viewProjection, middleDoorMaterial);

    XMMATRIX rightDoorWorld = MakeWorld(
        gDoorWidth, gDoorHeight, gDoorDepth,
        gSnowyDoorX, gDoorY, gDoorZ);

    DrawBox(rightDoorWorld, viewProjection, rightDoorMaterial);

    XMMATRIX sunnyLabelWorld = MakeWorld(
        gLabelWidth, gLabelHeight, gLabelDepth,
        gSunnyDoorX, gLabelY, gLabelZ);

    DrawDoorLabel(sunnyLabelWorld, viewProjection, sunnyLabelMaterial);

    XMMATRIX rainyLabelWorld = MakeWorld(
        gLabelWidth, gLabelHeight, gLabelDepth,
        gRainyDoorX, gLabelY, gLabelZ);

    DrawDoorLabel(rainyLabelWorld, viewProjection, rainyLabelMaterial);

    XMMATRIX snowyLabelWorld = MakeWorld(
        gLabelWidth, gLabelHeight, gLabelDepth,
        gSnowyDoorX, gLabelY, gLabelZ);

    DrawDoorLabel(snowyLabelWorld, viewProjection, snowyLabelMaterial);



    DrawPlayer(viewProjection);

	DrawInteractionPrompt(viewProjection);

	DrawEnvironmentObjects(viewProjection);


}

void App::DrawDoorLabel(
    const XMMATRIX& world,
    const XMMATRIX& viewProjection,
    const Material& material)
{
    DrawBox(world, viewProjection, material);
}

void App::DrawPlayer(const XMMATRIX& viewProjection)
{
    Material playerMaterial =
    {
        XMFLOAT4(0.95f, 0.85f, 0.20f, 1.0f),
        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        mDiffuseTextureView.Get()
    };

    XMMATRIX scale = XMMatrixScaling(
        0.30f,
        0.60f,
        0.45f);

    XMMATRIX rotation = XMMatrixRotationY(mPlayerYaw);

    XMMATRIX translation = XMMatrixTranslation(
        mPlayerPosition.x,
        mPlayerPosition.y,
        mPlayerPosition.z);

    XMMATRIX playerWorld = scale * rotation * translation;

    DrawBox(playerWorld, viewProjection, playerMaterial);
}

void App::DrawInteractionPrompt(const XMMATRIX& viewProjection)
{
    if (mNearbyDoor == DoorId::None)
    {
        return;
    }

	float promptX = GetDoorX(mNearbyDoor);

    Material promptMaterial =
    {
        XMFLOAT4(1.0f, 0.95f, 0.15f, 1.0f),
        XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
        mDiffuseTextureView.Get()
    };

    XMMATRIX promptWorld = MakeWorld(
        0.45f,
        0.08f,
        0.08f,
        promptX,
        1.95f,
        0.95f);

    DrawBox(
        promptWorld,
        viewProjection,
        promptMaterial);
}

void App::DrawEnvironmentObjects(const XMMATRIX& viewProjection)
{
    if (mActiveDoor == DoorId::None)
    {
        return;
    }

    if (mActiveDoor == DoorId::Sunny)
    {
        Material sunMaterial =
        {
            XMFLOAT4(1.0f, 0.85f, 0.10f, 1.0f),
            XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
            mDiffuseTextureView.Get()
        };

        XMMATRIX sunWorld = MakeWorld(
            0.45f,
            0.45f,
            0.08f,
            0.0f,
            2.45f,
            1.00f);

        DrawBox(
            sunWorld,
            viewProjection,
            sunMaterial);

        return;
    }

    if (mActiveDoor == DoorId::Rainy)
    {
        Material rainMaterial =
        {
            XMFLOAT4(0.25f, 0.55f, 1.0f, 1.0f),
            XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
            mDiffuseTextureView.Get()
        };

        XMMATRIX rainLeftWorld = MakeWorld(
            0.06f,
            0.50f,
            0.06f,
            -0.75f,
            2.30f,
            1.00f);

        XMMATRIX rainMiddleWorld = MakeWorld(
            0.06f,
            0.50f,
            0.06f,
            0.0f,
            2.30f,
            1.00f);

        XMMATRIX rainRightWorld = MakeWorld(
            0.06f,
            0.50f,
            0.06f,
            0.75f,
            2.30f,
            1.00f);

        DrawBox(rainLeftWorld, viewProjection, rainMaterial);
        DrawBox(rainMiddleWorld, viewProjection, rainMaterial);
        DrawBox(rainRightWorld, viewProjection, rainMaterial);

        return;
    }

    if (mActiveDoor == DoorId::Snowy)
    {
        Material snowMaterial =
        {
            XMFLOAT4(0.90f, 0.95f, 1.0f, 1.0f),
            XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f),
            mDiffuseTextureView.Get()
        };

        XMMATRIX snowLeftWorld = MakeWorld(
            0.75f,
            0.05f,
            0.45f,
            -1.0f,
            -0.46f,
            -0.75f);

        XMMATRIX snowMiddleWorld = MakeWorld(
            0.90f,
            0.05f,
            0.45f,
            0.0f,
            -0.45f,
            0.05f);

        XMMATRIX snowRightWorld = MakeWorld(
            0.75f,
            0.05f,
            0.45f,
            1.0f,
            -0.46f,
            -0.75f);

        DrawBox(snowLeftWorld, viewProjection, snowMaterial);
        DrawBox(snowMiddleWorld, viewProjection, snowMaterial);
        DrawBox(snowRightWorld, viewProjection, snowMaterial);

        return;
    }
}

App::DoorId App::GetNearbyDoor() const
{
    if (std::fabs(mPlayerPosition.z - gDoorZ) > gDoorInteractionDepth)
    {
        return DoorId::None;
    }

    if (std::fabs(mPlayerPosition.x - GetDoorX(DoorId::Sunny)) <= gDoorInteractionHalfWidth)
    {
        return DoorId::Sunny;
    }

    if (std::fabs(mPlayerPosition.x - GetDoorX(DoorId::Rainy)) <= gDoorInteractionHalfWidth)
    {
        return DoorId::Rainy;
    }

    if (std::fabs(mPlayerPosition.x - GetDoorX(DoorId::Snowy)) <= gDoorInteractionHalfWidth)
    {
        return DoorId::Snowy;
    }

    return DoorId::None;
}

float App::GetDoorX(DoorId door) const
{
    switch (door)
    {
    case DoorId::Sunny:
        return gSunnyDoorX;

    case DoorId::Rainy:
        return gRainyDoorX;

    case DoorId::Snowy:
        return gSnowyDoorX;

    default:
        return 0.0f;
    }
}

const wchar_t* App::GetDoorDisplayName(DoorId door) const
{
    switch (door)
    {
    case DoorId::Sunny:
        return L"SUNNY";

    case DoorId::Rainy:
        return L"RAINY";

    case DoorId::Snowy:
        return L"SNOWY";

    default:
        return L"NONE";
    }
}

void App::UpdateDoorInteractionFeedback()
{
    DoorId newNearbyDoor = GetNearbyDoor();

    if (newNearbyDoor == mNearbyDoor)
    {
        return;
    }

    mNearbyDoor = newNearbyDoor;

    if (mNearbyDoor == DoorId::None)
    {
        SetWindowTextW(
            mMainWindow,
            L"Doorways: Direct3D11 Demo");
    }
    else
    {
        std::wstring title = L"Doorways: Near ";
        title += GetDoorDisplayName(mNearbyDoor);
        title += L" door";

        SetWindowTextW(
            mMainWindow,
            title.c_str());
    }
}

void App::HandleDoorInteraction()
{
    bool isInteractKeyDown = (GetAsyncKeyState('E') & 0x8000) != 0;

    bool wasPressedThisFrame =
        isInteractKeyDown && !mWasInteractKeyDown;

    mWasInteractKeyDown = isInteractKeyDown;

    if (!wasPressedThisFrame)
    {
        return;
    }

    if (mNearbyDoor == DoorId::None)
    {
        SetWindowTextW(
            mMainWindow,
            L"Doorways: No door nearby");
        return;
    }

    mActiveDoor = mNearbyDoor;

    std::wstring title = L"Doorways: Entered ";
    title += GetDoorDisplayName(mActiveDoor);
    title += L" environment";

    SetWindowTextW(
        mMainWindow,
        title.c_str());
}

void App::Update(float deltaTime)
{
    
    if (GetAsyncKeyState(VK_LEFT) & 0x8000)
    {
        mCameraYaw -= mTurnSpeed * deltaTime;
    }

    if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
    {
        mCameraYaw += mTurnSpeed * deltaTime;
    }

    if (GetAsyncKeyState(VK_UP) & 0x8000)
    {
        mCameraPitch += mTurnSpeed * deltaTime;
    }

    if (GetAsyncKeyState(VK_DOWN) & 0x8000)
    {
        mCameraPitch -= mTurnSpeed * deltaTime;
    }

    if(GetAsyncKeyState('R') & 0x8000)
    {
        mCameraYaw = 0.0f;
        mCameraPitch = -0.25f;

        mPlayerPosition = XMFLOAT3(0.0f, -0.20f, -0.8f);
        mActiveDoor = DoorId::None;
	}

    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
    {
        DestroyWindow(mMainWindow);
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
        mPlayerYaw = atan2f(playerMovement.x, playerMovement.z);

        movementVector = XMVector3Normalize(movementVector);

        movementVector *= mPlayerMoveSpeed * deltaTime;

        XMVECTOR currentPosition = XMLoadFloat3(&mPlayerPosition);
        currentPosition += movementVector;

        XMStoreFloat3(&mPlayerPosition, currentPosition);
    }


    constexpr float porchMinX = -2.35f;
    constexpr float porchMaxX = 2.35f;
    constexpr float porchMinZ = -1.85f;
    constexpr float porchMaxZ = 1.85f;

    if (mPlayerPosition.x < porchMinX)
    {
        mPlayerPosition.x = porchMinX;
    }

    if (mPlayerPosition.x > porchMaxX)
    {
        mPlayerPosition.x = porchMaxX;
    }

    if (mPlayerPosition.z < porchMinZ)
    {
        mPlayerPosition.z = porchMinZ;
    }

    if (mPlayerPosition.z > porchMaxZ)
    {
        mPlayerPosition.z = porchMaxZ;
    }

    UpdateDoorInteractionFeedback();
	HandleDoorInteraction();
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