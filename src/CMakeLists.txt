cmake_minimum_required (VERSION 3.8)

SET(LIB_PATH "${CMAKE_SOURCE_DIR}/../vcpkg/installed/x86-windows/debug/lib")
SET(INC_PATH "${CMAKE_SOURCE_DIR}/../vcpkg/installed/x86-windows/include")
SET(VCPKG_PATH "${CMAKE_SOURCE_DIR}/../vcpkg/installed/x86-windows/debug/bin")
SET(VCPKG_REL "${CMAKE_SOURCE_DIR}/../vcpkg/installed/x86-windows/bin")
find_library(SFML-SYSTEM sfml-system-d PATHS "${LIB_PATH}" )
find_library(SFML-GRAPHICS sfml-graphics-d PATHS "${LIB_PATH}")
find_library(SFML-WINDOW sfml-window-d PATHS "${LIB_PATH}")
find_library(SFML-AUDIO sfml-audio-d PATHS "${LIB_PATH}")

message(SFML-SYSTEM=${SFML-SYSTEM})
find_path(SFML_INCLUDE_DIR SFML/Graphics.hpp PATHS "${INC_PATH}")
find_path(SFML_BIN_PATH sfml-system-d-2.dll PATHS ${VCPKG_PATH} )
find_path(SFML_BIN_REL sfml-system-2.dll PATHS ${VCPKG_REL} )
message(SFML_BIN_PATH=${SFML_BIN_PATH})
message(SFML_BIN_REL=${SFML_BIN_REL})
message(SFML_INCLUDE_DIR=${SFML_INCLUDE_DIR})
add_executable (arkanoid main.cpp)
add_custom_target(SFMLCopyBinaries
	# mandantory stuff
	COMMAND ${CMAKE_COMMAND} -E copy ${SFML_BIN_PATH}/sfml-system-d-2.dll  ${CMAKE_BINARY_DIR}/src
	COMMAND ${CMAKE_COMMAND} -E copy ${SFML_BIN_PATH}/sfml-graphics-d-2.dll  ${CMAKE_BINARY_DIR}/src
	COMMAND ${CMAKE_COMMAND} -E copy ${SFML_BIN_PATH}/sfml-window-d-2.dll  ${CMAKE_BINARY_DIR}/src
	COMMAND ${CMAKE_COMMAND} -E copy ${SFML_BIN_PATH}/sfml-audio-d-2.dll  ${CMAKE_BINARY_DIR}/src
	COMMAND ${CMAKE_COMMAND} -E copy ${SFML_BIN_PATH}/freetyped.dll  ${CMAKE_BINARY_DIR}/src
	COMMAND ${CMAKE_COMMAND} -E copy ${SFML_BIN_PATH}/jpeg62.dll  ${CMAKE_BINARY_DIR}/src
	COMMAND ${CMAKE_COMMAND} -E copy ${SFML_BIN_PATH}/libbz2d.dll  ${CMAKE_BINARY_DIR}/src
	COMMAND ${CMAKE_COMMAND} -E copy ${SFML_BIN_PATH}/libpng16d.dll  ${CMAKE_BINARY_DIR}/src
	COMMAND ${CMAKE_COMMAND} -E copy ${SFML_BIN_PATH}/zlibd1.dll  ${CMAKE_BINARY_DIR}/src
	# audio stuff
	COMMAND ${CMAKE_COMMAND} -E copy ${SFML_BIN_PATH}/vorbis.dll  ${CMAKE_BINARY_DIR}/src
	COMMAND ${CMAKE_COMMAND} -E copy ${SFML_BIN_PATH}/vorbisenc.dll  ${CMAKE_BINARY_DIR}/src
	COMMAND ${CMAKE_COMMAND} -E copy ${SFML_BIN_PATH}/vorbisfile.dll  ${CMAKE_BINARY_DIR}/src
	COMMAND ${CMAKE_COMMAND} -E copy ${SFML_BIN_PATH}/OpenAL32.dll  ${CMAKE_BINARY_DIR}/src
	COMMAND ${CMAKE_COMMAND} -E copy ${SFML_BIN_PATH}/ogg.dll  ${CMAKE_BINARY_DIR}/src
	COMMAND ${CMAKE_COMMAND} -E copy ${SFML_BIN_PATH}/flac.dll  ${CMAKE_BINARY_DIR}/src
	# resources
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/res/arial.ttf  ${CMAKE_BINARY_DIR}/src
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/res/chord.wav  ${CMAKE_BINARY_DIR}/src
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/res/block.wav  ${CMAKE_BINARY_DIR}/src
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/res/stop.wav  ${CMAKE_BINARY_DIR}/src
	
	COMMENT "Copying SFML binaries from '${SFML_BIN_PATH}' to '${CMAKE_BINARY_DIR}/src'" VERBATIM
)
add_dependencies(arkanoid SFMLCopyBinaries)
# C++11, c++14
target_compile_features(arkanoid PRIVATE cxx_lambda_init_captures)
target_include_directories(arkanoid PRIVATE ${SFML_INCLUDE_DIR})
target_link_libraries(arkanoid PRIVATE ${SFML-SYSTEM} ${SFML-GRAPHICS} ${SFML-WINDOW} ${SFML-AUDIO} )


# Add source to this project's executable.



