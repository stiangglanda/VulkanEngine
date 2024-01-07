# CMakeList.txt : CMake project for VulkanEngine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.16)

# Enable Hot Reload for MSVC compilers if supported.
# set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API 2182bf5c-ef0d-489a-91da-49dbc3090d2a)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)

project ("VulkanEngine") 

add_subdirectory("vendor/glfw")
add_subdirectory("vendor/glm")
add_subdirectory("vendor/tinyobjloader")
add_subdirectory("vendor/spdlog")

find_package(Vulkan REQUIRED)
# Add source to this project's executable. 

add_library(Core "src/Core/Core.cpp" "src/Core/Timer.h" "src/Core/Timer.cpp"
"src/Core/Application.cpp" "src/Core/Application.h" "src/Core/Window.h" 
"src/Core/Window.cpp" "src/Core/Input.h"
"src/Core/KeyCodes.h" "src/Core/MouseCodes.h" 
"src/Core/Platform/Windows/WindowsWindow.h" 
"src/Core/Platform/Windows/WindowsWindow.cpp" 
"src/Core/Platform/Windows/WindowsInput.cpp" 
"src/Core/PlatformDetection.h" "src/Core/EntryPoint.h" 
"src/Core/Events/ApplicationEvent.h"
"src/Core/Events/Event.h"
"src/Core/Events/KeyEvent.h"
"src/Core/Events/MouseEvent.h" "src/Core/vepch.h" "src/Core/vepch.cpp" 
"src/Core/Log.cpp" "src/Core/Renderer/RenderAPI.cpp" "src/Core/Renderer/RenderAPI.h" 
"src/Core/Renderer/RenderAPI/Vulkan/VulkanAPI.h" "src/Core/Renderer/RenderAPI/Vulkan/VulkanAPI.cpp" 
"src/Core/Renderer/GraphicsContext.cpp" "src/Core/Renderer/GraphicsContext.h" 
"src/Core/Renderer/RenderAPI/Vulkan/VulkanContext.h" "src/Core/Renderer/RenderAPI/Vulkan/VulkanContext.cpp")

target_precompile_headers(Core PRIVATE "src/Core/vepch.h")

target_compile_definitions(Core PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/") # This is useful to get an ASSETS_PATH in your IDE during development but you should comment this if you compile a release version and uncomment the next line

target_include_directories(Core 
	PUBLIC vendor/glfw/include 
	PUBLIC vendor/glm/glm 
	PUBLIC vendor/spdlog/include 
)

target_link_directories(Core 
	PUBLIC vendor/glfw/src 
	PUBLIC vendor/glm/glm 
	PUBLIC vendor/spdlog/src 
)

target_link_libraries(Core Vulkan::Vulkan)
target_link_libraries(Core glfw)
target_link_libraries(Core glm::glm)
target_link_libraries(Core spdlog::spdlog)

add_executable(Client "src/Client/main.cpp" "vendor/stb_image.h")

target_compile_definitions(Client PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/") # This is useful to get an ASSETS_PATH in your IDE during development but you should comment this if you compile a release version and uncomment the next line

target_include_directories(Client 
	PUBLIC vendor/glfw/include 
	PUBLIC vendor/glm/glm 
	PUBLIC vendor/tinyobjloader 
	PUBLIC src/Core
)

target_link_directories(Client 
	PUBLIC vendor/glfw/src 
	PUBLIC vendor/glm/glm 
	PUBLIC vendor/tinyobjloader 
	PRIVATE src/Core 
)

target_link_libraries(Client Vulkan::Vulkan)
target_link_libraries(Client glfw)
target_link_libraries(Client glm::glm)
target_link_libraries(Client tinyobjloader)
target_link_libraries(Client Core)

# Test section

enable_testing()
add_subdirectory("vendor/googletest")
include(GoogleTest)

add_executable(
  VE_Test
  "test/TimerUnitTests.cpp"
  "test/EventUnitTests.cpp" 
  "test/WindowingUnitTests.cpp")

  
gtest_discover_tests(VE_Test DISCOVERY_MODE PRE_TEST)

target_link_libraries(VE_Test GTest::gtest_main)

target_include_directories(VE_Test PUBLIC src/Core)

target_link_directories(VE_Test PRIVATE src/Core)

target_link_libraries(VE_Test Core)

#gtest_add_tests(TARGET VE_Test)


# TODO: Add tests and install targets if needed.
