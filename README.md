# VulkanEngine
![Build Status Windows](https://github.com/stiangglanda/VulkanEngine/actions/workflows/cmake-build-windows.yml/badge.svg)
![Build Status Linux](https://github.com/stiangglanda/VulkanEngine/actions/workflows/cmake-build-linux.yml/badge.svg)

A graphics engine that uses the Vulkan API.
Currently, there is only the project set up with CMake, Github Actions and some basic dependencies.
It shows a small sample that implements rendering an obj model with Vulkan in one file.

The plan is to use the engine primarily for voxel rendering.

![alt text](https://github.com/stiangglanda/VulkanEngine/blob/master/resources/VulkanSample.png)

## Build Guide
---

## Install:
- [Vulkan sdk](https://vulkan.lunarg.com/#new_tab)
- [CMake](https://cmake.org/download/)

build the project using cmake 

## Build:
- Clone the project with all its submodules(--recursive)
  - ```git clone --recursive https://github.com/stiangglanda/VulkanEngine.git```
- Configure Project with CMake
  - ```cmake -DCMAKE_BUILD_TYPE=Release -S ../VulkanEngine -B ../VulkanEngine/build```
- Build with CMake
  - ```cmake --build ../VulkanEngine/build --config Release --target all```
- Run the Client
  - ```build/Client```

## Vulkan Engine UML Diagram:
![VulkanEngineUML](https://github.com/user-attachments/assets/2eff6537-5ae6-4a73-b7f4-e75c5e60a433)
