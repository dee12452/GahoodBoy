cmake_minimum_required (VERSION 3.0)
project (GahoodBoy)

file(GLOB SRC_FILES src/*.cpp)

if(WIN32)
	include_directories(src include/)

	add_executable(GahoodBoy ${SRC_FILES})
	target_link_libraries(GahoodBoy ${CMAKE_CURRENT_SOURCE_DIR}/lib/x86/SDL2.lib ${CMAKE_CURRENT_SOURCE_DIR}/lib/x86/SDL2main.lib)

else()
	find_package(SDL2 REQUIRED)
	include_directories(${SDL2_INCLUDE_DIRS} src)

	add_executable(GahoodBoy ${SRC_FILES})
	target_link_libraries(GahoodBoy ${SDL2_LIBRARIES})

endif()