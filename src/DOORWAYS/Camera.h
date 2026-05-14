#pragma once

#include <DirectXMath.h>

struct Camera
{
    DirectX::XMFLOAT3 Position =
        DirectX::XMFLOAT3(0.0f, 3.6f, -3.3f);

    float Yaw = 0.0f;
    float Pitch = 0.0f;

    float TurnSpeed = 1.8f;

    float FieldOfViewY = 0.25f * DirectX::XM_PI;
    float NearZ = 0.1f;
    float FarZ = 300.0f;

    DirectX::XMMATRIX BuildViewProjectionMatrix(
        float aspectRatio) const;
};