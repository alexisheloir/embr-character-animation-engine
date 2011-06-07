
#include "CorrelationMap.h"


CorrelationMap::CorrelationMap(Character *_relatedCharacter, float _relative_fade_in, float _relative_fade_out):ComplexMotionSegment(_relatedCharacter,_relative_fade_in,_relative_fade_out),m_inputType(CorrelationMap::AUTONOMOUSPARAMETER)
{
}

void CorrelationMap::setAutonomousParameterKey(string _autonomousParameterKey)
{
  m_autonomousParameterKey = _autonomousParameterKey;
}

void CorrelationMap::setReferenceSkeleton(SMRSkeleton *_referenceSkeleton)
{
  m_referenceSkeleton = _referenceSkeleton;
}

void CorrelationMap::setSourceJoint(string _sourceJointName)
{
  m_referenceChain.setName(m_referenceSkeleton->getName());
  m_referenceChain.setMode(m_referenceSkeleton->getMode());
  m_referenceChain.setRotationOrder(m_referenceSkeleton->getRotationOrder());

  m_sourceJoint = m_referenceSkeleton->getJointByName(_sourceJointName);
  m_referenceChain.insertJoint(0,m_sourceJoint);
  SMRJoint* currentJoint = m_sourceJoint; 

  while(currentJoint->getParentName() != "")
  {
     string currentParentName = currentJoint->getParentName();
     currentJoint = m_referenceSkeleton->getJointByName(currentParentName);
     SMRJoint* parentJoint = m_referenceSkeleton->getJointByName(currentParentName);
     m_referenceChain.insertJoint(0,currentJoint);
  }
}

float CorrelationMap::getValue()
{
  return m_value;
}

void CorrelationMap::process(const unsigned int _absoluteTimeMs, bool _timeForFeedback)
{

  // a correlation map is not driven by time but by an input parameter which can be another joint, another correlation map or an autonomousBehaviourPArameter

  if(m_inputType == CorrelationMap::AUTONOMOUSPARAMETER)
  {
    m_value = m_relatedCharacter->getAutonomousParameterValue(m_autonomousParameterKey);
  }else if (m_inputType == CorrelationMap::JOINTPOSITION)
    //compute the cartesian position of the joint, map it and use it to drive the actuators
  {
  }else if (m_inputType == CorrelationMap::CORRELATIONMAP)
  {
    m_value = m_sourceCorrelationMap->getValue();
  }else
  {
    //logger.error(error: trying ot process a correlationMap of unrecognized type)
  }

  std::vector<ActuatorGenericEMBR*>::iterator actuatorIterator;
  for (actuatorIterator = m_actuators.begin(); actuatorIterator < m_actuators.end(); actuatorIterator ++)
  {
    (*actuatorIterator)->process(m_value);
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

  m_absoluteStartTime = _absoluteTimeMs + 40;
  m_absoluteStopTime = 0;
}