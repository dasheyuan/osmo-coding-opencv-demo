cmake_minimum_required(VERSION 3.14)
project(osmo_coding)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

SET(OpenCV_DIR /home/chenyuan/3rdparty/opencv-3.2.0/build)
FIND_PACKAGE(OpenCV REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(src)

add_executable(osmo_coding main.cpp)
target_link_libraries(osmo_coding ${OpenCV_LIBS} osmo-core)

# how to install lib,bin,... https://blog.csdn.net/guoyajie1990/article/details/78138636
INSTALL(TARGETS osmo-core
        LIBRARY DESTINATION lib
        )

