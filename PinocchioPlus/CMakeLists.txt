cmake_minimum_required(VERSION 3.10)
project(PinocchioPlus)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_BUILD_TYPE RELEASE)
# ====================================================================
# add_compile_options(PINOCCHIO_STATIC)
# add_definitions(-DPINOCCHIO_STATIC)
# list(APPEND CMAKE_CXX_FLAGS "-c -fPIC") # -O3 -Wall, really make a huge difference without -O3
file(GLOB core_src Pinocchio/pinocchio/*.cpp Pinocchio/pinocchio/tools/*.cpp Pinocchio/pinocchio/math/*.cpp)
add_library(Pinocchio ${core_src})
target_include_directories(Pinocchio PUBLIC "vendor/spdlog-1.14.1/include/" "Pinocchio/")
# ====================================================================
file(GLOB skin_src AttachWeights/*.cpp)
# list(APPEND CMAKE_CXX_FLAGS "-c -fPIC") # -O3 -Wall
add_executable(AttachWeights ${skin_src})
target_link_libraries(AttachWeights PUBLIC Pinocchio)
target_include_directories(AttachWeights PUBLIC "${PROJECT_SOURCE_DIR}/Pinocchio")
# ====================================================================
file(GLOB demoui_src DemoUI/*.cpp)
add_executable(DemoUI ${demoui_src})
target_link_libraries(DemoUI Pinocchio fltk fltk_gl GL GLU Xrender Xcursor Xfixes Xft fontconfig Xinerama pthread dl m X11)
target_include_directories(DemoUI
    PUBLIC 
    "${PROJECT_SOURCE_DIR}/Pinocchio"
    "${PROJECT_SOURCE_DIR}/vendor/spdlog-1.14.1/include/"
 )
# ====================================================================
file(GLOB demo_src Demo/*.cpp)
add_executable(Demo ${demo_src})
target_link_libraries(Demo Pinocchio fltk fltk_gl GL GLU Xrender Xcursor Xfixes Xft fontconfig Xinerama pthread dl m X11)
target_include_directories(Demo
    PUBLIC 
    "${PROJECT_SOURCE_DIR}/Pinocchio"
    "${PROJECT_SOURCE_DIR}/vendor/spdlog-1.14.1/include/"
 )