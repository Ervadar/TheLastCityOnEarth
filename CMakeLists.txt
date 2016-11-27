cmake_minimum_required(VERSION 3.6)

### Project information
project(TheLastCityOnEarth)
set(TheLastCityOnEarth_VERSION_MAJOR 1)
set(TheLastCityOnEarth_VERSION_MINOR 0)
set(TARGET_NAME TheLastCityOnEarth)

### Source information
set(ROOT_SRC_DIR "src")
set(GAME_DIR "src/game")
set(RENDERABLES_DIR "src/game/renderables")
set(MODELS_DIR "src/game/renderables/models")
set(PRIMITIVES_DIR "src/game/renderables/primitives")
set(PARTICLES_DIR "src/game/renderables/particles")
set(RENDERER_DIR "src/renderer")
set(SCREENS_DIR "src/screens")
set(SOUND_DIR "src/sound")

set(ALL_SRCS)
function(add_directory directory)
	include_directories(${CMAKE_SOURCE_DIR}/${directory})
	file(GLOB DIR_SRCS ${CMAKE_SOURCE_DIR}/${directory}/*.cpp ${CMAKE_SOURCE_DIR}/${directory}/*.h)
	string(REPLACE "/" "\\" directory_out ${directory})
	source_group(${directory_out} FILES ${DIR_SRCS})
	set(ALL_SRCS ${ALL_SRCS} ${DIR_SRCS} PARENT_SCOPE)
endfunction(add_directory)

add_directory(${ROOT_SRC_DIR})
add_directory(${GAME_DIR})
add_directory(${RENDERABLES_DIR})
add_directory(${MODELS_DIR})
add_directory(${PRIMITIVES_DIR})
add_directory(${PARTICLES_DIR})
add_directory(${RENDERER_DIR})
add_directory(${SCREENS_DIR})
add_directory(${SOUND_DIR})

# Shaders
file(GLOB SHADER_FILES ${CMAKE_SOURCE_DIR}/res/shaders/*.vert ${CMAKE_SOURCE_DIR}/res/shaders/*.frag)
source_group("shaders" FILES ${SHADER_FILES})

### Libraries and headers
set(LIB_DIR "lib")

if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
	set( TARGET_SYSTEM "win64" )
else( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    set( TARGET_SYSTEM "win32" )
endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )

# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
set(LIBS ${LIBS} ${OPENGL_LIBRARIES})

# Assimp
set(DEFAULT_ASSIMP_DIR "assimp-3.1.1")
find_path(ASSIMP_HEADERS_DIR assimp/config.h ${LIB_DIR}/${DEFAULT_ASSIMP_DIR}/include) # args: found_path, filename, possible_paths)
find_library(ASSIMP_LIB assimp ${LIB_DIR}/${DEFAULT_ASSIMP_DIR}/${TARGET_SYSTEM}) # args: found_lib, libname, possible_paths)
if(ASSIMP_HEADERS_DIR AND ASSIMP_LIB)
	set(LIBS ${LIBS} ${ASSIMP_LIB})
	include_directories( ${ASSIMP_HEADERS_DIR} )
else()
	message(SEND_ERROR "Assimp was not found.")
endif()

# GLEW
set(DEFAULT_GLEW_DIR "glew-1.10.0")
find_path(GLEW_HEADERS_DIR GL/glew.h ${LIB_DIR}/${DEFAULT_GLEW_DIR}/include)
find_library(GLEW_LIB glew32 ${LIB_DIR}/${DEFAULT_GLEW_DIR}/${TARGET_SYSTEM})
if(GLEW_HEADERS_DIR AND GLEW_LIB)
	set(LIBS ${LIBS} ${GLEW_LIB})
	include_directories( ${GLEW_HEADERS_DIR} )
else()
	message(SEND_ERROR "Glew was not found.")
endif()

# GLFW
set(DEFAULT_GLFW_DIR "glfw-3.1.1")
find_path(GLFW_HEADERS_DIR GLFW/glfw3.h ${LIB_DIR}/${DEFAULT_GLFW_DIR}/include)
find_library(GLFW_LIB glfw3 ${LIB_DIR}/${DEFAULT_GLFW_DIR}/${TARGET_SYSTEM})
if(GLFW_HEADERS_DIR AND GLFW_LIB)
	set(LIBS ${LIBS} ${GLFW_LIB})
	include_directories( ${GLFW_HEADERS_DIR} )
else()
	message(SEND_ERROR "GLFW was not found.")
endif()

# GLM
set(DEFAULT_GLM_DIR "glm")
find_path(GLM_HEADERS_DIR glm/glm.hpp ${LIB_DIR}/${DEFAULT_GLM_DIR})
if(GLM_HEADERS_DIR)
	include_directories( ${GLM_HEADERS_DIR} )
else()
	message(SEND_ERROR "GLM was not found.")
endif()

# FreeImage
set(DEFAULT_FREEIMAGE_DIR "FreeImage")
find_path(FREEIMAGE_HEADERS_DIR FreeImage.h ${LIB_DIR}/${DEFAULT_FREEIMAGE_DIR}/include)
find_library(FREEIMAGE_LIB FreeImage ${LIB_DIR}/${DEFAULT_FREEIMAGE_DIR}/${TARGET_SYSTEM})
if(FREEIMAGE_HEADERS_DIR AND FREEIMAGE_LIB)
	set(LIBS ${LIBS} ${FREEIMAGE_LIB})
	include_directories( ${FREEIMAGE_HEADERS_DIR} )
else()
	message(SEND_ERROR "FreeImage was not found.")
endif()

# IrrKlang
set(DEFAULT_IRRKLANG_DIR "irrKlang-1.5.0")
find_path(IRRKLANG_HEADERS_DIR irrKlang.h ${LIB_DIR}/${DEFAULT_IRRKLANG_DIR}/include)
find_library(IRRKLANG_LIB irrKlang ${LIB_DIR}/${DEFAULT_IRRKLANG_DIR}/${TARGET_SYSTEM})
if(IRRKLANG_HEADERS_DIR AND IRRKLANG_LIB)
	set(LIBS ${LIBS} ${IRRKLANG_LIB})
	include_directories( ${IRRKLANG_HEADERS_DIR} )
else()
	message(SEND_ERROR "irrKlang was not found.")
endif()

# Json
set(DEFAULT_JSON_DIR "json")
find_path(JSON_HEADERS_DIR json.hpp ${LIB_DIR}/${DEFAULT_JSON_DIR})
if(JSON_HEADERS_DIR)
	include_directories(${JSON_HEADERS_DIR})
else()
	message(SEND_ERROR "json was not found.")
endif()

### Link
add_executable(${TARGET_NAME} ${ALL_SRCS} ${SHADER_FILES})
target_link_libraries(${TARGET_NAME} ${LIBS})

if(MSVC)
	### Setting target as startup project
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TARGET_NAME})

	### Copying DLL target
	file(GLOB DLLS ${CMAKE_SOURCE_DIR}/dll/${TARGET_SYSTEM}/*.dll)
	add_custom_target(CopyDlls ALL DEPENDS ${CMAKE_BINARY_DIR}/unexisting_file.h)
	foreach(DLL ${DLLS})	
	  add_custom_command(TARGET CopyDlls POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${DLL} ${CMAKE_BINARY_DIR}/$<CONFIGURATION>/)
	endforeach()

	### Copying resources target
	add_custom_target(CopyResources ALL DEPENDS ${CMAKE_BINARY_DIR}/unexisting_file.h)
	add_custom_command(TARGET CopyResources POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res ${CMAKE_BINARY_DIR}/$<CONFIGURATION>/data)
	add_custom_command(TARGET CopyResources POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res ${CMAKE_BINARY_DIR}/data)
endif()