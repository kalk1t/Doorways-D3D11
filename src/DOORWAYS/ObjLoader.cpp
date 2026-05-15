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

    bool LoadFirstDiffuseTextureFromMtl(
        const std::filesystem::path& mtlPath,
        std::string& outDiffuseTexturePath)
    {
        std::ifstream mtlFile(mtlPath);

        if (!mtlFile.is_open())
        {
            return false;
        }

        std::string line;

        while (std::getline(mtlFile, line))
        {
            if (line.empty())
            {
                continue;
            }

            std::istringstream lineStream(line);

            std::string command;
            lineStream >> command;

            if (command == "map_Kd")
            {
                std::string texturePathText;
                std::getline(lineStream, texturePathText);

                // Remove leading spaces after map_Kd.
                size_t firstNonSpace = texturePathText.find_first_not_of(" \t");

                if (firstNonSpace == std::string::npos)
                {
                    return false;
                }

                texturePathText = texturePathText.substr(firstNonSpace);

                std::filesystem::path texturePath = texturePathText;

                if (texturePath.is_relative())
                {
                    texturePath = mtlPath.parent_path() / texturePath;
                }

                texturePath = texturePath.lexically_normal();

                outDiffuseTexturePath = texturePath.string();
                return true;
            }
        }

        return false;
    }


    std::string MakeVertexKeyString(const ObjVertexKey& key)
    {
        return
            std::to_string(key.PositionIndex) + "/" +
            std::to_string(key.TexCoordIndex) + "/" +
            std::to_string(key.NormalIndex);
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

                std::string diffuseTexturePath;

                if (LoadFirstDiffuseTextureFromMtl(mtlPath, diffuseTexturePath))
                {
                    outMeshData.DiffuseTexturePath = diffuseTexturePath;

                    OutputDebugStringA("OBJ diffuse texture found: ");
                    OutputDebugStringA(outMeshData.DiffuseTexturePath.c_str());
                    OutputDebugStringA("\n");
                }
                else
                {
                    OutputDebugStringA("OBJ MTL loaded but no map_Kd diffuse texture found.\n");
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
    }

    return
        !outMeshData.Vertices.empty() &&
        !outMeshData.Indices.empty();
}