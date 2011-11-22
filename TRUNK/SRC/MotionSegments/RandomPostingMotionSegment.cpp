#include "RandomPostingMotionSegment.h"

void RandomPostingMotionSegment::process(const unsigned int _absoluteTimeMs, bool _timeForFeedback){
  /*timeval timeVal;
  gettimeofday(&timeVal, NULL);

  int currentTick  =  (unsigned int) ( (timeVal.tv_sec * 1000.0) + (timeVal.tv_usec / 1000.0));
  */
  int currentTick = SMRUtils::getCurrentTime();
  if (!m_blinked)
  {
   m_increment = (float)(currentTick-m_lastTick) * 0.001f; // number of seconds since last tick
   m_counter += m_increment;                               // increment m_counter
  }
  m_lastTick = currentTick;                                // currentTick becomes current time


  float cursor = m_counter * 5.0f;                            // cursor moves betwen 0 and 1 every 200 ms

  if (cursor > 1.0f)                                       // cursor cycles between 0 and 1...
  {
    m_counter = 0.0f;
    cursor = 0.0f;
    m_blinked = true;
  }else
  {
    m_blinked = false;
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
      RealValuedResultStruct resultStruct = (realValuedActuator)->getRealValuedStruct();
      m_relatedCharacter->appplyRealvaluedResult(resultStruct);
    }
  }


  float blinkProbability = (m_relatedCharacter->getAutonomousParameterValue(m_incrementParameterKey));
  if (!m_blinked)
  {
    m_absoluteStartTime = _absoluteTimeMs + m_nextPostTimeOffsetInMs;
  }else
  {
    m_absoluteStartTime = _absoluteTimeMs + (unsigned int)(((float)rand()/(float)RAND_MAX)*blinkProbability);
  }
  m_absoluteStopTime = 0;
}

