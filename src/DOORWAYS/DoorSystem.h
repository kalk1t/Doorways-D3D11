#pragma once
#include <DirectXMath.h>
#include "Environment.h"

using namespace DirectX;

class App;

class DoorSystem
{
public:

    DoorSystem(App* app);


     float gSunnyDoorX = -1.5f;
     float gRainyDoorX = 0.0f;
     float gSnowyDoorX = 1.5f;

     float gDoorY = 0.4f;
     float gDoorZ = 1.2f;

     float gLabelY = 1.55f;
     float gLabelZ = 1.05f;

     float gDoorWidth = 0.8f;
     float gDoorHeight = 1.8f;
     float gDoorDepth = 0.15f;

     float gLabelWidth = 1.0f;
     float gLabelHeight = 0.25f;
     float gLabelDepth = 0.05f;

     float gDoorInteractionHalfWidth = 0.55f;
     float gDoorInteractionDepth = 0.55f;


    EnvironmentId GetNearbyDoor() const;
    float GetDoorX(EnvironmentId door) const;


    const wchar_t* GetDoorDisplayName(EnvironmentId door) const;
    void UpdateDoorInteractionFeedback();
    void HandleDoorInteraction();
    void EnterEnvironment(EnvironmentId door);
    void ResetToPorch();

	App* mApp = nullptr;
};