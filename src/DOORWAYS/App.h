#pragma once

#include <windows.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>


using namespace DirectX;

class App
{
public:
    App(HINSTANCE hInstance);
    bool Initialize();
    int Run();

private:
    struct Material
    {
        XMFLOAT4 Diffuse;
        XMFLOAT4 TexTransform;
        ID3D11ShaderResourceView* DiffuseMap;
    };
    enum class DoorId
    {
        None,
        Sunny,
        Rainy,
        Snowy
    };

    bool InitWindow();
    bool InitDirect3D();

    void Update(float deltaTime);
    void Render();

    bool BuildShaders();
	bool BuildGeometry();
	bool BuildTextures();
    bool BuildSamplerState();

    void ClearFrame();
    XMFLOAT4 GetEnvironmentClearColor() const;
    void BindRenderPipeline();

    XMMATRIX BuildViewProjectionMatrix() const;

    void UpdateLightingConstants();


    void DrawScene(const XMMATRIX& viewProjection);
    void DrawBox(
        const XMMATRIX& world,
        const XMMATRIX& viewProjection,
        const Material& material);
    void DrawDoorLabel(
        const XMMATRIX& world,
        const XMMATRIX& viewProjection,
        const Material& material);
    void DrawPlayer(const XMMATRIX& viewProjection);
    void DrawInteractionPrompt(const XMMATRIX& viewProjection);
    void DrawEnvironmentObjects(const XMMATRIX& viewProjection);
    void DrawEnvironmentBackdrop( const XMMATRIX& viewProjection,const XMFLOAT4& color);


    void DrawSunnyEnvironment(const XMMATRIX& viewProjection);
    void DrawRainyEnvironment(const XMMATRIX& viewProjection);
    void DrawSnowyEnvironment(const XMMATRIX& viewProjection);


    DoorId GetNearbyDoor() const;
    const wchar_t* GetDoorDisplayName(DoorId door) const;
	float GetDoorX(DoorId door) const;
    void UpdateDoorInteractionFeedback();
    void HandleDoorInteraction();
    void EnterEnvironment(DoorId door);
	void ResetToPorch();
    void UpdateWindowTitle();



private:
    HINSTANCE mAppInstance = nullptr;
    HWND mMainWindow = nullptr;

    int mClientWidth = 1280;
    int mClientHeight = 720;

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
	                            //x=center,y=above the ground,z=behin the objects
	XMFLOAT3 mCameraPosition = XMFLOAT3(0.0f, 2.0f, -6.0f); //eye position

	float mCameraYaw = 0.0f; //rotates around Y axis, left and right
	float mCameraPitch = -0.25f; //rotates around X axis, up and down.

	float mMoveSpeed = 0.05f; //how fast the camera moves in the scene
	float mTurnSpeed = 1.5f; //how fast the camera turns when the mouse moves

    XMFLOAT3 mPlayerPosition = XMFLOAT3(0.0f, -0.20f, -0.8f);

    float mPlayerYaw = 0.0f;
    float mPlayerMoveSpeed = 0.9f;

    DoorId mNearbyDoor = DoorId::None;
    DoorId mActiveDoor = DoorId::None;

    float mEnvironmentTime = 0.0f;

    bool mWasInteractKeyDown = false;
};