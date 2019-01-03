cmake_minimum_required (VERSION 3.0)
project (GahoodBoy)

find_package(SDL2 REQUIRED)

file(GLOB SRC_FILES src/*.cpp)
include_directories(${SDL2_INCLUDE_DIRS} src)

add_executable(GahoodBoy ${SRC_FILES})
target_link_libraries(GahoodBoy ${SDL2_LIBRARIES})    