project(MOTIONSEGMENTS)

REMOVE_DEFINITIONS(-D__MACOS__)
REMOVE_DEFINITIONS(-D__MWERKS__)


INCLUDE_DIRECTORIES(
      ${SAMSARA_COMMON_SOURCE_DIR}
      ${BOOST_INCLUDE_PATH}
      ${Boost_INCLUDE_DIR}
      ${LIBXML2_INCLUDE_DIR}
      ${SAMSARA_ICONV_INCLUDE_DIR}
      ${SAMSARA_CORE_SOURCE_DIR}
      ${REALIZER_SOURCE_DIR}
      ${ACTUATORS_SOURCE_DIR}
      ${SAMSARA_IO_SOURCE_DIR}
      ${SAMSARA_MATH_SOURCE_DIR}
      ${SAMSARA_NEWMAT_SOURCE_DIR}
      ${LOG4CPLUS_INCLUDE_DIR}
      ${SAMSARA_UTILS_SOURCE_DIR}
)


ADD_LIBRARY(motionsegments 
                           RandomPostingMotionSegment.cpp
		           ComplexMotionSegment.cpp
                           MotionSegment.cpp
                           SelfPostingMotionSegment.cpp
		           AnimationMotionSegment.cpp
		           CorrelationMap.cpp
		           RandomPostingMotionSegment.h
		           ComplexMotionSegment.h
                           MotionSegment.h
                           SelfPostingMotionSegment.h
		           AnimationMotionSegment.h
		           CorrelationMap.h
  )
