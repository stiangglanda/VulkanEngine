#pragma once
#include "Application.h"
#include "vepch.h"

#if defined(VE_PLATFORM_WINDOWS) || defined(VE_PLATFORM_LINUX)

extern Core::Application *Core::CreateApplication(ApplicationCommandLineArgs args);

int main(int argc, char **argv)
{
    Core::Log::Init();

    VE_CORE_INFO("Start VulkanEngine");

    auto app = Core::CreateApplication({argc, argv});
    app->Run();

    VE_CORE_INFO("End VulkanEngine");
    delete app;
}

#endif
