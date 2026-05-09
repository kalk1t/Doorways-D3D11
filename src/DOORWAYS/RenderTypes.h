#pragma once

#include <DirectXMath.h>

using namespace DirectX;

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