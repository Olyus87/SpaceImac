cmake_minimum_required(VERSION 2.8)

project(SpacImac)

add_definitions(-std=c++14)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

find_package(SDL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(
	${SDL_INCLUDE_DIRS}
	${OPENGL_INCLUDE_DIRS}
	${GLEW_INCLUDE_DIRS}
	third-party/include
	glimac/include
	app/include
)

IF (WIN32)
	set(LIBRARIES
		mingw32
		${SDL_LIBRARY}
		glew32s
		opengl32
		glu32
		freeglut
		glimac
	)
	add_definitions(-DGLEW_STATIC)
ELSE (WIN32)
	set(LIBRARIES
		${SDL_LIBRARY}
		${OPENGL_LIBRARY}
		${GLEW_LIBRARY}
		glimac
	)
ENDIF (WIN32)

subdirs(glimac)

file(
	GLOB_RECURSE
	SRC_FILES
	app/*.cpp app/*.hpp app/*.h app/*.glsl
)

add_executable(
	${PROJECT_NAME}
	${SRC_FILES}
)

target_link_libraries(
	${PROJECT_NAME}
	${LIBRARIES}
)

file(REMOVE_RECURSE ${CMAKE_CURRENT_BINARY_DIR}/bin/shaders)
file(COPY app/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin)

file(REMOVE_RECURSE ${CMAKE_CURRENT_BINARY_DIR}/bin/assets)
file(COPY app/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin)
