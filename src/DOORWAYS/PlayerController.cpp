#include "PlayerController.h"

#include "WorldState.h"

#include <Windows.h>
#include <DirectXMath.h>
#include <cmath>

using namespace DirectX;

void PlayerController::Update(
    WorldState& world,
    float deltaTime)
{
    if (GetAsyncKeyState('R') & 0x8000)
    {
        ResetPlayerAndCamera(world);
        return;
    }

    UpdateCameraRotation(world, deltaTime);
    ClampCameraPitch(world);

    UpdatePlayerMovement(world, deltaTime);
    UpdateCameraFollow(world);
}

void PlayerController::UpdateCameraRotation(
    WorldState& world,
    float deltaTime)
{
    if (GetAsyncKeyState(VK_LEFT) & 0x8000)
    {
        world.MainCamera.Yaw -= world.MainCamera.TurnSpeed * deltaTime;
    }

    if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
    {
        world.MainCamera.Yaw += world.MainCamera.TurnSpeed * deltaTime;
    }

    if (GetAsyncKeyState(VK_UP) & 0x8000)
    {
        world.MainCamera.Pitch += world.MainCamera.TurnSpeed * deltaTime;
    }

    if (GetAsyncKeyState(VK_DOWN) & 0x8000)
    {
        world.MainCamera.Pitch -= world.MainCamera.TurnSpeed * deltaTime;
    }
}

void PlayerController::ClampCameraPitch(
    WorldState& world)
{
    constexpr float pitchLimit = 0.9f;

    if (world.MainCamera.Pitch > pitchLimit)
    {
        world.MainCamera.Pitch = pitchLimit;
    }

    if (world.MainCamera.Pitch < -pitchLimit)
    {
        world.MainCamera.Pitch = -pitchLimit;
    }
}

void PlayerController::UpdatePlayerMovement(
    WorldState& world,
    float deltaTime)
{
    float sinYaw = sinf(world.MainCamera.Yaw);
    float cosYaw = cosf(world.MainCamera.Yaw);

    XMVECTOR forwardDirection = XMVectorSet(
        sinYaw,
        0.0f,
        cosYaw,
        0.0f);

    XMVECTOR rightDirection = XMVectorSet(
        cosYaw,
        0.0f,
        -sinYaw,
        0.0f);

    XMVECTOR movementDirection = XMVectorZero();

    if (GetAsyncKeyState('W') & 0x8000)
    {
        movementDirection += forwardDirection;
    }

    if (GetAsyncKeyState('S') & 0x8000)
    {
        movementDirection -= forwardDirection;
    }

    if (GetAsyncKeyState('D') & 0x8000)
    {
        movementDirection += rightDirection;
    }

    if (GetAsyncKeyState('A') & 0x8000)
    {
        movementDirection -= rightDirection;
    }

    if (XMVectorGetX(XMVector3LengthSq(movementDirection)) <= 0.0f)
    {
        return;
    }

    movementDirection = XMVector3Normalize(movementDirection);

    XMFLOAT3 movementDirectionFloat = {};
    XMStoreFloat3(
        &movementDirectionFloat,
        movementDirection);

    world.MainPlayer.Yaw =
        atan2f(
            movementDirectionFloat.x,
            movementDirectionFloat.z);

    XMVECTOR movementAmount =
        movementDirection * world.MainPlayer.MoveSpeed * deltaTime;

    XMVECTOR currentPosition =
        XMLoadFloat3(&world.MainPlayer.Position);

    currentPosition += movementAmount;

    XMStoreFloat3(
        &world.MainPlayer.Position,
        currentPosition);
}

void PlayerController::UpdateCameraFollow(
    WorldState& world)
{
    world.MainCamera.Position.x =
        world.MainPlayer.Position.x;

    world.MainCamera.Position.y =
        world.MainPlayer.Position.y + 3.6f;

    world.MainCamera.Position.z =
        world.MainPlayer.Position.z - 3.3f;
}

void PlayerController::ResetPlayerAndCamera(
    WorldState& world)
{
    world.MainPlayer.Position =
        XMFLOAT3(0.0f, 0.0f, 0.0f);

    world.MainPlayer.Yaw = 0.0f;

    world.MainCamera.Yaw = 0.0f;
    world.MainCamera.Pitch = 0.0f;

    UpdateCameraFollow(world);
}

