
#include "MotionSegment.h"


MotionSegment::MotionSegment(Character* _relatedCharacter, float _relative_fade_in, float _relative_fade_out): m_absoluteStartTime(200), m_absoluteStopTime(0), m_relative_fade_in(_relative_fade_in), m_relative_fade_out(_relative_fade_out), m_gestureModifier(NULL), m_relatedCharacter(_relatedCharacter)
{
  m_holdDuration = 0;
  m_temporalWarpSpline = SMRHermitePoly<double>(-1.0,0.0,1.0,2.0);
}

MotionSegment::~MotionSegment()
{
  for_each( m_actuators.begin(), m_actuators.end(), DeleteObjectPtr());
}

void MotionSegment::process(const unsigned int _absoluteTimeMs, bool _timeForFeedback)
{
  float warpedRelativeTime = 0.0f;
  if (_absoluteTimeMs > m_absoluteStopTime-m_holdDuration)
  {
    LOG_TRACE(logger," m_absoluteStopTime= " << m_absoluteStopTime << " m_holdDuration= " << m_holdDuration << " absoluteTimeMs= " << _absoluteTimeMs);
    warpedRelativeTime = 1.0f;
  }
  else
  {
    double relativeTime = static_cast<double>(_absoluteTimeMs - m_absoluteStartTime) /  static_cast<double>((m_absoluteStopTime-m_holdDuration) - m_absoluteStartTime);
    if (relativeTime > 1.0f)
      relativeTime = 1.0f;
    warpedRelativeTime = relativeTime; //static_cast<float>(m_temporalWarpSpline.evaluate(relativeTime));
    LOG_TRACE(logger," processing motion segment with warped relative time: " << warpedRelativeTime << " relativeTime is " << relativeTime <<" m_absoluteStopTim= " << m_absoluteStopTime<< " absoluteTimeMs= " << _absoluteTimeMs);
  }
  std::vector<ActuatorGenericEMBR*>::iterator actuatorIterator;
  for (actuatorIterator = m_actuators.begin(); actuatorIterator < m_actuators.end(); actuatorIterator ++)
  {
    (*actuatorIterator)->m_timeForFeedback = _timeForFeedback;
    (*actuatorIterator)->m_absoluteTimeMs = _absoluteTimeMs;
    (*actuatorIterator)->process(warpedRelativeTime);
    //TODO register absolute time here to send it in the feedback messagge
    if (this->m_type == MotionSegment::ABSOLUTE_KINEMATIC_POSE || this->m_type == MotionSegment::RELATIVE_KINEMATIC_POSE)
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
  //UDPFeedbackSender::getInstance()->sendFeedbackMessage("troulala\n");
  //std::vector<ActuatorGenericEMBR*>::iterator realValuedActuatorIterator;
  //for (realValuedActuatorIterator = m_actuators.begin(); realValuedActuatorIterator < m_actuators.end(); realValuedActuatorIterator ++)
  //{
  //  (*realValuedActuatorIterator)->process(warpedRelativeTime);
  //  RealValuedResultStruct resultStruct = (*realValuedActuatorIterator)->getRealValuedStruct();
  //  m_relatedCharacter->appplyRealvaluedResult(resultStruct); 
  //}

  LOG_TRACE(logger,"processed" );
}


SMRSkeleton* MotionSegment::getSkeletonUnsecured()
{
    return &m_skeleton;
}


SMRSkeleton* MotionSegment::getSkeleton()
{
  if (m_skeleton.getNumJoints() > 0)
    return &m_skeleton;
  else
    return NULL;
}

void MotionSegment::setTempWarpSpline(const double p0, const double m0, const double p1, const double m1, const double _tension)
{
  m_temporalWarpSpline = SMRHermitePoly<double>(p0,m0,p1,m1);
  m_temporalWarpSpline.setTension(_tension);
}

void MotionSegment::setAbsoluteStartTime(const int _startTime)
{
  m_absoluteStartTime = _startTime;
}

void MotionSegment::setAbsoluteStopTime(const int _stopTime)
{
  m_absoluteStopTime = _stopTime;
}

void MotionSegment::setHoldDuration(const int _holdDuration)
{
  m_holdDuration = _holdDuration;
}

unsigned int MotionSegment::getAbsoluteStartTime(void) const 
{
  return m_absoluteStartTime;
}

unsigned int MotionSegment::getHoldDuration(void) const 
{
  return m_holdDuration;
}

unsigned int MotionSegment::getAbsoluteStopTime(void) const 
{
  return m_absoluteStopTime;
}

std::vector<ActuatorGenericEMBR*> MotionSegment::getActuators() const 
{
  return m_actuators;
}

void MotionSegment::addActuator(ActuatorGenericEMBR *_actuator)
{
  m_actuators.push_back(_actuator);
}

//void MotionSegment::addRealValuedActuator(EMBRRealValuedActuator *_actuator)
//{
//  m_actuators.push_back(_actuator);
//}


ActuatorGenericEMBR* MotionSegment::getActuator(int _actuatorIndex) const 
{
  std::vector<ActuatorGenericEMBR*>::const_iterator actuatorIterator;
  actuatorIterator = m_actuators.begin();
  actuatorIterator += _actuatorIndex;
  if (actuatorIterator == m_actuators.end())
  {
    LOG_ERROR(logger,"the actuator at index" << _actuatorIndex << "does not exists");
  }
  return *actuatorIterator;
}

float MotionSegment::getAmount(unsigned int _absoluteTimeMs)
{
  if(m_absoluteStopTime > 0)
  {
    float relativeTime = ((float)(_absoluteTimeMs - m_absoluteStartTime)) / ((float)((m_absoluteStopTime) - m_absoluteStartTime));
    if (relativeTime > 0.0f && relativeTime < 1.0f)
    {
      if (relativeTime < m_relative_fade_in)
      {
        return relativeTime / m_relative_fade_in;
      }else if (relativeTime > m_relative_fade_out && m_relative_fade_out < 1.0f)
      {
        return 1-((relativeTime - m_relative_fade_out) / (1 - m_relative_fade_out));
      }else
      {
        return 1.0f;
      }
    }else
    {
      return 1.0f;
    }
  }else
  {
    return 1.0f;
  }
}

SMRHermitePoly<double> MotionSegment::getTemporalWarpSpline()
{
  return m_temporalWarpSpline;
}

void MotionSegment::setGestureModifier(SMRSmartPtr<GestureModifier> * _gestMod)
{ 
  m_gestureModifier = _gestMod; 
}

SMRSmartPtr<GestureModifier>* MotionSegment::getGestureModifier()
{
  return m_gestureModifier;
}

MotionSegment::MotionSegmentType MotionSegment::getType()
{
    return m_type;
}
