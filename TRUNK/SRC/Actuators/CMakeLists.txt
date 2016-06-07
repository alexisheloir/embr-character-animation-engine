project(ACTUATORS)

REMOVE_DEFINITIONS(-D__MACOS__)
REMOVE_DEFINITIONS(-D__MWERKS__)

INCLUDE_DIRECTORIES(
      ${REALIZER_SOURCE_DIR}
      ${MOTIONSEGMENTS_SOURCE_DIR}
      ${ACTUATORS_SOURCE_DIR}
      ${SAMSARA_COMMON_SOURCE_DIR}
      ${BOOST_INCLUDE_PATH}
      #${Boost_INCLUDE_DIR}
      ${LIBXML2_INCLUDE_DIR}
      ${SAMSARA_ICONV_INCLUDE_DIR}
      ${SAMSARA_CORE_SOURCE_DIR}
      ${SAMSARA_IO_SOURCE_DIR}
      ${SAMSARA_MATH_SOURCE_DIR}
      ${SAMSARA_NEWMAT_SOURCE_DIR}
      ${SAMSARA_UTILS_SOURCE_DIR}
      ${LOG4CPLUS_INCLUDE_DIR}
)


ADD_LIBRARY(actuators #SHARED
     ActuatorGenericEMBR.cpp
     ActuatorMorphAnimation.cpp
     ActuatorHybridIKSlerp.cpp
     ActuatorIKSlerp.cpp
     ActuatorMorphTarget.cpp
     ActuatorShaderParameter.cpp
     ActuatorAutonomousBehaviorParameter.cpp
     ActuatorAnimation.cpp
     ActuatorCorrelationMap.cpp
     EMBRRealValuedActuator.cpp
     UDPFeedbackSender.cpp

     ActuatorGenericEMBR.h
     ActuatorMorphAnimation.h
     ActuatorHybridIKSlerp.h
     ActuatorIKSlerp.h
     ActuatorMorphTarget.h
     ActuatorShaderParameter.h
     ActuatorAutonomousBehaviorParameter.h
     ActuatorAnimation.h
     ActuatorCorrelationMap.h
     EMBRRealValuedActuator.h
     EMBRKinematicActuator.h
     UDPFeedbackSender.h
)

TARGET_LINK_LIBRARIES(actuators core)