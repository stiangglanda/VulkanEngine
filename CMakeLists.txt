# CMakeList.txt : CMake project for VulkanEngine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.19)

# Enable Hot Reload for MSVC compilers if supported.
set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API 2182bf5c-ef0d-489a-91da-49dbc3090d2a)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)

project ("VulkanEngine") 

add_subdirectory("vendor/glfw")
add_subdirectory("vendor/glm")
add_subdirectory("vendor/tinyobjloader")
add_subdirectory("vendor/spdlog")
add_subdirectory("vendor/VulkanMemoryAllocator")
add_subdirectory("vendor/vk-bootstrap")
add_subdirectory("vendor/fastgltf")

set(IMGUI_SOURCES
    "vendor/imgui/imgui.h"
    "vendor/imgui/imgui.cpp"
    "vendor/imgui/imgui_demo.cpp"
    "vendor/imgui/imgui_draw.cpp"
    "vendor/imgui/imgui_widgets.cpp"
    "vendor/imgui/imgui_tables.cpp"
)

find_package(Vulkan REQUIRED)
# Add source to this project's executable. 

add_library(Core ${IMGUI_SOURCES} "vendor/stb_image.h" "src/Core/Core.cpp" "src/Core/Timer.h" "src/Core/Timer.cpp"
"src/Core/Application.cpp" "src/Core/Application.h" "src/Core/Window.h" 
"src/Core/Window.cpp" "src/Core/Input.h"
"src/Core/KeyCodes.h" "src/Core/MouseCodes.h" 
"src/Core/Platform/Windows/WindowsWindow.h" 
"src/Core/Platform/Windows/WindowsWindow.cpp" 
"src/Core/Platform/Windows/WindowsInput.cpp" 
"src/Core/PlatformDetection.h" "src/Core/EntryPoint.h" 
"src/Core/Events/ApplicationEvent.h" "src/Core/Events/Event.h" "src/Core/Events/KeyEvent.h"
"src/Core/Events/MouseEvent.h" "src/Core/vepch.h" "src/Core/vepch.cpp" 
"src/Core/Log.cpp" "src/Core/Renderer/RenderAPI.cpp" "src/Core/Renderer/RenderAPI.h" 
"src/Core/Renderer/RenderAPI/Vulkan/VulkanAPI.h" "src/Core/Renderer/RenderAPI/Vulkan/VulkanAPI.cpp" 
"src/Core/Renderer/RenderAPI/Vulkan/vk_types.h" "src/Core/Renderer/RenderAPI/Vulkan/vk_pipelines.h" 
"src/Core/Renderer/RenderAPI/Vulkan/vk_pipelines.cpp" "src/Core/Renderer/RenderAPI/Vulkan/vk_initializers.h" 
"src/Core/Renderer/RenderAPI/Vulkan/vk_initializers.cpp"  
"src/Core/Renderer/RenderAPI/Vulkan/vk_descriptors.h" "src/Core/Renderer/RenderAPI/Vulkan/vk_descriptors.cpp"   
"src/Core/Renderer/RenderAPI/Vulkan/meshes.h" "src/Core/Renderer/RenderAPI/Vulkan/meshes.cpp"
"src/Core/Renderer/RenderAPI/Vulkan/vk_images.h" "src/Core/Renderer/RenderAPI/Vulkan/vk_images.cpp" 
"src/Core/Renderer/RenderAPI/Vulkan/camera.cpp" "src/Core/Renderer/RenderAPI/Vulkan/vk_loader.h" 
"src/Core/Renderer/RenderAPI/Vulkan/vk_loader.cpp" "src/Core/Renderer/RenderAPI/Vulkan/vk_engine.h" 
"src/Core/Renderer/RenderAPI/Vulkan/vk_engine.cpp" "src/Core/Renderer/RenderAPI/Vulkan/imgui/imgui_impl_vulkan.h" 
"src/Core/Renderer/RenderAPI/Vulkan/imgui/imgui_impl_vulkan.cpp" "src/Core/Renderer/RenderAPI/Vulkan/imgui/imgui_impl_glfw.h" 
"src/Core/Renderer/RenderAPI/Vulkan/imgui/imgui_impl_glfw.cpp" "src/Core/Renderer/Renderer.cpp"
"src/Core/Renderer/Renderer.h" "src/Core/Renderer/RenderAPI/Vulkan/VulkanInstance.h"
"src/Core/Renderer/RenderAPI/Vulkan/VulkanInstance.cpp" "src/Core/Renderer/RenderAPI/Vulkan/VulkanDebug.h"
"src/Core/Renderer/RenderAPI/Vulkan/VulkanDebug.cpp")

set_property(TARGET Core PROPERTY CXX_STANDARD 20)

target_precompile_headers(Core PRIVATE "src/Core/vepch.h")

target_compile_definitions(Core PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/") # This is useful to get an ASSETS_PATH in your IDE during development but you should comment this if you compile a release version and uncomment the next line

target_include_directories(Core 
	PUBLIC vendor/glfw/include 
	PUBLIC vendor/glm/glm 
	PUBLIC vendor/spdlog/include 
	PUBLIC vendor/VulkanMemoryAllocator/include 
	PUBLIC vendor/fastgltf/include 
	PRIVATE vendor/imgui
	PUBLIC vendor/tinyobjloader 
)

target_link_libraries(Core Vulkan::Vulkan)
target_link_libraries(Core glfw)
target_link_libraries(Core glm::glm)
target_link_libraries(Core spdlog::spdlog)
target_link_libraries(Core GPUOpen::VulkanMemoryAllocator)
target_link_libraries(Core vk-bootstrap::vk-bootstrap)
target_link_libraries(Core fastgltf::fastgltf)
target_link_libraries(Core tinyobjloader)

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

  
gtest_discover_tests(VE_Test)

target_link_libraries(VE_Test GTest::gtest_main GTest::gtest)

target_include_directories(VE_Test PUBLIC src/Core)

target_link_directories(VE_Test PRIVATE src/Core)

target_link_libraries(VE_Test Core)

#gtest_add_tests(TARGET VE_Test)


# TODO: Add tests and install targets if needed.
