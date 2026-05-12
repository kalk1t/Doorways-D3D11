#include "Camera.h"
#include <DirectXMath.h>

using namespace DirectX;


XMMATRIX Camera::BuildViewProjectionMatrix(float aspectRatio) const
{


    XMVECTOR cameraPosition = XMLoadFloat3(&Position);

    float cosYaw = cosf(Yaw);
    float sinYaw = sinf(Yaw);
    float cosPitch = cosf(Pitch);
    float sinPitch = sinf(Pitch);

    XMVECTOR lookDirection = XMVectorSet(
        cosPitch * sinYaw,
        sinPitch,
        cosPitch * cosYaw,
        0.0f);

    XMVECTOR cameraTarget = cameraPosition + lookDirection;

    XMVECTOR upDirection = XMVectorSet(
        0.0f,
        1.0f,
        0.0f,
        0.0f);

    XMMATRIX view = XMMatrixLookAtLH(
        cameraPosition,
        cameraTarget,
        upDirection);

    constexpr float fieldOfViewY = 0.25f * XM_PI;
    constexpr float nearPlane = 0.1f;
    constexpr float farPlane = 200.0f;

    XMMATRIX projection = XMMatrixPerspectiveFovLH(
        fieldOfViewY,
        aspectRatio,
        nearPlane,
        farPlane);

    return view * projection;
}