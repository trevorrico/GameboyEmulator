cmake_minimum_required(VERSION "3.22.1")

project("thirdparty")

set(IMGUI_PATH ${CMAKE_CURRENT_LIST_DIR}/imgui)

add_library(${PROJECT_NAME} STATIC glad/src/glad.c
	${IMGUI_PATH}/imgui.cpp
	${IMGUI_PATH}/imgui_demo.cpp
	${IMGUI_PATH}/imgui_draw.cpp
	${IMGUI_PATH}/imgui_impl_glfw.cpp
	${IMGUI_PATH}/imgui_impl_opengl3.cpp
	${IMGUI_PATH}/imgui_tables.cpp
	${IMGUI_PATH}/imgui_widgets.cpp
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(glfw)

target_link_libraries(${PROJECT_NAME} glfw)

target_include_directories(${PROJECT_NAME} 
	PUBLIC 
		glad/include/ 
		glm/ 
		imgui/
		portable_file_dialogs/
)

# Fix imm 
if(NOT (CMAKE_GENERATOR MATCHES "Visual Studio"))
	target_link_libraries(${PROJECT_NAME} imm32.lib)
endif()