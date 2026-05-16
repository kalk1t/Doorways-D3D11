#pragma once

#include <DirectXMath.h>

struct ImportedSceneSettings
{
    // Default imported scene transform.
    //
    // These values are tuned using the runtime placement controls:
    //
    // J / L = move X
    // U / O = move Y
    // I / K = move Z
    // + / - = scale
    // R     = reset
    //
    // After finding good values in the Visual Studio Output window,
    // paste them here so the scene starts in the correct place.

    DirectX::XMFLOAT3 Scale =
        DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);

    DirectX::XMFLOAT3 Rotation =
        DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

    DirectX::XMFLOAT3 Translation =
        DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
};