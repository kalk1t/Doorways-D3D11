#pragma once

#include "RenderTypes.h"

#include <d3d11.h>
#include <wrl/client.h>
#include <vector>
#include <string>

struct ObjMaterialData
{
    std::string Name;
    std::string DiffuseTexturePath;

    DirectX::XMFLOAT4 DiffuseColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

    // x = specular red
    // y = specular green
    // z = specular blue
    // w = specular strength multiplier
    DirectX::XMFLOAT4 SpecularColor = DirectX::XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);

    // Higher value = smaller, sharper highlight.
    // Lower value = wider, softer highlight.
    float SpecularPower = 32.0f;

    // x = U tiling scale
    // y = V tiling scale
    // z/w reserved for future texture offset or extra material data
    DirectX::XMFLOAT4 TexTransform = DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f);

    // True when a custom Doorways material(.dmat) override provides texture scale.
    bool HasMaterialOverrideTexTransform = false;
};

struct SubmeshData
{
    unsigned int StartIndex = 0;
    unsigned int IndexCount = 0;
    int MaterialIndex = -1;
};

struct MeshData
{
    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;


    // Full OBJ material/submesh data.
    std::vector<ObjMaterialData> Materials;
    std::vector<SubmeshData> Submeshes;
};

struct GpuMaterial
{
    DirectX::XMFLOAT4 DiffuseColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

    // x = specular red
    // y = specular green
    // z = specular blue
    // w = specular strength multiplier
    DirectX::XMFLOAT4 SpecularColor = DirectX::XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);

    // Higher value = smaller, sharper highlight.
    // Lower value = wider, softer highlight.
    float SpecularPower = 32.0f;

    // x = U tiling scale
    // y = V tiling scale
    // z/w reserved for future texture offset or extra material data
    DirectX::XMFLOAT4 TexTransform = DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f);

    Microsoft::WRL::ComPtr<ID3D11Resource> DiffuseTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> DiffuseSRV;
};

struct GpuSubmesh
{
    unsigned int StartIndex = 0;
    unsigned int IndexCount = 0;
    int MaterialIndex = -1;
};

struct GpuMesh
{
    Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> IndexBuffer;

    unsigned int IndexCount = 0;

    std::vector<GpuMaterial> Materials;
    std::vector<GpuSubmesh> Submeshes;
};