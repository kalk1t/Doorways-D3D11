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
    bool InitWindow();
    bool InitDirect3D();

    void Update();
    void Render();

    bool BuildShaders();
	bool BuildGeometry();

    void ClearFrame();
    void BindRenderPipeline();

    XMMATRIX BuildViewProjectionMatrix() const;

    void DrawScene(const XMMATRIX& viewProjection);
    void DrawBox(const XMMATRIX& world, const XMMATRIX& viewProjection);

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
    Microsoft::WRL::ComPtr<ID3D11Buffer> mConstantBuffer;

    UINT mIndexCount = 0;
	                            //x=center,y=above the ground,z=behin the objects
	XMFLOAT3 mCameraPosition = XMFLOAT3(0.0f, 2.0f, -6.0f); //eye position

	float mCameraYaw = 0.0f; //rotates around Y axis, left and right
	float mCameraPitch = -0.25f; //rotates around X axis, up and down.

	float mMoveSpeed = 0.05f; //how fast the camera moves in the scene
	float mTurnSpeed = 0.02f; //how fast the camera turns when the mouse moves

};