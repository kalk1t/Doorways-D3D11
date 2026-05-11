#include "RenderTypes.h"
#include "Renderer.h"

#include <d3dcompiler.h>
#include <cstddef>
#include <cstdint>
#include <DirectXMath.h>
#include <string>
#include <chrono>
#include <cmath>
#include <vector>

#include "WICTextureLoader.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;
using namespace std;


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



bool Renderer::Initialize(
    HWND mMainWindow, int mClientWidth, int mClientHeight)
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


void Renderer::ClearFrame(XMFLOAT4 ClearColor)
{
    

    mImmediateContext->ClearRenderTargetView(
        mRenderTargetView.Get(),
        reinterpret_cast<const float*>(&ClearColor));

    mImmediateContext->ClearDepthStencilView(
        mDepthStencilView.Get(),
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
        1.0f,
        0);
}

bool Renderer::BuildShaders()
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
        L"..\\..\\assets\\shaders\\Color.hlsl",
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
        L"..\\..\\assets\\shaders\\Color.hlsl",
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

bool Renderer::BuildGeometry()
{
    if (!BuildBoxGeometry())
        return false;
    if (!BuildSphereGeometry())
        return false;



    D3D11_BUFFER_DESC perObjectBufferDesc = {};

    perObjectBufferDesc.ByteWidth = sizeof(PerObjectConstantBuffer);
    perObjectBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    perObjectBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    perObjectBufferDesc.CPUAccessFlags = 0;
    perObjectBufferDesc.MiscFlags = 0;
    perObjectBufferDesc.StructureByteStride = 0;

    HRESULT hr = mDevice->CreateBuffer(
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



    return true;
}

bool Renderer::BuildBoxGeometry()
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

    mBoxIndexCount = static_cast<UINT>(sizeof(indices) / sizeof(indices[0]));

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
        mBoxVertexBuffer.GetAddressOf());

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
        mBoxIndexBuffer.GetAddressOf());

    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

bool Renderer::BuildSphereGeometry() {
    constexpr int sliceCount = 48;
    constexpr int stackCount = 24;
    constexpr float radius = 0.5f;

	vector <Vertex> vertices;
	vector <unsigned short> indices;

	vertices.reserve((sliceCount + 1) * (stackCount + 1));
	indices.reserve(sliceCount * stackCount * 6);

    for (int stack = 0; stack <= stackCount; ++stack)
    {
        float phi =
            XM_PI * static_cast<float>(stack) /
            static_cast<float>(stackCount);

        for (int slice = 0; slice <= sliceCount; ++slice)
        {
            float theta =
                XM_2PI * static_cast<float>(slice) /
                static_cast<float>(sliceCount);

            float x = radius * sinf(phi) * cosf(theta);
            float y = radius * cosf(phi);
            float z = radius * sinf(phi) * sinf(theta);

            XMFLOAT3 position(x, y, z);

            XMVECTOR normalVector = XMVector3Normalize(XMLoadFloat3(&position));

            XMFLOAT3 normal;
            XMStoreFloat3(&normal, normalVector);

            float u =
                static_cast<float>(slice) /
                static_cast<float>(sliceCount);

            float v =
                static_cast<float>(stack) /
                static_cast<float>(stackCount);

            Vertex vertex =
            {
                position,
                normal,
                XMFLOAT2(u, v)
            };

            vertices.push_back(vertex);
        }
    }

    for (int stack = 0; stack < stackCount; ++stack)
    {
        for (int slice = 0; slice < sliceCount; ++slice)
        {
            unsigned short a =
                static_cast<unsigned short>(
                    stack * (sliceCount + 1) + slice);

            unsigned short b =
                static_cast<unsigned short>(
                    stack * (sliceCount + 1) + slice + 1);

            unsigned short c =
                static_cast<unsigned short>(
                    (stack + 1) * (sliceCount + 1) + slice);

            unsigned short d =
                static_cast<unsigned short>(
                    (stack + 1) * (sliceCount + 1) + slice + 1);

            indices.push_back(a);
            indices.push_back(b);
            indices.push_back(c);

            indices.push_back(c);
            indices.push_back(b);
            indices.push_back(d);
        }
    }

	mSphereIndexCount = static_cast<UINT>(indices.size());

    D3D11_BUFFER_DESC vertexBufferDesc = {};

    vertexBufferDesc.ByteWidth = 
        static_cast<UINT>(vertices.size() * sizeof(Vertex));
    vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vertexInitData = {};

    vertexInitData.pSysMem = vertices.data();
    vertexInitData.SysMemPitch = 0;
    vertexInitData.SysMemSlicePitch = 0;

    HRESULT hr = mDevice->CreateBuffer(
        &vertexBufferDesc,
        &vertexInitData,
        mSphereVertexBuffer.GetAddressOf());

    if (FAILED(hr))
    {
        return false;
    }

    D3D11_BUFFER_DESC indexBufferDesc = {};

    indexBufferDesc.ByteWidth =
        static_cast<UINT>(indices.size() * sizeof(unsigned short));
    indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA indexInitData = {};

    indexInitData.pSysMem = indices.data();
    indexInitData.SysMemPitch = 0;
    indexInitData.SysMemSlicePitch = 0;

    hr = mDevice->CreateBuffer(
        &indexBufferDesc,
        &indexInitData,
        mSphereIndexBuffer.GetAddressOf());

    if (FAILED(hr))
    {
        return false;
    }

    return true;
}


bool CreateMoonGlowTexture(
    ID3D11Device* device,
    ID3D11Texture2D** outTexture,
    ID3D11ShaderResourceView** outTextureView)
{
    constexpr UINT textureWidth = 256;
    constexpr UINT textureHeight = 256;
    constexpr UINT bytesPerPixel = 4;

    std::uint8_t pixels[textureWidth * textureHeight * bytesPerPixel] = {};

    for (UINT y = 0; y < textureHeight; ++y)
    {
        for (UINT x = 0; x < textureWidth; ++x)
        {
            float u =
                (static_cast<float>(x) + 0.5f) /
                static_cast<float>(textureWidth);

            float v =
                (static_cast<float>(y) + 0.5f) /
                static_cast<float>(textureHeight);

            float dx = u * 2.0f - 1.0f;
            float dy = v * 2.0f - 1.0f;

            float distance = sqrtf(dx * dx + dy * dy);

            float alpha = 1.0f - distance;

            if (alpha < 0.0f)
            {
                alpha = 0.0f;
            }

            alpha = alpha * alpha;

            UINT index = (y * textureWidth + x) * bytesPerPixel;

            pixels[index + 0] = 180;
            pixels[index + 1] = 205;
            pixels[index + 2] = 255;
            pixels[index + 3] = static_cast<std::uint8_t>(alpha * 180.0f);
        }
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


bool Renderer::BuildTextures()
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


    //moontexture
	Microsoft::WRL::ComPtr<ID3D11Resource> moonTextureResource;

    //loads the JPH file from disk and create a GPU texture
    //moonTextureResource receives the generic Direct3D resource
	//mMoonTextureView receives the shader resource view
    //that is important because the pixel shader read textures through shader resource view
	hr = CreateWICTextureFromFile(mDevice.Get(),
        L"..\\..\\assets\\textures\\moon\\moon_color_2k.jpg",
        moonTextureResource.GetAddressOf(), mMoonTextureView.GetAddressOf());
    if (FAILED(hr))
    {
        return false;
    }

	//converts the generic resource into ID3D11Texture2D
    hr = moonTextureResource.As(&mMoonTexture);
    if (FAILED(hr))
    {
        return false;
    }

    if (!CreateMoonGlowTexture(
        mDevice.Get(),
        mMoonGlowTexture.GetAddressOf(),
        mMoonGlowTextureView.GetAddressOf()))
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

bool Renderer::BuildSamplerState()
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

void Renderer::BindRenderPipeline()
{
    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    mImmediateContext->IASetInputLayout(mInputLayout.Get());

    mImmediateContext->IASetVertexBuffers(
        0,
        1,
        mBoxVertexBuffer.GetAddressOf(),
        &stride,
        &offset);

    mImmediateContext->IASetIndexBuffer(
        mBoxIndexBuffer.Get(),
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

bool Renderer::BuildBlendStates()
{
    D3D11_BLEND_DESC alphaBlendDesc = {};

    alphaBlendDesc.AlphaToCoverageEnable = false;
    alphaBlendDesc.IndependentBlendEnable = false;

    alphaBlendDesc.RenderTarget[0].BlendEnable = true;
    alphaBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    alphaBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    alphaBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

    alphaBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    alphaBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    alphaBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

    alphaBlendDesc.RenderTarget[0].RenderTargetWriteMask =
        D3D11_COLOR_WRITE_ENABLE_ALL;

    HRESULT hr = mDevice->CreateBlendState(
        &alphaBlendDesc,
        mAlphaBlendState.GetAddressOf());

    if (FAILED(hr))
    {
        return false;
    }

    D3D11_BLEND_DESC opaqueBlendDesc = {};

    opaqueBlendDesc.AlphaToCoverageEnable = false;
    opaqueBlendDesc.IndependentBlendEnable = false;

    opaqueBlendDesc.RenderTarget[0].BlendEnable = false;
    opaqueBlendDesc.RenderTarget[0].RenderTargetWriteMask =
        D3D11_COLOR_WRITE_ENABLE_ALL;

    hr = mDevice->CreateBlendState(
        &opaqueBlendDesc,
        mOpaqueBlendState.GetAddressOf());

    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

void Renderer::SetAlphaBlendingEnabled(bool enabled)
{
    float blendFactor[4] =
    {
        0.0f,
        0.0f,
        0.0f,
        0.0f
    };

    UINT sampleMask = 0xffffffff;

    ID3D11BlendState* blendState = enabled
        ? mAlphaBlendState.Get()
        : mOpaqueBlendState.Get();

    mImmediateContext->OMSetBlendState(
        blendState,
        blendFactor,
        sampleMask);
}

void Renderer::DrawBox(
    const XMMATRIX& world,
    const XMMATRIX& viewProjection,
    const Material& material)
{

    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    ID3D11Buffer* vertexBuffers[] =
    {
        mBoxVertexBuffer.Get()
    };

    mImmediateContext->IASetVertexBuffers(
        0,
        1,
        vertexBuffers,
        &stride,
        &offset);

    mImmediateContext->IASetIndexBuffer(
        mBoxIndexBuffer.Get(),
        DXGI_FORMAT_R16_UINT,
        0);

    mImmediateContext->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


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
	perObjectData.EmissiveStrength = material.EmissiveStrength;

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
        mBoxIndexCount,
        0,
        0);
}

void Renderer::DrawSphere(
    const XMMATRIX& world,
    const XMMATRIX& viewProjection,
    const Material& material)
{
    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    mImmediateContext->IASetVertexBuffers(
        0,
        1,
        mSphereVertexBuffer.GetAddressOf(),
        &stride,
        &offset);

    mImmediateContext->IASetIndexBuffer(
        mSphereIndexBuffer.Get(),
        DXGI_FORMAT_R16_UINT,
        0);

    mImmediateContext->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

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
    perObjectData.EmissiveStrength = material.EmissiveStrength;

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
        mSphereIndexCount,
        0,
        0);
}

void Renderer::DrawDoorLabel(
    const XMMATRIX& world,
    const XMMATRIX& viewProjection,
    const Material& material)
{
    DrawBox(world, viewProjection, material);
}


void Renderer::Present()
{
    mSwapChain->Present(1, 0);
}