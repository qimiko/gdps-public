cmake_minimum_required(VERSION 3.4.0)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(ANDROID_ABI armeabi-v7a)
set(ANDROID_STL c++_shared)

project(gdps)

add_subdirectory(libaurav2)
