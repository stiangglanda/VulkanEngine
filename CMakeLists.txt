# CMakeList.txt : CMake project for VulkanEngine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API aa1f7df0-828a-4fcd-9afc-2dc80491aca7)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)

project ("VulkanEngine") 

add_subdirectory("vendor/glfw")
add_subdirectory("vendor/glm")
add_subdirectory("vendor/tinyobjloader")

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
"src/Core/Events/MouseEvent.h")

target_include_directories(Core 
	PUBLIC vendor/glfw/include 
	PUBLIC vendor/glm/glm 
)

target_link_directories(Core 
	PUBLIC vendor/glfw/src 
	PUBLIC vendor/glm/glm 
)

target_link_libraries(Core Vulkan::Vulkan)
target_link_libraries(Core glfw)
target_link_libraries(Core glm::glm)

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

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  VE_Test
  "test/VE_UnitTest.cpp"
)

target_include_directories(VE_Test PUBLIC src/Core)

target_link_directories(VE_Test PRIVATE src/Core)

target_link_libraries(VE_Test GTest::gtest_main)
target_link_libraries(VE_Test Core)

include(GoogleTest)
gtest_discover_tests(VE_Test)

# TODO: Add tests and install targets if needed.
