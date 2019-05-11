cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(hanz LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_FLAGS "-g")

find_package(OpenCV REQUIRED)
find_package(X11 REQUIRED)

include_directories(${X11_INCLUDE_DIR})
add_executable(${PROJECT_NAME} hand.cc filter.cc camera.cc main.cc)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} ${X11_LIBRARIES})
