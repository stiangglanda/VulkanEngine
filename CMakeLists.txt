# CMakeList.txt : CMake project for VulkanEngine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)

project ("VulkanEngine")

add_executable (VulkanEngine "src/main.cpp" "vendor/stb_image.h")

add_subdirectory("vendor/glfw")
add_subdirectory("vendor/glm")
add_subdirectory("vendor/tinyobjloader")

find_package(Vulkan REQUIRED)
# Add source to this project's executable. 

target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/") # This is useful to get an ASSETS_PATH in your IDE during development but you should comment this if you compile a release version and uncomment the next line

target_include_directories(VulkanEngine 
	PUBLIC vendor/glfw/include 
	PUBLIC vendor/glm/glm 
	PUBLIC vendor/tinyobjloader 
)

target_link_directories(VulkanEngine 
	PUBLIC vendor/glfw/src 
	PUBLIC vendor/glm/glm 
	PUBLIC vendor/tinyobjloader 
)

target_link_libraries(VulkanEngine Vulkan::Vulkan)
target_link_libraries(VulkanEngine glfw)
target_link_libraries(VulkanEngine glm::glm)
target_link_libraries(VulkanEngine tinyobjloader)

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
target_link_libraries(
  VE_Test
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(VE_Test)

# TODO: Add tests and install targets if needed.
