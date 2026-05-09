#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>

#include "EnvironmentSettings.h"

struct Material
{
    XMFLOAT4 Diffuse;
    XMFLOAT4 TexTransform;
    ID3D11ShaderResourceView* DiffuseMap;
};

class Renderer
{
public:

    bool Initialize(HWND mMainWindow, int mClientWidth, int mClientHeight);
    void ClearFrame(XMFLOAT4 ClearColor);
    void Present();


    void BindRenderPipeline();

    void DrawBox(
        const XMMATRIX& world,
        const XMMATRIX& viewProjection,
        const Material& material);

    void DrawDoorLabel(
        const XMMATRIX& world,
        const XMMATRIX& viewProjection,
        const Material& material);




    bool BuildShaders();
	bool BuildGeometry();
	bool BuildSamplerState();
	bool BuildTextures();



    Microsoft::WRL::ComPtr<ID3D11Device> mDevice;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> mImmediateContext;
    Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mRenderTargetView;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> mDepthStencilBuffer;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mDepthStencilView;


    Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> mInputLayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer;



    Microsoft::WRL::ComPtr<ID3D11Buffer> mPerObjectConstantBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> mPerFrameConstantBuffer;


    Microsoft::WRL::ComPtr<ID3D11Texture2D> mDiffuseTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mDiffuseTextureView;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> mSunnyLabelTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mSunnyLabelTextureView;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> mRainyLabelTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mRainyLabelTextureView;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> mSnowyLabelTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mSnowyLabelTextureView;

    Microsoft::WRL::ComPtr<ID3D11SamplerState> mSamplerState;

    UINT mIndexCount = 0;
};