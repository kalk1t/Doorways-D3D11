#pragma once

#include <windows.h>
#include <d3d11.h>
#include <wrl/client.h>

class App
{
public:
    App(HINSTANCE hInstance);
    bool Initialize();
    int Run();

private:
    bool InitWindow();
    bool InitDirect3D();
    void Render();

    bool BuildShaders();
	bool BuildGeometry();

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

};