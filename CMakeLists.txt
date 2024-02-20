cmake_minimum_required(VERSION 3.26)
project(fishDetection)

set(CMAKE_CXX_STANDARD 17)

add_executable(fishDetection main.cpp
        headers/VideoProcessor.h
        "headers/MovementDetector .h")
set(OpenCV_DIR "C:\\Users\\ra91m\\opencv\\build")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

target_link_libraries(fishDetection ${OpenCV_LIBS})