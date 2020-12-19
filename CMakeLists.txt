cmake_minimum_required(VERSION 3.16)

# using c++17
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS_RELEASE "-O2")

# set output dir for binary & lib files
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

project(Iolive)

OPTION(IOLIVE_DEBUG
	"ON  = window + console
	 OFF = window only"
	OFF
)
message("IOLIVE_DEBUG = ${IOLIVE_DEBUG}")

if(CMAKE_EXE_LINKER_FLAGS STREQUAL "/machine:x64")
	set(ARCH x64)
else()
	message(FATAL_ERROR "Unsupported architecture ${CMAKE_EXE_LINKER_FLAGS}.")
endif()

add_subdirectory(Ioface)
add_subdirectory(Iolive)