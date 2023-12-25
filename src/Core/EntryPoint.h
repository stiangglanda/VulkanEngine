#pragma once
#include "Application.h"
#include "vepch.h"

#ifdef VE_PLATFORM_WINDOWS

extern Core::Application *Core::CreateApplication(ApplicationCommandLineArgs args);

int main(int argc, char **argv)
{
    auto app = Core::CreateApplication({argc, argv});
    app->Run();
    delete app;
}

#endif
