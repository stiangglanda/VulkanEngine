# CMakeList.txt : CMake project for VulkanEngine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)

project ("VulkanEngine")

find_package(glfw3 3.3 REQUIRED)
find_package(glm REQUIRED)
find_package(tinyobjloader REQUIRED)
find_package(Vulkan REQUIRED)
# Add source to this project's executable. 
add_executable (VulkanEngine "src/main.cpp" "vendor/stb_image.h")

target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/") # This is useful to get an ASSETS_PATH in your IDE during development but you should comment this if you compile a release version and uncomment the next line

target_link_libraries(VulkanEngine Vulkan::Vulkan)
target_link_libraries(VulkanEngine glfw)
target_link_libraries(VulkanEngine glm::glm)
target_link_libraries(VulkanEngine tinyobjloader::tinyobjloader)

# TODO: Add tests and install targets if needed.
