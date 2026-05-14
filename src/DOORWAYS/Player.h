#pragma once

#include <DirectXMath.h>

struct Player
{
    DirectX::XMFLOAT3 Position =
        DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

    float Yaw = 0.0f;

    float MoveSpeed = 4.0f;
};