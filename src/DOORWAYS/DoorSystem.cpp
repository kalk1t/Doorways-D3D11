#include "DoorSystem.h"
#include "App.h"

#include <d3dcompiler.h>
#include <cstddef>
#include <cstdint>
#include <DirectXMath.h>
#include <string>
#include <chrono>
#include <cmath>

DoorSystem::DoorSystem(App* app)
    : mApp(app)
{
}

EnvironmentId DoorSystem::GetNearbyDoor() const
{
    if (std::fabs(mApp->mWorld.MainPlayer.Position.z - gDoorZ) > gDoorInteractionDepth)
    {
        return EnvironmentId::Porch;
    }

    if (std::fabs(mApp->mWorld.MainPlayer.Position.x - GetDoorX(EnvironmentId::Sunny)) <= gDoorInteractionHalfWidth)
    {
        return EnvironmentId::Sunny;
    }

    if (std::fabs(mApp->mWorld.MainPlayer.Position.x - GetDoorX(EnvironmentId::Rainy)) <= gDoorInteractionHalfWidth)
    {
        return EnvironmentId::Rainy;
    }

    if (std::fabs(mApp->mWorld.MainPlayer.Position.x - GetDoorX(EnvironmentId::Snowy)) <= gDoorInteractionHalfWidth)
    {
        return EnvironmentId::Snowy;
    }

    return EnvironmentId::Porch;
}

float DoorSystem::GetDoorX(EnvironmentId door) const
{
    switch (door)
    {
    case EnvironmentId::Sunny:
        return gSunnyDoorX;

    case EnvironmentId::Rainy:
        return gRainyDoorX;

    case EnvironmentId::Snowy:
        return gSnowyDoorX;

    default:
        return 0.0f;
    }
}


const wchar_t* DoorSystem::GetDoorDisplayName(EnvironmentId door) const
{
    switch (door)
    {
    case EnvironmentId::Sunny:
        return L"SUNNY";

    case EnvironmentId::Rainy:
        return L"RAINY";

    case EnvironmentId::Snowy:
        return L"SNOWY";

    default:
        return L"Porch";
    }
}

void DoorSystem::UpdateDoorInteractionFeedback()
{
    EnvironmentId newNearbyDoor = GetNearbyDoor();

    if (newNearbyDoor == mApp->mWorld.NearbyDoor)
    {
        return;
    }

    mApp->mWorld.NearbyDoor = newNearbyDoor;

    mApp->UpdateWindowTitle();
}

void DoorSystem::HandleDoorInteraction()
{
    bool isInteractKeyDown = (GetAsyncKeyState('E') & 0x8000) != 0;

    bool wasPressedThisFrame =
        isInteractKeyDown && !mApp->mWorld.WasInteractKeyDown;

    mApp->mWorld.WasInteractKeyDown = isInteractKeyDown;

    if (!wasPressedThisFrame)
    {
        return;
    }

    if (mApp->mWorld.NearbyDoor == EnvironmentId::Porch)
    {
        mApp->UpdateWindowTitle();
        return;
    }

    EnterEnvironment(mApp->mWorld.NearbyDoor);
}

void DoorSystem::EnterEnvironment(EnvironmentId door)
{
    if (door == EnvironmentId::Porch)
    {
        return;
    }

    if (mApp->mWorld.ActiveEnvironment != door)
    {
        mApp->mWorld.EnvironmentTime = 0.0f;
    }

    mApp->mWorld.ActiveEnvironment = door;

    mApp->UpdateWindowTitle();
}


void DoorSystem::ResetToPorch()
{
    mApp->mWorld.MainCamera.Yaw = 0.0f;
    mApp->mWorld.MainCamera.Pitch = -0.25f;

    mApp->mWorld.MainPlayer.Position = XMFLOAT3(0.0f, -0.20f, -0.8f);

    mApp->mWorld.NearbyDoor = EnvironmentId::Porch;
    mApp->mWorld.ActiveEnvironment = EnvironmentId::Porch;
    mApp->mWorld.EnvironmentTime = 0.0f;

    mApp->UpdateWindowTitle();
}