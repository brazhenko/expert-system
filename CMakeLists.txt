cmake_minimum_required(VERSION 3.15)
project(expert-system)

set(CMAKE_CXX_STANDARD 20)

include(Grammar/grammar.cmake)
#

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

# <IMGUI>
find_package (
		glfw3 REQUIRED
)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

find_package(GLEW REQUIRED)
if (GLEW_FOUND)
	include_directories(${GLEW_INCLUDE_DIRS})
	#link_libraries(${GLEW_LIBRARIES})
endif()

include_directories(imgui)
include_directories(imgui/examples)
include_directories(imgui/examples/libs/gl3w)
include_directories(/usr/local/include)
include_directories(/opt/local/include)
include_directories(little_imgui)

SET(IMGUI_SOURCES
		little_imgui/imgui.cpp
		little_imgui/imgui_draw.cpp
		little_imgui/imgui_widgets.cpp

		little_imgui/imgui_impl_opengl3.cpp
		little_imgui/imgui_impl_opengl3.h
		little_imgui/imgui_impl_glfw.cpp
		little_imgui/imgui_impl_glfw.h


		little_imgui/ImNodes.cpp
		little_imgui/ImNodes.h
		little_imgui/ImNodesEz.cpp
		little_imgui/ImNodesEz.h

		little_imgui/imconfig.h
		little_imgui/imgui_internal.h
		little_imgui/imstb_rectpack.h
		little_imgui/imstb_textedit.h
		little_imgui/imstb_truetype.h
		)

# </IMGUI>




add_executable(${PROJECT_NAME} main.cpp
		${CMAKE_CURRENT_BINARY_DIR}/Grammar/Lexer.ll.cpp ${CMAKE_CURRENT_BINARY_DIR}/Grammar/Parser.yy.cpp expertNode.cpp expertNode.h little_imgui/Interpreter.cpp Interpreter.h
		${IMGUI_SOURCES} gui.cpp)
add_dependencies(${PROJECT_NAME}  Grammar)

target_link_libraries(${PROJECT_NAME}
		glfw
		${OPENGL_LIBRARIES}
		GLEW::GLEW
		)