#pragma once

#include "PlatformDetection.h"
#include <vulkan/vulkan.h>

#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <utility>

#include <array>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Log.h"

#ifdef VE_PLATFORM_WINDOWS
#define NOMINMAX
#include <Windows.h>
#endif

namespace Core
{
struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};
}