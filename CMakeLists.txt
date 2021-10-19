# Version check
cmake_minimum_required (VERSION 3.0)

if(NOT "${CMAKE_CXX_STANDARD}")
	set(CMAKE_CXX_STANDARD 17)
endif()

# Project
project(star-tape)

# Add library
add_library(star-tape STATIC 
	"src/star-tape/tape_archive.cpp"
	"src/star-tape/tape_convenience.cpp"
	"src/star-tape/tape_entry.cpp"
	"src/star-tape/tape_file_proxy.cpp"
	"src/star-tape/tape_header.cpp"
	"src/star-tape/tape_index.cpp"
	"src/star-tape/tape_operation.cpp"
	"src/star-tape/tape_writer.cpp"

	"src/star-tape/tape_io/tape_bzip2_stream.cpp"
	"src/star-tape/tape_io/tape_file.cpp"
	"src/star-tape/tape_io/tape_gzip_stream.cpp"
	"src/star-tape/tape_io/tape_stream.cpp"
)

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
	target_include_directories(star-tape PUBLIC ${BZIP2_INCLUDE_DIR})
	target_compile_definitions(star-tape PUBLIC "ENABLE_BZIP2=1")
	target_link_libraries(star-tape PUBLIC ${BZIP2_LIBRARIES})
endif()

if(ZLIB_FOUND)
	message("-- zlib found, adding to target")
	target_include_directories(star-tape PUBLIC ${ZLIB_INCLUDE_DIR})
	target_compile_definitions(star-tape PUBLIC "ENABLE_GZIP=1")	
	target_link_libraries(star-tape PUBLIC ${ZLIB_LIBRARIES})
endif()

target_link_libraries(star-tape PUBLIC ${libs} Boost::iostreams Boost::filesystem Boost::system)

target_include_directories(star-tape PUBLIC "include")

# Compiler Options
set(DEBUG_OPTIONS -fexceptions -g -Wall -pedantic-errors -pedantic)
set(RELEASE_OPTIONS -fexceptions -O3 -Wall -pedantic-errors -pedantic)
target_compile_options(star-tape PUBLIC "$<$<CONFIG:DEBUG>:${DEBUG_OPTIONS}>")
target_compile_options(star-tape PUBLIC "$<$<CONFIG:RELEASE>:${RELEASE_OPTIONS}>")