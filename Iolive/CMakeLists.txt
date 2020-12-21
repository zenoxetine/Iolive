if (${IOLIVE_DEBUG})
	add_definitions(-DIOLIVE_DEBUG=1)
else()
	add_definitions(-DIOLIVE_DEBUG=0)
	set(USE_SUBSYSTEM_WINDOWS WIN32)
endif()

add_executable(Iolive ${USE_SUBSYSTEM_WINDOWS})

set(IOLIVE_VENDOR_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Vendor CACHE INTERNAL "")
set(GLFW_PATH ${IOLIVE_VENDOR_PATH}/glfw)
set(GLEW_PATH ${IOLIVE_VENDOR_PATH}/glew)

# find opengl
find_package(OpenGL REQUIRED)

# surpress GLEW and GLFW process.
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
# add GLFW.
add_subdirectory(${GLFW_PATH} ${CMAKE_CURRENT_BINARY_DIR}/glfw)

# add static glew library
add_definitions(-DGLEW_STATIC)
add_library(glew_s STATIC IMPORTED)
set_target_properties(glew_s
PROPERTIES
	IMPORTED_LOCATION_DEBUG
		${GLEW_PATH}/lib/Debug/libglew32d.lib
    IMPORTED_LOCATION_RELEASE
		${GLEW_PATH}/lib/Release/libglew32.lib
    INTERFACE_INCLUDE_DIRECTORIES ${GLEW_PATH}/include
)

# build cubism framework
include(BuildCubism.cmake)

# add imgui sources
file(GLOB_RECURSE IMGUI_SOURCES CONFIGURE_DEPENDS
	${IOLIVE_VENDOR_PATH}/imgui/*.cpp
	${IOLIVE_VENDOR_PATH}/imgui/*.h
)

target_sources(Iolive
PRIVATE
	# cpp files
	Source/Main.cpp
	Source/Application.cpp
	Source/Window.cpp
	Source/Live2D/Live2DManager.cpp
	Source/Live2D/Utility.cpp
	Source/Live2D/Component/TextureManager.cpp
	Source/GUI/IoliveGui.cpp
	Source/GUI/Utility.cpp

	# header files
	Source/Application.hpp
	Source/Window.hpp
	Source/IofaceBridge.hpp
	Source/MathUtils.hpp
	Source/Live2D/Live2DManager.hpp
	Source/Live2D/Model2D.hpp
	Source/Live2D/Utility.hpp
	Source/Live2D/Component/TextureManager.hpp
	Source/Live2D/CubismSamples/LAppAllocator.hpp
	Source/GUI/IoliveGui.hpp
	Source/GUI/Utility.hpp
	Source/GUI/Widget/MainLWidget.hpp
	Source/GUI/Widget/MainRWidget.hpp
	Source/GUI/Widget/Component/ParameterValue.hpp
	Source/GUI/Widget/Component/Checkbox.hpp

	# ImGui file
	${IMGUI_SOURCES}
)

target_include_directories(Iolive
PRIVATE
	${OPENGL_INCLUDE_DIRS}
	${IOFACE_INCLUDE_DIR}
	${IOLIVE_VENDOR_PATH}/stb/ # header only file
	${IOLIVE_VENDOR_PATH}/imgui/
)

target_link_libraries(Iolive
PRIVATE
	${OPENGL_LIBRARIES}
	Ioface
	glew_s	  # static glew
	glfw
	Framework # cubism framework
)

add_custom_command(TARGET Iolive POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Assets ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Assets
)