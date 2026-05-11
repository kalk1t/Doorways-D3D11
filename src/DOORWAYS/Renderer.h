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

    float EmissiveStrength = 0.0f;
};

class Renderer
{
public:

    bool Initialize(HWND mMainWindow, int mClientWidth, int mClientHeight);
    void ClearFrame(XMFLOAT4 ClearColor);
    void Present();


    bool BuildBlendStates();
	void SetAlphaBlendingEnabled(bool enabled);
    void BindRenderPipeline();

    void DrawBox(
        const XMMATRIX& world,
        const XMMATRIX& viewProjection,
        const Material& material);

    void DrawDoorLabel(
        const XMMATRIX& world,
        const XMMATRIX& viewProjection,
        const Material& material);

    void DrawSphere(
        const XMMATRIX& world,
        const XMMATRIX& viewProjection,
		const Material& material);


    bool BuildShaders();
	bool BuildGeometry();
    bool BuildBoxGeometry();
    bool BuildSphereGeometry();

	bool BuildSamplerState();
	bool BuildTextures();

    Microsoft::WRL::ComPtr<ID3D11BlendState> mAlphaBlendState;
    Microsoft::WRL::ComPtr<ID3D11BlendState> mOpaqueBlendState;

    Microsoft::WRL::ComPtr<ID3D11Device> mDevice;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> mImmediateContext;
    Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mRenderTargetView;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> mDepthStencilBuffer;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mDepthStencilView;


    Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> mInputLayout;


    Microsoft::WRL::ComPtr<ID3D11Buffer> mBoxVertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> mBoxIndexBuffer;

    Microsoft::WRL::ComPtr<ID3D11Buffer> mSphereVertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> mSphereIndexBuffer;



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

	//Textures for .jpg loading
    Microsoft::WRL::ComPtr<ID3D11Texture2D> mMoonTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mMoonTextureView;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> mMoonGlowTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mMoonGlowTextureView;



    UINT mBoxIndexCount = 0;
    UINT mSphereIndexCount = 0;
};