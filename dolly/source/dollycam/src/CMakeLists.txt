set(EXECUTABLE_NAME dollycam)
set(EXECUTABLE_SOURCES
	soap/soapServer.c
	soap/soapC.c
	dollycam.cpp
	camera.cpp
	cameraConfig.cpp
	webService.cpp
	robo.cpp
)

set(EXECUTABLE_HEADERS
	camera.h
	robo.h
	cameraConfig.h
	webService.h
)

add_executable(${EXECUTABLE_NAME} ${EXECUTABLE_SOURCES})

set(EXECUTABLE_LINK_LIBRARIES
	${LIBGPHOTO2_LIBRARIES}
	${GSOAP_LIBRARIES}
	${Boost_LIBRARIES}
	${LIBXML2_LIBRARIES}
)
#	/usr/local/lib/libgphoto2_port.so
include_directories(
	${LIBGPHOTO2_INCLUDE_DIR}
	${BOOST_INCLUDE_DIR}
	${LIBXML2_INCLUDE_DIR}
	"./soap"
)
set (CMAKE_CXX_FLAGS "-g") 
message(${EXECUTABLE_LINK_LIBRARIES})

target_link_libraries(${EXECUTABLE_NAME} ${EXECUTABLE_LINK_LIBRARIES})
# add_dependencies(${EXECUTABLE_NAME})
