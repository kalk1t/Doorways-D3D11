#include "PlayerController.h"

#include "WorldState.h"
#include "PlayerBlockedZone.h"
#include "PlayerGroundHeight.h"

#include <Windows.h>
#include <DirectXMath.h>
#include <cmath>

using namespace DirectX;

namespace
{
    void SetSystemCursorVisible(bool visible)
    {
        if (visible)
        {
            while (ShowCursor(TRUE) < 0)
            {
            }
        }
        else
        {
            while (ShowCursor(FALSE) >= 0)
            {
            }
        }
    }

    void ClipCursorToClientArea(HWND windowHandle)
    {
        RECT clientRect = {};
        GetClientRect(
            windowHandle,
            &clientRect);

        POINT topLeft = {};
        topLeft.x = clientRect.left;
        topLeft.y = clientRect.top;

        POINT bottomRight = {};
        bottomRight.x = clientRect.right;
        bottomRight.y = clientRect.bottom;

        ClientToScreen(
            windowHandle,
            &topLeft);

        ClientToScreen(
            windowHandle,
            &bottomRight);

        RECT screenRect = {};
        screenRect.left = topLeft.x;
        screenRect.top = topLeft.y;
        screenRect.right = bottomRight.x;
        screenRect.bottom = bottomRight.y;

        ClipCursor(&screenRect);
    }

    float ClampFloat(
        float value,
        float minValue,
        float maxValue)
    {
        if (value < minValue)
        {
            return minValue;
        }

        if (value > maxValue)
        {
            return maxValue;
        }

        return value;
    }

}


void PlayerController::Update(WorldState& world,float deltaTime,HWND windowHandle,
    int mouseWheelDelta)
{
    if (GetAsyncKeyState('R') & 0x8000)
    {
        ResetPlayerAndCamera(world);
        return;
    }

    UpdateCameraRotation(world,windowHandle);
    UpdateCameraZoom(world,mouseWheelDelta);
    ClampCameraPitch(world);

    UpdatePlayerMovement(world, deltaTime);
    UpdateCameraFollow(world);
}

void PlayerController::SetMouseLookActive(
    HWND windowHandle,
    bool active)
{
    if (active == mIsMouseLookActive)
    {
        return;
    }

    mIsMouseLookActive = active;
    mIsMouseLookInitialized = false;

    if (active)
    {
        SetSystemCursorVisible(false);
        ClipCursorToClientArea(windowHandle);
    }
    else
    {
        ClipCursor(nullptr);
        SetSystemCursorVisible(true);
    }
}

void PlayerController::UpdateCameraRotation(
    WorldState& world,
    HWND windowHandle)
{
    if (GetForegroundWindow() != windowHandle)
    {
        SetMouseLookActive(windowHandle,false);

        return;
    }

    SetMouseLookActive(windowHandle,true);

    RECT clientRect = {};
    GetClientRect(
        windowHandle,
        &clientRect);

    POINT centerPoint = {};
    centerPoint.x = (clientRect.right - clientRect.left) / 2;
    centerPoint.y = (clientRect.bottom - clientRect.top) / 2;

    ClientToScreen(
        windowHandle,
        &centerPoint);

    if (!mIsMouseLookInitialized)
    {
        SetCursorPos(
            centerPoint.x,
            centerPoint.y);

        mIsMouseLookInitialized = true;
        return;
    }

    POINT mousePoint = {};
    GetCursorPos(&mousePoint);

    int mouseDeltaX =
        mousePoint.x - centerPoint.x;

    int mouseDeltaY =
        mousePoint.y - centerPoint.y;

    world.MainCamera.Yaw +=
        static_cast<float>(mouseDeltaX) * mMouseSensitivity;

    world.MainCamera.Pitch -=
        static_cast<float>(mouseDeltaY) * mMouseSensitivity;

    SetCursorPos(
        centerPoint.x,
        centerPoint.y);
}

void PlayerController::UpdateCameraZoom(WorldState& world,int mouseWheelDelta)
{
    if (mouseWheelDelta == 0)
    {
        return;
    }

    float wheelSteps =
        static_cast<float>(mouseWheelDelta) /
        static_cast<float>(WHEEL_DELTA);

    // Wheel up usually gives positive delta.
    // Positive delta should zoom closer, so distance decreases.
    world.MainCamera.FollowDistance -=
        wheelSteps * world.MainCamera.ZoomSpeed;

    if (world.MainCamera.FollowDistance <
        world.MainCamera.MinFollowDistance)
    {
        world.MainCamera.FollowDistance =
            world.MainCamera.MinFollowDistance;
    }

    if (world.MainCamera.FollowDistance >
        world.MainCamera.MaxFollowDistance)
    {
        world.MainCamera.FollowDistance =
            world.MainCamera.MaxFollowDistance;
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

void PlayerController::UpdateCameraFollow(WorldState& world)
{

    float sinYaw = sinf(world.MainCamera.Yaw);
    float cosYaw = cosf(world.MainCamera.Yaw);

    // This matches the horizontal part of Camera::BuildViewProjectionMatrix().
    // When Yaw = 0, forward is +Z.
    XMVECTOR cameraForward =
        XMVectorSet(
            sinYaw,
            0.0f,
            cosYaw,
            0.0f);

    XMVECTOR playerPosition =
        XMLoadFloat3(&world.MainPlayer.Position);

    XMVECTOR cameraPosition =
        playerPosition -
        cameraForward * world.MainCamera.FollowDistance +
        XMVectorSet(0.0f, world.MainCamera.FollowHeight, 0.0f, 0.0f);

    XMStoreFloat3(
        &world.MainCamera.Position,
        cameraPosition);
}

void PlayerController::ResetPlayerAndCamera(
    WorldState& world)
{
    world.MainPlayer.Position = world.MainPlayerSpawn.Position;

    ClampPlayerToPlayableArea(world);

    world.MainPlayer.TargetGroundY =
        GetMainSceneGroundHeight(
            world.MainPlayer.Position.x,
            world.MainPlayer.Position.z);

    world.MainPlayer.Position.y =
        world.MainPlayer.TargetGroundY;

    world.MainPlayer.Yaw =world.MainPlayerSpawn.Yaw;

    world.MainPlayer.IsMoving = false;
    world.MainPlayer.WalkCycleTime = 0.0f;

    ClampPlayerToPlayableArea(world);

    world.MainCamera.Yaw =world.MainPlayerSpawn.Yaw;

    world.MainCamera.Pitch = -0.25f;

    UpdateCameraFollow(world);
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
        world.MainPlayer.IsMoving = false;
        return;
    }

    world.MainPlayer.IsMoving = true;

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

    XMVECTOR candidatePosition =
        currentPosition + movementAmount;

    XMFLOAT3 candidatePositionFloat = {};
    XMStoreFloat3(
        &candidatePositionFloat,
        candidatePosition);

    // Temporary flat-ground lock.
    // Later this becomes stairs / terrain / collision height.
    //candidatePositionFloat.y =world.MainPlayer.GroundY;

    // First commit the candidate position.
    world.MainPlayer.Position =
        candidatePositionFloat;

    // Keep the player inside the first playable area.
    ClampPlayerToPlayableArea(world);


    world.MainPlayer.TargetGroundY =
        GetMainSceneGroundHeight(
            world.MainPlayer.Position.x,
            world.MainPlayer.Position.z);

    float heightBlend =
        world.MainPlayer.HeightFollowSpeed * deltaTime;

    heightBlend =
        ClampFloat(
            heightBlend,
            0.0f,
            1.0f);

    world.MainPlayer.Position.y =
        world.MainPlayer.Position.y +
        (world.MainPlayer.TargetGroundY - world.MainPlayer.Position.y) *
        heightBlend;

    // Extra blocked-zone check.
    // Currently returns false, but we keep this hook for future water,
    // columns, rocks, doors, and temple collision.
    if (IsMainSceneBlockedPosition(world.MainPlayer.Position))
    {
        XMStoreFloat3(
            &world.MainPlayer.Position,
            currentPosition);

        world.MainPlayer.TargetGroundY =
            GetMainSceneGroundHeight(
                world.MainPlayer.Position.x,
                world.MainPlayer.Position.z);
    }

    // Advance the walking timer only while movement exists.
    // Later the renderer will use this for arm/leg swinging.
    world.MainPlayer.WalkCycleTime += deltaTime;
}

void PlayerController::ClampPlayerToPlayableArea(
    WorldState& world)
{
    const PlayerMovementBounds& bounds =
        world.MainPlayerBounds;

    if (world.MainPlayer.Position.x < bounds.MinX)
    {
        world.MainPlayer.Position.x = bounds.MinX;
    }

    if (world.MainPlayer.Position.x > bounds.MaxX)
    {
        world.MainPlayer.Position.x = bounds.MaxX;
    }

    if (world.MainPlayer.Position.z < bounds.MinZ)
    {
        world.MainPlayer.Position.z = bounds.MinZ;
    }

    if (world.MainPlayer.Position.z > bounds.MaxZ)
    {
        world.MainPlayer.Position.z = bounds.MaxZ;
    }
}