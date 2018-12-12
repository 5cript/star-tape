# Version check
cmake_minimum_required (VERSION 3.0)

set(star-tape "star_tape")

if(NOT "${CMAKE_CXX_STANDARD}")
	set(CMAKE_CXX_STANDARD 17)
endif()

# Project
project(star-tape)

# Add files
file(GLOB sources "*.cpp" "tape_io/*.cpp")

# Add library
add_library(${star-tape} STATIC ${sources})

find_package(Boost REQUIRED COMPONENTS
	filesystem
	system
	iostreams
)

find_package(BZip2)
find_package(ZLIB)

set(libs "")

# Compression
if(BZIP2_FOUND)
	message("-- BZip2 found, adding to target")
	target_include_directories(${star-tape} PUBLIC ${BZIP2_INCLUDE_DIR})
	target_compile_definitions(${star-tape} PUBLIC "ENABLE_BZIP2=1")
	
	set(libs "${libs}${BZIP2_LIBRARIES}")
endif()

if(ZLIB_FOUND)
	message("-- zlib found, adding to target")
	target_include_directories(${star-tape} PUBLIC ${ZLIB_INCLUDE_DIR})
	target_compile_definitions(${star-tape} PUBLIC "ENABLE_GZIP=1")
	
	set(libs "${libs}${ZLIB_LIBRARIES}")
endif()

target_link_libraries(${star-tape} ${libs} Boost::iostreams Boost::filesystem Boost::system)

# Compiler Options
set(DEBUG_OPTIONS "-fexceptions -g -Wall -pedantic-errors -pedantic")
set(RELEASE_OPTIONS "-fexceptions -O3 -Wall -pedantic-errors -pedantic")
target_compile_options(${star-tape} PUBLIC "$<$<CONFIG:DEBUG>:${DEBUG_OPTIONS}>")
target_compile_options(${star-tape} PUBLIC "$<$<CONFIG:RELEASE>:${RELEASE_OPTIONS}>")