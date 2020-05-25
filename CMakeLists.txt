cmake_minimum_required(VERSION 3.16)
project(VulkanMap)

set(CMAKE_CXX_STANDARD 20)

FILE(GLOB sources src/*.cpp)

add_executable(VulkanMap example1.cpp ${sources})

target_include_directories(VulkanMap PUBLIC include)

find_package(Vulkan REQUIRED FATAL_ERROR)
target_link_libraries(VulkanMap Vulkan::Vulkan)

set(CMAKE_CXX_FLAGS "-O0")