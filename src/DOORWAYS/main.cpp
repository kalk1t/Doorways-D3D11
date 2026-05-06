#include <windows.h>
#include "App.h"

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE prevInstance,
    PSTR cmdLine,
    int nCmdShow)
{
    UNREFERENCED_PARAMETER(prevInstance);
    UNREFERENCED_PARAMETER(cmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    App app(hInstance);

    if (!app.Initialize())
    {
        return 0;
    }

    return app.Run();
}