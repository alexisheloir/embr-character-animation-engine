#
#  DFKI Realizer initial Cmake project file 
#  author: Alexis Heloir
#  Date of birth: September, 24 2008 
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET (CMAKE_VERBOSE_MAKEFILE true)

project(EMBR)

SET(CMAKE_BUILD_TYPE Release)

SET(LOG4CPLUS_INCLUDE_DIR ${SAMSARA_SOURCE_DIR}/thirdParty/Universal/log4cplus-1.2.0/include)


IF(WIN32)
  IF(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    SET(LOG4CPLUS_LIBRARIES log4cplusSD) #U for Unicode, S for Static, D for Debug
  ELSE(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    SET(LOG4CPLUS_LIBRARIES log4cplusS) #U for Unicode, S for Static, D for Debug
  ENDIF(${CMAKE_BUILD_TYPE} MATCHES "Debug")
ELSE(WIN32)
  IF(APPLE)
    SET(LOG4CPLUS_LIBRARIES log4cplus)
  ELSE(APPLE)	
    IF(${CMAKE_BUILD_TYPE} MATCHES "Debug")
      SET(LOG4CPLUS_LIBRARIES /home/ppohl/Smr/Smr/linuxBuild/lib/Release/liblog4cplus.so) #U for Unicode, S for Static, D for Debug
    ELSE(${CMAKE_BUILD_TYPE} MATCHES "Debug")
      SET(LOG4CPLUS_LIBRARIES /home/ppohl/Smr/Smr/linuxBuild/lib/Release/liblog4cplus.so) #U for Unicode, S for Static, D for Debug
    ENDIF(${CMAKE_BUILD_TYPE} MATCHES "Debug")
  ENDIF(APPLE)
ENDIF(WIN32)

ADD_DEFINITIONS (-DLOG4CPLUS_STATIC)

IF(APPLE)
  add_definitions(-D__MWERKS__)
  add_definitions(-D__MACOS__)
ENDIF(APPLE)

SET(SAMSARA_HOME_DIR
    CACHE PATH
    "Path to the SAMSARA hone dir")

SET(SAMSARA_SOURCE_DIR
    CACHE PATH
    "Path to the SAMSARA source dir")

SET(GAME_PATH
    CACHE PATH
    "Path to the EMBR game dir")

SET(SAMSARA_UTILS_SOURCE_DIR ${SAMSARA_SOURCE_DIR}/SRC/SmrUtils)
SET(SAMSARA_COMMON_SOURCE_DIR ${SAMSARA_SOURCE_DIR}/SRC/common)
SET(SAMSARA_CORE_SOURCE_DIR ${SAMSARA_SOURCE_DIR}/SRC/SmrCore)
SET(SAMSARA_IO_SOURCE_DIR ${SAMSARA_SOURCE_DIR}/SRC/SmrIO)
SET(SAMSARA_MATH_SOURCE_DIR ${SAMSARA_SOURCE_DIR}/SRC/SmrMath)
SET(SAMSARA_NEWMAT_SOURCE_DIR ${SAMSARA_SOURCE_DIR}/thirdParty/Universal/newmat)
SET(SAMSARA_ICONV_INCLUDE_DIR ${SAMSARA_SOURCE_DIR}/thirdParty/Universal/iconv/include)

INCLUDE(FindOpenGL)

SET(HORDE3D_INCLUDE_DIR ${SAMSARA_SOURCE_DIR}/thirdParty/Universal/Horde3D_SDK_1.0.0_Beta3/Horde3D/Bindings/C++)
SET(GLFW_INCLUDE_DIR ${SAMSARA_SOURCE_DIR}/thirdParty/Universal/glfw)

IF(APPLE)
 INCLUDE(FindLibXml2)
 INCLUDE(FindZlib)
 SET(ICONV_LIBRARIES /usr/lib/libiconv.dylib)
ELSE(APPLE)
ENDIF(APPLE)


IF(WIN32)

    SET(GUI "Win32")
    
    SET(GLUT_INCLUDE_DIR ${SAMSARA_SOURCE_DIR}/thirdParty/win32/include)
    SET(GLUT_LIBRARIES ${SAMSARA_SOURCE_DIR}/thirdParty/win32/lib/glut32.lib)
    
    SET(GLEW_INCLUDE_DIR ${SAMSARA_SOURCE_DIR}/thirdParty/win32/include)
    SET(GLEW_LIBRARIES ${SAMSARA_SOURCE_DIR}/thirdParty/win32/lib/glew32.lib)
    
    SET(GLFW_LIBRARIES glfw.lib)

    SET ( LIBXML2_INCLUDE_DIR
        "${SAMSARA_SOURCE_DIR}/thirdParty/Universal/libxml2/include"
        CACHE PATH
        "Path to the libxml2 include directory."
	)

    SET ( LIBXML2_LIBRARIES
	  libxml2
    CACHE PATH
        "Path to the libxml2 library"
	)

ELSE(WIN32)
  IF(UNIX)
    INCLUDE(FindGLUT)
 
    SET(GLEW_INCLUDE_DIR ${SAMSARA_SOURCE_DIR}/thirdParty/Universal/glew/include)
    SET(GLEW_LIBRARIES glew)     
    
  ENDIF(UNIX)
ENDIF(WIN32)


IF(WIN32)
    SET(LIBRARY_OUTPUT_PATH ${EMBR_SOURCE_DIR}/build/win32Build/lib)
    SET(EXECUTABLE_OUTPUT_PATH ${EMBR_SOURCE_DIR}/build/win32Build/bin)
    SET(ARCHIVE_OUTPUT_DIRECTORY ${EMBR_SOURCE_DIR}/build/win32Build/build)
ELSE(WIN32)
  IF(APPLE)
    SET(LIBRARY_OUTPUT_PATH ${EMBR_SOURCE_DIR}/build/macBuild/lib)
    SET(EXECUTABLE_OUTPUT_PATH ${EMBR_SOURCE_DIR}/build/macBuild/bin)
    SET(ARCHIVE_OUTPUT_DIRECTORY ${EMBR_SOURCE_DIR}/build/macBuild/build)
  ELSE(APPLE)
    SET(LIBRARY_OUTPUT_PATH ${EMBR_SOURCE_DIR}/build/linuxBuild/lib)
    SET(EXECUTABLE_OUTPUT_PATH ${EMBR_SOURCE_DIR}/build/linuxBuild/bin)
    SET(ARCHIVE_OUTPUT_DIRECTORY ${EMBR_SOURCE_DIR}/build/linuxBuild/build)
  ENDIF(APPLE)
ENDIF(WIN32)

   

    FIND_PACKAGE(Boost 1.60.0)
    SET(Boost_USE_STATIC_LIBS ON)
    SET(Boost_USE_MULTITHREAD ON) 
    
   FIND_PACKAGE(PythonLibs 2.5 REQUIRED)
    INCLUDE(FindPythonLibs 25)
    SET(Boost_LIBRARY_DIRS 
    CACHE PATH
    "path to python libs")

IF(WIN32)
  IF(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    SET(Boost_PYTHON_LIBRARY ${Boost_LIBRARY_DIRS}/boost_python-vc90-mt-gd-1_59_0.lib)
    SET(Boost_THREAD_LIBRARY ${Boost_LIBRARY_DIRS}/libboost_thread-vc90-mt-gd-1_59_0.lib)
    SET(Boost_DATE_TIME_LIBRARY ${Boost_LIBRARY_DIRS}/libboost_date_time-vc90-mt-gd-1_59_0.lib)
    SET(Boost_SYSTEM_LIBRARY ${Boost_LIBRARY_DIRS}/libboost_system-vc90-mt-gd-1_59_0.lib)
    #SET(Boost_ASIO_LIBRARY ${Boost_LIBRARY_DIRS}/libboost_asio-vc90-mt-gd-1_59_0.lib)



  ELSE(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    SET(Boost_PYTHON_LIBRARY ${Boost_LIBRARY_DIRS}/boost_python-vc90-mt-1_59_0.lib)
    SET(Boost_THREAD_LIBRARY ${Boost_LIBRARY_DIRS}/libboost_thread-vc90-mt-1_59_0.lib)
    SET(Boost_DATE_TIME_LIBRARY ${Boost_LIBRARY_DIRS}/libboost_date_time-vc90-mt-1_59_0.lib)
    SET(Boost_SYSTEM_LIBRARY ${Boost_LIBRARY_DIRS}/libboost_system-vc90-mt-1_59_0.lib)
    #SET(Boost_ASIO_LIBRARY ${Boost_LIBRARY_DIRS}/libboost_asio-vc90-mt-1_59_0.lib)

  ENDIF(${CMAKE_BUILD_TYPE} MATCHES "Debug")
ELSE(WIN32)
 IF(APPLE)
    SET(Boost_PYTHON_LIBRARY ${Boost_LIBRARY_DIRS}/libboost_python.a)
    SET(Boost_THREAD_LIBRARY ${Boost_LIBRARY_DIRS}/libboost_thread.a)
    SET(Boost_DATE_TIME_LIBRARY ${Boost_LIBRARY_DIRS}/libboost_date_time.a)
    SET(Boost_SYSTEM_LIBRARY ${Boost_LIBRARY_DIRS}/libboost_system.a)
    #SET(Boost_ASIO_LIBRARY ${Boost_LIBRARY_DIRS}/libboost_asio.a)


 ELSE(APPLE)
  IF(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    SET(Boost_PYTHON_LIBRARY ${Boost_LIBRARY_DIRS}/boost_python-gcc42-mt-gd-1_59_0.so)
    SET(Boost_THREAD_LIBRARY ${Boost_LIBRARY_DIRS}/libboost_thread-gcc42-mt-gd-1_59_0.so)
    SET(Boost_DATE_TIME_LIBRARY ${Boost_LIBRARY_DIRS}/libboost_date_time-gcc42-mt-gd-1_59_0.so)
    SET(Boost_SYSTEM_LIBRARY ${Boost_LIBRARY_DIRS}/libboost_system-gcc42-mt-gd-1_59_0.so)
    #SET(Boost_ASIO_LIBRARY ${Boost_LIBRARY_DIRS}/libboost_asio-gcc42-mt-gd-1_59_0.so)

  ELSE(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    SET(Boost_PYTHON_LIBRARY ${Boost_LIBRARY_DIRS}/libboost_python-gcc42-mt-1_59_0.so)
    SET(Boost_THREAD_LIBRARY ${Boost_LIBRARY_DIRS}/libboost_thread-gcc42-mt-1_59_0.so)
    SET(Boost_DATE_TIME_LIBRARY ${Boost_LIBRARY_DIRS}/libboost_date_time-gcc42-mt-1_59_0.so)
    SET(Boost_SYSTEM_LIBRARY ${Boost_LIBRARY_DIRS}/libboost_system-gcc42-mt-1_59_0.so)
    #SET(Boost_ASIO_LIBRARY ${Boost_LIBRARY_DIRS}/libboost_asio-gcc42-mt-1_59_0.so)
  ENDIF(${CMAKE_BUILD_TYPE} MATCHES "Debug")
 ENDIF(APPLE)
ENDIF(WIN32)  

# thirdparty

IF(WIN32)
  SET(THIRDPARTY_INCLUDE_DIR ${EMBR_SOURCE_DIR}/thirdParty/win32/include)
  SET(THIRDPARTY_LIB_DIR ${EMBR_SOURCE_DIR}/thirdParty/win32/lib)
ENDIF(WIN32)

SUBDIRS(SRC ${SAMSARA_HOME_DIR})
