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

    XMFLOAT4 DiffuseColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
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


    // First simple material support.
    // For now we support one diffuse texture for the whole OBJ.
    std::string DiffuseTexturePath;

    // Full OBJ material/submesh data.
    std::vector<ObjMaterialData> Materials;
    std::vector<SubmeshData> Submeshes;
};

struct GpuMaterial
{
    XMFLOAT4 DiffuseColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

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