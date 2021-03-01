cmake_minimum_required(VERSION 3.17)
project(vk_wrapper)

# Set c++ Standard
set(CMAKE_CXX_STANDARD 17)
# Prevent compiler from falling back to latest supported standard
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Ensures the compiler uses '-std=c++11' rather  than '-std=g++11'
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Vulkan REQUIRED FATAL_ERROR)

add_subdirectory(examples)

add_library(vk_wrapper src/Core.cpp src/Core.h src/Context.cpp src/Context.h src/DeletionQueue.h src/Helpers.cpp src/Helpers.h)

target_include_directories(vk_wrapper PUBLIC src)

target_link_libraries(vk_wrapper PUBLIC Vulkan::Vulkan)