cmake_minimum_required(VERSION 2.6)

INCLUDE_DIRECTORIES( 
  ${THIRDPARTY_INCLUDE_DIR}
  ${PYTHON_INCLUDE_PATH}  
)

LINK_DIRECTORIES( ${THIRDPARTY_LIB_DIR}
                  ${LIBRARY_OUTPUT_PATH}
                )
   
ADD_EXECUTABLE(testvfs main.cpp)

IF(WIN32)
  TARGET_LINK_LIBRARIES(testvfs libpandaexpress libp3dtool ${PYTHON_LIBRARY} )
ENDIF(WIN32)