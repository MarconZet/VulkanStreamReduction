cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)

project(VulkanStreamReduce)
add_executable(VulkanStreamReduce src/VulkanContext.cpp src/VulkanContext.h src/Filter.cpp src/Filter.h src/vulkan_assertion_error.cpp src/vulkan_assertion_error.h src/common.h main.cpp)

find_package(Vulkan REQUIRED FATAL_ERROR)
target_link_libraries(VulkanStreamReduce Vulkan::Vulkan)