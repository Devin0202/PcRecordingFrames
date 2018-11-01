cmake_minimum_required(VERSION 3.9)
project(PCframeRecording)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenCV REQUIRED)
add_executable(PCframeRecording main.cpp tools.cpp tools.h)
target_link_libraries(PCframeRecording opencv_objdetect)
target_link_libraries(PCframeRecording opencv_imgproc)
target_link_libraries(PCframeRecording opencv_highgui)
target_link_libraries(PCframeRecording opencv_core)