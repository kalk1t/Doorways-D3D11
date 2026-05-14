#pragma once

#include "Mesh.h"

#include <string>

class ObjLoader
{
public:
    static bool LoadFromFile(
        const std::string& filePath,
        MeshData& outMeshData);
};
