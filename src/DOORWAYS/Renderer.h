#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>

#include "EnvironmentSettings.h"
#include "Mesh.h"

struct Material
{
    XMFLOAT4 Diffuse;
    XMFLOAT4 TexTransform;
    ID3D11ShaderResourceView* DiffuseMap;

    // xyz = specular color
    // w   = specular strength multiplier
    XMFLOAT4 SpecularColor = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);

    float SpecularPower = 32.0f;
    float EmissiveStrength = 0.0f;
};

class Renderer
{
public:

    bool Initialize(HWND mMainWindow, int mClientWidth, int mClientHeight);
    void ClearFrame(XMFLOAT4 ClearColor);
    void Present();



    bool CreateGpuMesh(
        const MeshData& meshData,
        GpuMesh& outMesh);

    void DrawMesh(
        const GpuMesh& mesh,
        const XMMATRIX& world,
        const XMMATRIX& viewProjection,
        const Material& material);

    bool BuildAssetMeshes();
    bool LoadStaticMesh(
        const char* filePath,
        GpuMesh& outMesh);


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
    bool BuildConstantBuffers();
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


    UINT mBoxIndexCount = 0;
    UINT mSphereIndexCount = 0;
    UINT mMountainIndexCount = 0;

    Microsoft::WRL::ComPtr<ID3D11Buffer> mPerObjectConstantBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> mPerFrameConstantBuffer;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> mWhiteTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mWhiteTextureView;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> mDiffuseTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mDiffuseTextureView;

    // Main texture sampler used by the game.
    Microsoft::WRL::ComPtr<ID3D11SamplerState> mAnisotropicSamplerState;

    // Debug sampler used to compare against old point filtering.
    Microsoft::WRL::ComPtr<ID3D11SamplerState> mPointSamplerState;

    // When true, the renderer uses point filtering so we can compare
    // old blocky texture filtering against the new anisotropic filtering.
    bool mUsePointSamplerForDebug = false;

	//Textures for .jpg loading
    //
    //
    //SRV - Shader Resource View
    // 
    // 
    
	//soft glow
    Microsoft::WRL::ComPtr<ID3D11Texture2D> mSoftGlowTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mSoftGlowSRV;

    //moon
    Microsoft::WRL::ComPtr<ID3D11Texture2D> mMoonTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mMoonSRV;

    //night sky
    Microsoft::WRL::ComPtr<ID3D11Texture2D> mStarSkyTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mStarSkySRV;

    
	GpuMesh mPrimarySceneMesh;
};