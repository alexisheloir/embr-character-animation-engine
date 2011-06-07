#include "AnimationMotionSegment.h"

void AnimationMotionSegment::process(const unsigned int _absoluteTimeMs, bool _timeForFeedback){

  float relativeTime = (float)(_absoluteTimeMs-this->getAbsoluteStartTime()) / (float)((this->getAbsoluteStopTime()-this->getAbsoluteStartTime()));
  if (relativeTime > 1.0f)
    relativeTime = 1.0f;
  //float warpedRelativeTime = static_cast<float>(m_temporalWarpSpline.evaluate(relativeTime)); 
  float warpedRelativeTime = relativeTime;// static_cast<float> ( m_currentTimeWarpSpline.evaluate(relativeTime));

  std::vector<ActuatorGenericEMBR*>::iterator actuatorIterator;
  for (actuatorIterator = m_actuators.begin(); actuatorIterator < m_actuators.end(); actuatorIterator ++)
  {
    (*actuatorIterator)->process(warpedRelativeTime);
    if ((*actuatorIterator)->m_type == ActuatorGenericEMBR::KINEMATIC || (*actuatorIterator)->m_type == ActuatorGenericEMBR::ANIMATION)
    {
      EMBRKinematicActuator* skeletalActuator = static_cast<EMBRKinematicActuator*>(*actuatorIterator);
      m_skeleton += *(skeletalActuator->getSkeleton()); //TODO: correct the cast problem for the correlationMapActuator and also solve the DOF limits issue 
    }
    else
    {
      EMBRRealValuedActuator* realValuedActuator = static_cast<EMBRRealValuedActuator*>(*actuatorIterator);
      RealValuedResultStruct resultStruct = realValuedActuator->getRealValuedStruct();
      m_relatedCharacter->appplyRealvaluedResult(resultStruct);
    }
  }
}

