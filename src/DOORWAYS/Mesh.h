#pragma once
#pragma once

#include "RenderTypes.h"

#include <d3d11.h>
#include <wrl/client.h>
#include <vector>
#include <string>

struct MeshData
{
    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;


    // First simple material support.
    // For now we support one diffuse texture for the whole OBJ.
    std::string DiffuseTexturePath;
};

struct GpuMesh
{
    Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> IndexBuffer;

    unsigned int IndexCount = 0;
};