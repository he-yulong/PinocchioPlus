cmake_minimum_required(VERSION 3.10)
project(homework3)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)

include_directories(
    ${OpenCV_INCLUDE_DIRS} 
    ${EIGEN3_INCLUDE_DIR}
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/include
)

add_library(mesh SHARED
    src/mesh.cpp
    src/half_edge.cpp
)


add_executable(task1 task1.cpp)
target_link_libraries(task1 ${OpenCV_LIBRARIES} mesh)

add_executable(task2 task2.cpp)
target_link_libraries(task2 ${OpenCV_LIBRARIES} mesh)