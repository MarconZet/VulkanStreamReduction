cmake_minimum_required(VERSION 3.16)
project(VulkanStreamReduce)

set(CMAKE_CXX_STANDARD 20)

FILE(GLOB sources src/*.cpp)

add_executable(VulkanStreamReduce main.cpp ${sources})

target_include_directories(VulkanStreamReduce PUBLIC include)

find_package(Vulkan REQUIRED FATAL_ERROR)
target_link_libraries(VulkanStreamReduce Vulkan::Vulkan)