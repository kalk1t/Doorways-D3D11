#include "ObjLoader.h"

#include <DirectXMath.h>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>

using namespace DirectX;

namespace
{
    struct ObjVertexKey
    {
        int PositionIndex = 0;
        int TexCoordIndex = 0;
        int NormalIndex = 0;
    };

    bool ParseObjVertexToken(
        const std::string& token,
        ObjVertexKey& outKey)
    {
        outKey = {};

        std::string positionPart;
        std::string texCoordPart;
        std::string normalPart;

        size_t firstSlash = token.find('/');

        if (firstSlash == std::string::npos)
        {
            positionPart = token;
        }
        else
        {
            positionPart = token.substr(0, firstSlash);

            size_t secondSlash = token.find('/', firstSlash + 1);

            if (secondSlash == std::string::npos)
            {
                texCoordPart = token.substr(firstSlash + 1);
            }
            else
            {
                texCoordPart = token.substr(
                    firstSlash + 1,
                    secondSlash - firstSlash - 1);

                normalPart = token.substr(secondSlash + 1);
            }
        }

        if (positionPart.empty())
        {
            return false;
        }

        outKey.PositionIndex = std::stoi(positionPart);

        if (!texCoordPart.empty())
        {
            outKey.TexCoordIndex = std::stoi(texCoordPart);
        }

        if (!normalPart.empty())
        {
            outKey.NormalIndex = std::stoi(normalPart);
        }

        return true;
    }

    bool IsValidPositiveIndex(int index, size_t count)
    {
        return index > 0 && static_cast<size_t>(index) <= count;
    }

    std::string TrimWhitespace(const std::string& text)
    {
        const size_t first = text.find_first_not_of(" \t\r\n");

        if (first == std::string::npos)
        {
            return "";
        }

        const size_t last = text.find_last_not_of(" \t\r\n");

        return text.substr(first, last - first + 1);
    }

    bool LoadMaterialsFromMtl(
        const std::filesystem::path& mtlPath,
        std::vector<ObjMaterialData>& outMaterials)
    {
        outMaterials.clear();

        std::ifstream mtlFile(mtlPath);

        if (!mtlFile.is_open())
        {
            OutputDebugStringA("Failed to open MTL file:\n");
            OutputDebugStringA(mtlPath.string().c_str());
            OutputDebugStringA("\n");

            return false;
        }

        ObjMaterialData* currentMaterial = nullptr;

        std::string line;

        while (std::getline(mtlFile, line))
        {
            line = TrimWhitespace(line);

            if (line.empty())
            {
                continue;
            }

            if (line[0] == '#')
            {
                continue;
            }

            std::istringstream lineStream(line);

            std::string command;
            lineStream >> command;

            if (command == "newmtl")
            {
                std::string materialName;
                std::getline(lineStream, materialName);

                materialName = TrimWhitespace(materialName);

                if (materialName.empty())
                {
                    continue;
                }

                ObjMaterialData material = {};
                material.Name = materialName;
                material.DiffuseColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

                outMaterials.push_back(material);
                currentMaterial = &outMaterials.back();
            }
            else if (command == "Kd")
            {
                if (currentMaterial == nullptr)
                {
                    continue;
                }

                float r = 1.0f;
                float g = 1.0f;
                float b = 1.0f;

                if (lineStream >> r >> g >> b)
                {
                    currentMaterial->DiffuseColor = XMFLOAT4(r, g, b, 1.0f);
                }
            }
            else if (command == "map_Kd")
            {
                if (currentMaterial == nullptr)
                {
                    continue;
                }

                std::string texturePathText;
                std::getline(lineStream, texturePathText);

                texturePathText = TrimWhitespace(texturePathText);

                if (texturePathText.empty())
                {
                    continue;
                }

                std::filesystem::path texturePath = texturePathText;

                if (texturePath.is_relative())
                {
                    texturePath = mtlPath.parent_path() / texturePath;
                }

                texturePath = texturePath.lexically_normal();

                currentMaterial->DiffuseTexturePath = texturePath.string();
            }
        }

        return !outMaterials.empty();
    }

    std::string MakeVertexKeyString(const ObjVertexKey& key)
    {
        return
            std::to_string(key.PositionIndex) + "/" +
            std::to_string(key.TexCoordIndex) + "/" +
            std::to_string(key.NormalIndex);
    }

    int FindMaterialIndexByName(
        const std::vector<ObjMaterialData>& materials,
        const std::string& materialName)
    {
        for (size_t i = 0; i < materials.size(); ++i)
        {
            if (materials[i].Name == materialName)
            {
                return static_cast<int>(i);
            }
        }

        return -1;
    }

    void CloseCurrentSubmeshIfNeeded(
        MeshData& meshData,
        unsigned int submeshStartIndex,
        int materialIndex)
    {
        const unsigned int currentIndexCount =
            static_cast<unsigned int>(meshData.Indices.size());

        if (currentIndexCount <= submeshStartIndex)
        {
            return;
        }

        SubmeshData submesh = {};
        submesh.StartIndex = submeshStartIndex;
        submesh.IndexCount = currentIndexCount - submeshStartIndex;
        submesh.MaterialIndex = materialIndex;

        meshData.Submeshes.push_back(submesh);
    }

}

bool ObjLoader::LoadFromFile(
    const std::string& filePath,
    MeshData& outMeshData)
{
    outMeshData = {};

    std::ifstream file(filePath);

    if (!file.is_open())
    {
        return false;
    }

    std::vector<XMFLOAT3> positions;
    std::vector<XMFLOAT2> texCoords;
    std::vector<XMFLOAT3> normals;

    std::unordered_map<std::string, unsigned int> uniqueVertexMap;

    std::string line;

    std::filesystem::path objPath = filePath;
    std::filesystem::path objDirectory = objPath.parent_path();

    int currentMaterialIndex = -1;
    unsigned int currentSubmeshStartIndex = 0;

    while (std::getline(file, line))
    {
        if (line.empty())
        {
            continue;
        }

        std::istringstream lineStream(line);

        std::string command;
        lineStream >> command;

        if (command == "v")
        {
            XMFLOAT3 position = {};
            lineStream >> position.x >> position.y >> position.z;

            positions.push_back(position);
        }
        else if (command == "vt")
        {
            XMFLOAT2 texCoord = {};
            lineStream >> texCoord.x >> texCoord.y;

            // OBJ texture V usually starts from the bottom.
            // Direct3D texture V usually starts from the top.
            texCoord.y = 1.0f - texCoord.y;

            texCoords.push_back(texCoord);
        }
        else if (command == "vn")
        {
            XMFLOAT3 normal = {};
            lineStream >> normal.x >> normal.y >> normal.z;

            normals.push_back(normal);
        }
        else if (command == "mtllib")
        {
            std::string mtlFileName;
            lineStream >> mtlFileName;

            if (!mtlFileName.empty())
            {
                std::filesystem::path mtlPath = objDirectory / mtlFileName;
                mtlPath = mtlPath.lexically_normal();

                std::vector<ObjMaterialData> materials;

                if (LoadMaterialsFromMtl(mtlPath, materials))
                {
                    outMeshData.Materials = materials;

                    OutputDebugStringA("OBJ MTL materials loaded: ");
                    OutputDebugStringA(std::to_string(outMeshData.Materials.size()).c_str());
                    OutputDebugStringA("\n");

                    // Milestone 17 compatibility:
                    // Keep using the first material texture as the old single-scene texture.
                    for (const ObjMaterialData& material : outMeshData.Materials)
                    {
                        OutputDebugStringA("  Material: ");
                        OutputDebugStringA(material.Name.c_str());

                        if (!material.DiffuseTexturePath.empty())
                        {
                            OutputDebugStringA(" | map_Kd: ");
                            OutputDebugStringA(material.DiffuseTexturePath.c_str());

                            if (outMeshData.DiffuseTexturePath.empty())
                            {
                                outMeshData.DiffuseTexturePath = material.DiffuseTexturePath;
                            }
                        }

                        OutputDebugStringA("\n");
                    }
                }
                else
                {
                    OutputDebugStringA("OBJ MTL loaded but no materials were parsed.\n");
                }
            }
        }
        else if (command == "f")
        {
            std::vector<ObjVertexKey> faceVertices;

            std::string vertexToken;

            while (lineStream >> vertexToken)
            {
                ObjVertexKey key = {};

                if (!ParseObjVertexToken(vertexToken, key))
                {
                    return false;
                }

                if (!IsValidPositiveIndex(key.PositionIndex, positions.size()))
                {
                    return false;
                }

                if (key.TexCoordIndex != 0 &&
                    !IsValidPositiveIndex(key.TexCoordIndex, texCoords.size()))
                {
                    return false;
                }

                if (key.NormalIndex != 0 &&
                    !IsValidPositiveIndex(key.NormalIndex, normals.size()))
                {
                    return false;
                }

                faceVertices.push_back(key);
            }

            if (faceVertices.size() < 3)
            {
                return false;
            }

            // Triangulate the face as a fan:
            //
            // quad:      0 1 2 3
            // triangles: 0 1 2
            //            0 2 3
            for (size_t i = 1; i + 1 < faceVertices.size(); ++i)
            {
                ObjVertexKey triangleKeys[3] =
                {
                    faceVertices[0],
                    faceVertices[i],
                    faceVertices[i + 1]
                };

                for (int vertexIndex = 0; vertexIndex < 3; ++vertexIndex)
                {
                    const ObjVertexKey& key = triangleKeys[vertexIndex];

                    std::string uniqueKey = MakeVertexKeyString(key);

                    auto found = uniqueVertexMap.find(uniqueKey);

                    if (found != uniqueVertexMap.end())
                    {
                        outMeshData.Indices.push_back(found->second);
                        continue;
                    }

                    Vertex vertex = {};

                    vertex.Position =
                        positions[static_cast<size_t>(key.PositionIndex - 1)];

                    if (key.NormalIndex != 0)
                    {
                        vertex.Normal =
                            normals[static_cast<size_t>(key.NormalIndex - 1)];
                    }
                    else
                    {
                        vertex.Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
                    }

                    if (key.TexCoordIndex != 0)
                    {
                        vertex.TexCoord =
                            texCoords[static_cast<size_t>(key.TexCoordIndex - 1)];
                    }
                    else
                    {
                        vertex.TexCoord = XMFLOAT2(0.0f, 0.0f);
                    }

                    unsigned int newIndex =
                        static_cast<unsigned int>(outMeshData.Vertices.size());

                    outMeshData.Vertices.push_back(vertex);
                    outMeshData.Indices.push_back(newIndex);

                    uniqueVertexMap[uniqueKey] = newIndex;
                }
            }
        }
        else if (command == "usemtl")
        {
            // Close the previous material range before switching materials.
            CloseCurrentSubmeshIfNeeded(
                outMeshData,
                currentSubmeshStartIndex,
                currentMaterialIndex);

            currentSubmeshStartIndex =
                static_cast<unsigned int>(outMeshData.Indices.size());

            std::string materialName;
            std::getline(lineStream, materialName);

            materialName = TrimWhitespace(materialName);

            currentMaterialIndex =
                FindMaterialIndexByName(
                    outMeshData.Materials,
                    materialName);

            if (currentMaterialIndex == -1 && !materialName.empty())
            {
                // Robust fallback:
                // If OBJ references a material that was not found in the MTL,
                // create a placeholder material so the submesh still has a valid material slot.
                ObjMaterialData fallbackMaterial = {};
                fallbackMaterial.Name = materialName;
                fallbackMaterial.DiffuseColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

                outMeshData.Materials.push_back(fallbackMaterial);

                currentMaterialIndex =
                    static_cast<int>(outMeshData.Materials.size() - 1);
            }

            OutputDebugStringA("OBJ usemtl: ");
            OutputDebugStringA(materialName.c_str());
            OutputDebugStringA(" -> material index ");
            OutputDebugStringA(std::to_string(currentMaterialIndex).c_str());
            OutputDebugStringA("\n");
}


        //
    }


    CloseCurrentSubmeshIfNeeded(
        outMeshData,
        currentSubmeshStartIndex,
        currentMaterialIndex);

    if (outMeshData.Submeshes.empty() && !outMeshData.Indices.empty())
    {
        SubmeshData fullMeshSubmesh = {};
        fullMeshSubmesh.StartIndex = 0;
        fullMeshSubmesh.IndexCount =
            static_cast<unsigned int>(outMeshData.Indices.size());
        fullMeshSubmesh.MaterialIndex = -1;

        outMeshData.Submeshes.push_back(fullMeshSubmesh);
    }

    OutputDebugStringA("OBJ submeshes loaded: ");
    OutputDebugStringA(std::to_string(outMeshData.Submeshes.size()).c_str());
    OutputDebugStringA("\n");

    return
        !outMeshData.Vertices.empty() &&
        !outMeshData.Indices.empty();
}