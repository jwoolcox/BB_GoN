cmake_minimum_required(VERSION 2.7)
PROJECT(GameOfNeighbours)

set(GAME_NAME GameOfNeighbours)

if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    set(ARCH_DIR "x64" )
else( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    set(ARCH_DIR "x86" )
endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )

set(GAMEPLAY_SRC_PATH "../../GamePlay") 
set(GAMEPLAY_EXT_LIBS_PATH "${GAMEPLAY_SRC_PATH}/external-deps")

set(TARGET_OS "LINUX")
set(TARGET_OS_DIR "linux")

set(GAME_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/${TARGET_OS_DIR}")

macro (append_gameplay_lib listToAppend)
    set(libName gameplay)
    find_library(FOUND_LIB_${libName} ${libName} HINTS
        "${GAMEPLAY_SRC_PATH}/cmake/gameplay" "${GAMEPLAY_SRC_PATH}/build/gameplay" "${GAMEPLAY_SRC_PATH}/gameplay/src")
    set(${listToAppend} ${${listToAppend}} ${FOUND_LIB_${libName}})
endmacro(append_gameplay_lib)

macro (append_gameplay_ext_lib listToAppend libName libDirName)
    IF("${libDirName}" STREQUAL "")
        find_library(FOUND_LIB_${libName} ${libName})
    ELSE("${libDirName}" STREQUAL "")
        set(pathToSearch
            "${GAMEPLAY_EXT_LIBS_PATH}/${libDirName}/lib/${TARGET_OS_DIR}/${ARCH_DIR}")
        find_library(FOUND_LIB_${libName} ${libName} HINTS ${pathToSearch})
    ENDIF("${libDirName}" STREQUAL "")

    set(${listToAppend} ${${listToAppend}} ${FOUND_LIB_${libName}})
    message(STATUS "Library Found: ${libName} Path: ${FOUND_LIB_${libName}}")
endmacro (append_gameplay_ext_lib)

macro(copy_files TARGET_NAME GLOBPAT SOURCE DESTINATION)
  file(GLOB_RECURSE COPY_FILES
    RELATIVE ${SOURCE}
    "${SOURCE}/${GLOBPAT}")
  add_custom_target(${TARGET_NAME} ALL
    COMMENT "Copying files: ${SOURCE}/${GLOBPAT}")

  foreach(FILENAME ${COPY_FILES})
    set(SRC "${SOURCE}/${FILENAME}")
    set(DST "${DESTINATION}/${FILENAME}")

    add_custom_command(
      TARGET ${TARGET_NAME}
      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SRC} ${DST}
      )
  endforeach(FILENAME)
endmacro(copy_files)

include_directories( 
    ${GAMEPLAY_SRC_PATH}/gameplay/src
    ${GAMEPLAY_SRC_PATH}/external-deps/lua/include
    ${GAMEPLAY_SRC_PATH}/external-deps/bullet/include
    ${GAMEPLAY_SRC_PATH}/external-deps/libpng/include
    ${GAMEPLAY_SRC_PATH}/external-deps/oggvorbis/include
    ${GAMEPLAY_SRC_PATH}/external-deps/zlib/include
    ${GAMEPLAY_SRC_PATH}/external-deps/openal/include
    ${GAMEPLAY_SRC_PATH}/external-deps/glew/include
)

append_gameplay_lib(GAMEPLAY_LIBRARIES)
append_gameplay_ext_lib(GAMEPLAY_LIBRARIES "m" "" )
append_gameplay_ext_lib(GAMEPLAY_LIBRARIES "lua" "lua")
append_gameplay_ext_lib(GAMEPLAY_LIBRARIES "png" "libpng")
append_gameplay_ext_lib(GAMEPLAY_LIBRARIES "z" "zlib")
append_gameplay_ext_lib(GAMEPLAY_LIBRARIES "vorbis" "oggvorbis")
append_gameplay_ext_lib(GAMEPLAY_LIBRARIES "ogg" "oggvorbis")
append_gameplay_ext_lib(GAMEPLAY_LIBRARIES "BulletDynamics" "bullet")
append_gameplay_ext_lib(GAMEPLAY_LIBRARIES "BulletCollision" "bullet")
append_gameplay_ext_lib(GAMEPLAY_LIBRARIES "LinearMath" "bullet")
append_gameplay_ext_lib(GAMEPLAY_LIBRARIES "openal" "openal")
append_gameplay_ext_lib(GAMEPLAY_LIBRARIES "GLEW" "glew")
append_gameplay_ext_lib(GAMEPLAY_LIBRARIES "GL" "")
append_gameplay_ext_lib(GAMEPLAY_LIBRARIES "rt" "" )
append_gameplay_ext_lib(GAMEPLAY_LIBRARIES "dl" "")
append_gameplay_ext_lib(GAMEPLAY_LIBRARIES "X11" "")
append_gameplay_ext_lib(GAMEPLAY_LIBRARIES "pthread" "" )

add_definitions(-D__linux__)

source_group(res FILES ${GAME_RES} ${GAMEPLAY_RES} ${GAME_RES_SHADERS} ${GAME_RES_SHADERS_LIB})
source_group(src FILES ${GAME_SRC})

set(GAME_SRC
	src/gonAppMain.cpp
	src/gonAppMain.h
)

add_executable(${GAME_NAME}
    ${GAME_SRC}
)

target_link_libraries(${GAME_NAME} ${GAMEPLAY_LIBRARIES})

set_target_properties(${GAME_NAME} PROPERTIES
    OUTPUT_NAME "${GAME_NAME}"
    RUNTIME_OUTPUT_DIRECTORY "${GAME_OUTPUT_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${GAME_OUTPUT_DIR}"
)

#TODO: Copy res files to the bin dir, it is done that way so we can make post
#processing to the the res files in the future like zipping or preparation to
#per platfom format.
copy_files(CopyRes * "${CMAKE_SOURCE_DIR}/res" "${GAME_OUTPUT_DIR}/res")
copy_files(CopyConfig *.config "${CMAKE_SOURCE_DIR}" "${GAME_OUTPUT_DIR}")
