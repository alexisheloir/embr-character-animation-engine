SUBDIRS(Actuators MotionSegments)
project (REALIZER)
INCLUDE_DIRECTORIES(
	    ${ACTUATORS_SOURCE_DIR}
      ${SAMSARA_UTILS_SOURCE_DIR}
	    ${Boost_INCLUDE_DIR}
	    ${LIBXML2_INCLUDE_DIR}
	    ${SAMSARA_ICONV_INCLUDE_DIR}
	    ${SAMSARA_CORE_SOURCE_DIR}
	    ${SAMSARA_IO_SOURCE_DIR}
	    ${SAMSARA_MATH_SOURCE_DIR}
	    ${SAMSARA_NEWMAT_SOURCE_DIR}
	    ${MOTIONSEGMENTS_SOURCE_DIR}
	    ${LOG4CPLUS_INCLUDE_DIR}
      ${SAMSARA_COMMON_SOURCE_DIR}
      ${GLFW_INCLUDE_DIR}
      ${HORDE3D_INCLUDE_DIR}
)

ADD_LIBRARY(realizer
                 ActuatorFactory.cpp
                 SocketListener.cpp
                 Parser.cpp
                 Scheduler.cpp
                 Realizer.cpp
                 GestureModifier.cpp
                 SchedulerManager.cpp
                 Character.cpp
                 EMBRLoggers.h
                 CPointer.h
                 ActuatorFactory.h
                 SocketListener.h
                 Parser.h
                 Scheduler.h 
                 Realizer.h
                 GestureModifier.h
                 SchedulerManager.h
		             Character.h
)


LINK_DIRECTORIES(	${LIBRARY_OUTPUT_PATH}
	    		        ${Boost_LIBRARY_DIRS}
		)



ADD_EXECUTABLE(realizerBin 
			   main.cpp 
              )

ADD_EXECUTABLE(client 
			   client.cpp 
              )

ADD_EXECUTABLE(UDPListener 
			   UDPListener.cpp 
              )


TARGET_LINK_LIBRARIES(UDPListener ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${Boost_DATE_TIME_LIBRARY} ${LIBXML2_LIBRARIES}  ${ZLIB_LIBRARIES} ${SAMARA_ICONV_LIBRARIES} )

TARGET_LINK_LIBRARIES(client ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${Boost_DATE_TIME_LIBRARY} ${LIBXML2_LIBRARIES}  ${ZLIB_LIBRARIES} ${SAMARA_ICONV_LIBRARIES} )

IF(WIN32)
TARGET_LINK_LIBRARIES(realizerBin ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${Boost_DATE_TIME_LIBRARY} ${LIBXML2_LIBRARIES} ${LOG4CPLUS_LIBRARIES} io realizer actuators motionSegments newmat common core math utils)
ENDIF(WIN32)

IF(UNIX)
TARGET_LINK_LIBRARIES(realizerBin ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${Boost_DATE_TIME_LIBRARY} ${LIBXML2_LIBRARIES} ${LOG4CPLUS_LIBRARIES} io realizer actuators motionsegments newmat common core math utils ${ZLIB_LIBRARIES} ${SAMARA_ICONV_LIBRARIES})
ENDIF(UNIX)

IF(WIN32)
	TARGET_LINK_LIBRARIES(client Ws2_32.lib )
	TARGET_LINK_LIBRARIES(realizerBin Ws2_32.lib )
ENDIF(WIN32)

SUBDIRS(pythonBindings)

