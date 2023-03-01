cmake_minimum_required(VERSION 3.19)
project(omp-net LANGUAGES C CXX VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE source_list CONFIGURE_DEPENDS
    "src/*.hpp"
	"src/*.cpp"
    "*.h"
	"*.cpp"
	"includes/native/*.h"
)

add_subdirectory(includes/sdk)
add_subdirectory(includes/network)

include_directories(
	.
	includes
)

add_library(${PROJECT_NAME} SHARED main.cpp ${source_list})

target_link_libraries(${PROJECT_NAME} PRIVATE
    OMP-SDK
	OMP-Network
	${CMAKE_SOURCE_DIR}/includes/native/libnethost.a
)
