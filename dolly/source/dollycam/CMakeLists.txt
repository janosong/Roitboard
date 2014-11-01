cmake_minimum_required(VERSION 2.8)
set(PROJECT_NAME "dollycam")

project(${PROJECT_NAME})

#set uncommon find package scripts
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMAKE)
# find lib ghoto and set lib dependencies, includes
find_package(Gphoto2 REQUIRED)
if (NOT ${LIBGPHOTO2_FOUND})
	message(FATAL_ERROR "lib gphoto2 not found")
endif()
find_package(gSoap REQUIRED)
if (NOT ${LIBGSOAP_FOUND})
	message(FATAL_ERROR "lib gSoap not found")
endif()
find_package(Boost REQUIRED COMPONENTS thread system)
if (NOT ${LIBBOOST_FOUND})
	message(FATAL_ERROR "lib boost not found")
endif()
find_package(LibXml2 REQUIRED)
if (NOT ${LIBBOOST_FOUND})
	message(FATAL_ERROR "lib boost not found")
endif()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

#find package libgphoto2


add_subdirectory(src)
