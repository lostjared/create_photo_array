cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(img_convert LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQURIED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
find_package(OpenCV REQUIRED)
add_executable(create_photo_array source/create_photo_array.cpp)
include_directories({CMAKE_SOURCE_DIR} ${OpenCV_INCLUDE_DIRS})
target_link_libraries(create_photo_array ${OpenCV_LIBS})
install(TARGETS create_photo_array DESTINATION bin)
