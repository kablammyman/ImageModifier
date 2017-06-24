cmake_minimum_required(VERSION 2.8.9)
project(ImageModifier)

set (CMAKE_CXX_STANDARD 11)

	IF (WIN32)
		set (CMAKE_CXX_FLAGS "/D _CRT_SECURE_NO_WARNINGS -W4 /EHsc")
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/Debug)
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/Release)
	ELSE()
		set (CMAKE_CXX_FLAGS "-I/usr/include/i386-linux-gnu/c++/4.8  -I/usr/include/SDL2 -std=c++11 -pthread -g3 -Wall -O2")
		# includes cmake/FindSDL2.cmake
		set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
	ENDIF()


ADD_SUBDIRECTORY(ImageModifier)

