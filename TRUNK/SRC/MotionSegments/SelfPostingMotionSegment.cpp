#include "SelfPostingMotionSegment.h"

void SelfPostingMotionSegment::process(const unsigned int _absoluteTimeMs, bool _timeForFeedback){

  float frequency = m_relatedCharacter->getAutonomousParameterValue(m_incrementParameterKey); // TODO, renseigned m_relatedCharacter dans le constructeur du motionSegment
 /* timeval timeVal;
  gettimeofday(&timeVal, NULL);
  int currentTick  = (unsigned int) ( (timeVal.tv_sec * 1000.0) + (timeVal.tv_usec / 1000.0));
  */
  int currentTick = SMRUtils::getCurrentTime();
  m_increment = (float)(currentTick-m_lastTick) * 0.001f; // number of seconds since last tick
  m_lastTick = currentTick;                               // currentTick becomes current time

  m_counter += m_increment;                               // increment m_counter
  
  float cursor = m_counter * frequency;                   // wtf ?
    
  if (cursor > 1.0f)                                      // cursor cycles between 0 and 1...
  {
    m_counter = 0.0f;
    cursor = 0.0f;
  }

  std::vector<ActuatorGenericEMBR*>::iterator actuatorIterator;
  for (actuatorIterator = m_actuators.begin(); actuatorIterator < m_actuators.end(); actuatorIterator ++)
  {
    (*actuatorIterator)->process(cursor);
    if ((*actuatorIterator)->m_type == ActuatorGenericEMBR::KINEMATIC || (*actuatorIterator)->m_type == ActuatorGenericEMBR::ANIMATION)
    {
      EMBRKinematicActuator* skeletalActuator = static_cast<EMBRKinematicActuator*>(*actuatorIterator);
      m_skeleton += *(skeletalActuator->getSkeleton());
    }
    else
    {
      EMBRRealValuedActuator* realValuedActuator = static_cast<EMBRRealValuedActuator*>(*actuatorIterator);
      RealValuedResultStruct resultStruct = realValuedActuator->getRealValuedStruct();
      m_relatedCharacter->appplyRealvaluedResult(resultStruct);
    }
  }

  m_absoluteStartTime = _absoluteTimeMs + m_nextPostTimeOffsetInMs;
  m_absoluteStopTime = 0;
}

