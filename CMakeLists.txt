cmake_minimum_required(VERSION 3.6)

### Project information
project(TheLastCityOnEarth)
set(TheLastCityOnEarth_VERSION_MAJOR 1)
set(TheLastCityOnEarth_VERSION_MINOR 0)
set(TARGET_NAME TheLastCityOnEarth)

### Source information
set(ROOT_SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(GAME_DIR ${CMAKE_SOURCE_DIR}/src/game)
set(RENDERABLES_DIR ${CMAKE_SOURCE_DIR}/src/game/renderables)
set(MODELS_DIR ${CMAKE_SOURCE_DIR}/src/game/renderables/models)
set(PRIMITIVES_DIR ${CMAKE_SOURCE_DIR}/src/game/renderables/primitives)
set(RENDERER_DIR ${CMAKE_SOURCE_DIR}/src/renderer)
set(SCREENS_DIR ${CMAKE_SOURCE_DIR}/src/screens)
set(SOUND_DIR ${CMAKE_SOURCE_DIR}/src/sound)

set(ALL_SRCS)
function(add_directory path)
	include_directories(${path})
	file(GLOB DIR_SRCS ${path}/*.cpp ${path}/*.h)
	get_filename_component(LAST_SRC_DIR "${path}" NAME)
	source_group(${LAST_SRC_DIR} FILES ${DIR_SRCS})
	set(ALL_SRCS ${ALL_SRCS} ${DIR_SRCS} PARENT_SCOPE)
endfunction(add_directory)

add_directory(${ROOT_SRC_DIR})
add_directory(${GAME_DIR})
add_directory(${RENDERABLES_DIR})
add_directory(${MODELS_DIR})
add_directory(${PRIMITIVES_DIR})
add_directory(${RENDERER_DIR})
add_directory(${SCREENS_DIR})
add_directory(${SOUND_DIR})

# Shaders
file(GLOB SHADER_FILES ${CMAKE_SOURCE_DIR}/res/shaders/*.vert ${CMAKE_SOURCE_DIR}/res/shaders/*.frag)
source_group("shaders" FILES ${SHADER_FILES})

### Libraries and headers
set(LIB_DIR "${CMAKE_SOURCE_DIR}/lib")

# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
set(LIBS ${LIBS} ${OPENGL_LIBRARIES})

# Assimp
set(DEFAULT_ASSIMP_DIR "assimp-3.1.1")
find_path(ASSIMP_HEADERS_DIR assimp/config.h ${LIB_DIR}/${DEFAULT_ASSIMP_DIR}/include) # args: found_path, filename, possible_paths)
find_library(ASSIMP_LIB assimp ${LIB_DIR}/${DEFAULT_ASSIMP_DIR}) # args: found_lib, libname, possible_paths)
if(ASSIMP_HEADERS_DIR AND ASSIMP_LIB)
	set(LIBS ${LIBS} ${ASSIMP_LIB})
	include_directories( ${ASSIMP_HEADERS_DIR} )
else()
	message(SEND_ERROR "Assimp was not found.")
endif()

# GLEW
set(DEFAULT_GLEW_DIR "glew-1.10.0")
find_path(GLEW_HEADERS_DIR GL/glew.h ${LIB_DIR}/${DEFAULT_GLEW_DIR}/include)
find_library(GLEW_LIB glew32 ${LIB_DIR}/${DEFAULT_GLEW_DIR})
if(GLEW_HEADERS_DIR AND GLEW_LIB)
	set(LIBS ${LIBS} ${GLEW_LIB})
	include_directories( ${GLEW_HEADERS_DIR} )
else()
	message(SEND_ERROR "Glew was not found.")
endif()

# GLFW
set(DEFAULT_GLFW_DIR "glfw-3.1.1")
find_path(GLFW_HEADERS_DIR GLFW/glfw3.h ${LIB_DIR}/${DEFAULT_GLFW_DIR}/include)
find_library(GLFW_LIB glfw3 ${LIB_DIR}/${DEFAULT_GLFW_DIR})
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
find_library(FREEIMAGE_LIB FreeImage ${LIB_DIR}/${DEFAULT_FREEIMAGE_DIR})
if(FREEIMAGE_HEADERS_DIR AND FREEIMAGE_LIB)
	set(LIBS ${LIBS} ${FREEIMAGE_LIB})
	include_directories( ${FREEIMAGE_HEADERS_DIR} )
else()
	message(SEND_ERROR "FreeImage was not found.")
endif()

# FreeImage
set(DEFAULT_IRRKLANG_DIR "irrKlang-1.5.0")
find_path(IRRKLANG_HEADERS_DIR irrKlang.h ${LIB_DIR}/${DEFAULT_IRRKLANG_DIR}/include)
message(STATUS "${LIB_DIR}/${DEFAULT_IRRKLANG_DIR}/include")
find_library(IRRKLANG_LIB irrKlang ${LIB_DIR}/${DEFAULT_IRRKLANG_DIR})
if(IRRKLANG_HEADERS_DIR AND IRRKLANG_LIB)
	set(LIBS ${LIBS} ${IRRKLANG_LIB})
	include_directories( ${IRRKLANG_HEADERS_DIR} )
else()
	message(SEND_ERROR "irrKlang was not found.")
endif()

### Link
add_executable(${TARGET_NAME} ${ALL_SRCS} ${SHADER_FILES})
target_link_libraries(${TARGET_NAME} ${LIBS})

### Copying DLL target
file(GLOB DLLS ${CMAKE_SOURCE_DIR}/dll/*.dll)
add_custom_target(CopyDlls)
foreach(DLL ${DLLS})	
  add_custom_command(TARGET CopyDlls POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${DLL} ${CMAKE_BINARY_DIR}/Release/)
  add_custom_command(TARGET CopyDlls POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${DLL} ${CMAKE_BINARY_DIR}/Debug/)
endforeach()
add_dependencies(${TARGET_NAME} CopyDlls)

### Copying resources target
add_custom_target(CopyResources)
add_custom_command(TARGET CopyResources POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res ${CMAKE_BINARY_DIR}/Debug/data)
add_custom_command(TARGET CopyResources POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res ${CMAKE_BINARY_DIR}/data)
add_custom_command(TARGET CopyResources POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res ${CMAKE_BINARY_DIR}/Release/data)
add_dependencies(${TARGET_NAME} CopyResources)

### Copying shaders target
add_custom_target(CopyShaders ALL DEPENDS ${SHADER_FILES})
add_custom_command(TARGET CopyShaders COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/res/shaders" "${CMAKE_BINARY_DIR}/Release/data/shaders")
add_custom_command(TARGET CopyShaders COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/res/shaders" "${CMAKE_BINARY_DIR}/data/shaders")