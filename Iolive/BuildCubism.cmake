if (${CMAKE_VERSION} GREATER "3.13")
	cmake_policy(SET CMP0079 NEW)
else()
	message(FATAL_ERROR "[BuildCubism] CMake version not supported, please using cmake > 3.13")
endif()

set(CORE_PATH ${IOLIVE_VENDOR_PATH}/Live2DCubismCore)
set(FRAMEWORK_PATH ${IOLIVE_VENDOR_PATH}/CubismNativeFramework)

option(CORE_CRL_MD
	"Use Cubism Core that is multithread-specific and DLL-specific version"
	ON
)

if(CORE_CRL_MD)
	set(CRT MD)
else()
	set(CRT MT)
endif()

# Detect Compiler
# only compile in visual studio 2017 & 2019
if(MSVC_VERSION GREATER_EQUAL 1910 AND MSVC_VERSION LESS 1920)
	# Visual Studio 2017
	set(COMPILER 141)
elseif(MSVC_VERSION GREATER_EQUAL 1928)
	# Visual Studio 2019
	set(COMPILER 142)
elseif(MSVC)
	message(FATAL_ERROR "[BuildCubism] Unsupported Visual C++ compiler used (${MSVC_VERSION}).")
else()
	message(FATAL_ERROR "[BuildCubism] Unsupported compiler used.")
endif()

# check architecture again
if (${ARCH} MATCHES x64)
	set(CORE_LIB_SUFFIX ${CORE_PATH}/lib/windows/x86_64/${COMPILER})
else()
	message(FATAL_ERROR "[BuildCubism] Unsupported architecture ${CMAKE_EXE_LINKER_FLAGS}.")
endif()

# Add Cubism Core.
# Import as static library.
add_library(Live2DCubismCore STATIC IMPORTED)

set_target_properties(Live2DCubismCore
PROPERTIES
	IMPORTED_LOCATION_DEBUG
		${CORE_LIB_SUFFIX}/Live2DCubismCore_${CRT}d.lib
    IMPORTED_LOCATION_RELEASE
		${CORE_LIB_SUFFIX}/Live2DCubismCore_${CRT}.lib
    INTERFACE_INCLUDE_DIRECTORIES ${CORE_PATH}/include
)

# Specify Cubism Framework rendering.
set(FRAMEWORK_SOURCE OpenGL)
# Add Cubism Native Framework.
add_subdirectory(${FRAMEWORK_PATH} ${CMAKE_CURRENT_BINARY_DIR}/Framework)
# Add rendering definition to framework.
target_compile_definitions(Framework PUBLIC CSM_TARGET_WIN_GL)
# Add include path of GLEW to framework.
target_include_directories(Framework PUBLIC ${GLEW_PATH}/include)
# Link libraries to framework.
target_link_libraries(Framework
	Live2DCubismCore
	glew_s
)