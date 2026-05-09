#pragma once


#include <DirectXMath.h>

struct Player
{
    DirectX::XMFLOAT3 Position = DirectX::XMFLOAT3(0.0f, -0.20f, -0.8f);

    float Yaw = 0.0f;
    float MoveSpeed = 0.9f;
};