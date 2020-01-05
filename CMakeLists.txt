cmake_minimum_required(VERSION 3.1.0)

project(Lsystems)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "-Wall -pedantic -std=c++17 -fPIC")
  set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -DDEBUG")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
endif(CMAKE_COMPILER_IS_GNUCXX)

if(WIN32)
	include_directories("../OGLPack/include")
	link_directories("../OGLPack/lib/x86")
endif()

add_executable(Lsystems
	main.cpp
	graphics.cpp
	graphics.hpp
	lindenmayer.cpp
	lindenmayer.hpp
	geometry.hpp
)

if(WIN32)
	target_link_libraries(Lsystems "SDL2.lib")
	target_link_libraries(Lsystems "SDL2main.lib")
	target_link_libraries(Lsystems "glew32.lib")
	target_link_libraries(Lsystems "opengl32.lib")

	set(CMAKE_MSVCIDE_RUN_PATH "../OGLPack/bin/x86")
endif()

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/examples/" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/examples")