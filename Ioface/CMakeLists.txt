add_library(Ioface)

set(IOFACE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Include/ CACHE INTERNAL "")
set(INTRAFACE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/intraface)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(dFLAGS "d")
else()
	set(dFLAGS "")
endif()

set(OPENCV_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/opencv-3.0.0)
set(OPENCV_LIBRARIES
	${OPENCV_DIR}/lib/${CMAKE_BUILD_TYPE}/opencv_highgui300${dFLAGS}.lib
	${OPENCV_DIR}/lib/${CMAKE_BUILD_TYPE}/opencv_objdetect300${dFLAGS}.lib
	${OPENCV_DIR}/lib/${CMAKE_BUILD_TYPE}/opencv_core300${dFLAGS}.lib
	${OPENCV_DIR}/lib/${CMAKE_BUILD_TYPE}/opencv_imgproc300${dFLAGS}.lib
	${OPENCV_DIR}/lib/${CMAKE_BUILD_TYPE}/opencv_videoio300${dFLAGS}.lib
	${OPENCV_DIR}/lib/${CMAKE_BUILD_TYPE}/opencv_calib3d300${dFLAGS}.lib
)

target_sources(Ioface
PRIVATE
	Source/Ioface.cpp
	${IOFACE_INCLUDE_DIR}/Ioface/Ioface.hpp
)

target_include_directories(Ioface
PUBLIC
	${IOFACE_INCLUDE_DIR}
	${OPENCV_DIR}/include
	${INTRAFACE_DIR}/include
)

target_link_libraries(Ioface
	${OPENCV_LIBRARIES}
	${INTRAFACE_DIR}/lib/Release/IntraFaceDLL.lib
)

add_custom_command(TARGET Ioface POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${OPENCV_DIR}/bin/${CMAKE_BUILD_TYPE} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${INTRAFACE_DIR}/bin/Release ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Assets ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Assets
)