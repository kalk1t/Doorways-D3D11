#pragma once

#include <DirectXMath.h>

using namespace DirectX;
class Camera
{
public:

    XMFLOAT3 Position = XMFLOAT3(0.0f, 2.0f, -6.0f);

    float Yaw = 0.0f;
    float Pitch = -0.25f;

    float MoveSpeed = 0.05f;
    float TurnSpeed = 1.5f;


    XMMATRIX BuildViewProjectionMatrix(float aspectRatio) const;

};