
#include "ActuatorCorrelationMap.h"

ActuatorCorrelationMap::~ActuatorCorrelationMap()
{
    m_referenceChain.flush();
}

ActuatorCorrelationMap::ActuatorCorrelationMap(Character* _relatedCharacter, string _sourceJointName, float _sourceMinValue, float _sourceMaxValue, string _targetJointName,  float _targetMinValue, float _targetMaxValue, SMRSkeleton* _refSkeleton) : EMBRKinematicActuator(_relatedCharacter), m_sourceMinValue(_sourceMinValue), m_sourceMaxValue(_sourceMaxValue), m_targetMinValue(_targetMinValue), m_targetMaxValue(_targetMaxValue)
{
  m_referenceSkeleton = _refSkeleton;

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
     //SMRJoint* parentJoint = m_referenceSkeleton->getJointByName(currentParentName);
     m_referenceChain.insertJoint(0,currentJoint);
  }

  m_type = ActuatorGenericEMBR::KINEMATIC;
  LOG_DEBUG(logger,"constructor of ActuatorCorrelationMap");
  //m_kinematicJoint = SMRKinematicChain();
    //for_each( m_joints.begin(), m_joints.end(), DeleteObjectPtr());
  m_kinematicJoint.setName(m_referenceSkeleton->getName());
  m_kinematicJoint.setMode(m_referenceSkeleton->getMode());
  m_kinematicJoint.setRotationOrder(m_referenceSkeleton->getRotationOrder());
  //m_kinematicJoint.setStartJointIndex(0);
  
  SMRKinematicJoint* newKinematicJoint = new SMRKinematicJoint(*m_referenceSkeleton->getJointByName(_targetJointName));
  SMRDOF *currentDof = newKinematicJoint->getDOF(3);
  currentDof->setUpperBound(currentDof->getRotationAngle() + m_targetMaxValue);
  currentDof->setLowerBound(currentDof->getRotationAngle() + m_targetMinValue);

  m_targetMaxValue = currentDof->getRotationAngle() + m_targetMaxValue;
  m_targetMinValue = currentDof->getRotationAngle() + m_targetMinValue;

  m_kinematicJoint.insertJoint(newKinematicJoint);
}


void ActuatorCorrelationMap::process(float relativeTime)
{
  //TODO:retrieve the related joint, get its value and link it to the target joint's value
  
  m_referenceChain.setMode(ABSOLUTEMODE);
  SMRVector3 sourceJointAbsolutePosition = m_sourceJoint->getPosition();
  m_referenceChain.setMode(RELATIVEMODE);
  
  float beta = (sourceJointAbsolutePosition.Z()-m_sourceMinValue) / (m_sourceMaxValue - m_sourceMinValue);
  float alpha = m_targetMaxValue-m_targetMinValue;
  float result = m_targetMinValue + alpha*beta;
  m_kinematicJoint.getJoint(0)->getDOF(3)->setRotationAngle(result);
  m_kinematicJoint.getJoint(0)->updateRotation();

}

SMRSkeleton *ActuatorCorrelationMap::getSkeleton(float attenuationFactor)
{
  return (SMRSkeleton*)(&m_kinematicJoint);
}
