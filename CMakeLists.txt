cmake_minimum_required(VERSION 3.15)
project(expert-system)

set(CMAKE_CXX_STANDARD 20)

include(Grammar/grammar.cmake)
#
add_custom_target(minilibx COMMAND make
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/minilibx_macos
		)

add_executable(${PROJECT_NAME} main.cpp
		${CMAKE_CURRENT_BINARY_DIR}/Grammar/AVMLexer.ll.cpp ${CMAKE_CURRENT_BINARY_DIR}/Grammar/AVMParser.yy.cpp )



#find_package(GLEW REQUIRED)
#find_package(GLUT REQUIRED)

find_package(NumCpp REQUIRED)

#find_package(OpenCL REQUIRED)
#find_package(OpenCL 1.2 EXACT REQUIRED)


#find_package(GLUT REQUIRED)
#include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})


target_link_libraries(${PROJECT_NAME}
		#		GLEW::GLEW
#		${GLUT_LIBRARY}
#		OpenCL::OpenCL
		NumCpp::NumCpp
		)

add_dependencies(${PROJECT_NAME}  Grammar)
