cmake_minimum_required( VERSION 3.0 )

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/build )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/build )

project( game )

add_subdirectory( thirdparty/lodepng )

include_directories( thirdparty )
add_subdirectory( src )
