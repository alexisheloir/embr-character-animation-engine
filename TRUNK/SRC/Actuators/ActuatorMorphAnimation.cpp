#include "ActuatorMorphAnimation.h"

ActuatorMorphAnimation::~ActuatorMorphAnimation()
{
  map<string,MorphPosesVector*>::iterator morphAnimationsIterator;
  for (morphAnimationsIterator = m_morphAnimations.begin(); morphAnimationsIterator != m_morphAnimations.end(); morphAnimationsIterator++)
  {
    delete(morphAnimationsIterator->second);
  }
}

void ActuatorMorphAnimation::process(float _relativeTime)
{
  std::map<string,MorphPosesVector*>::iterator currentMorphTargetAnimation;

  for (currentMorphTargetAnimation = m_morphAnimations.begin(); currentMorphTargetAnimation != m_morphAnimations.end(); currentMorphTargetAnimation++)
  {
    string morphTargetName = currentMorphTargetAnimation->first;
    MorphPosesVector *currentPosesVector = currentMorphTargetAnimation->second;


    float firstInterpolant = 0.0f;
    float secondInterpolant = 1.0f;
    float firstValue = 0.0f;
    float secondValue = 1.0f;


    std::map<float,float>::iterator currentIterator;
    currentIterator = currentPosesVector->begin();


    while (currentIterator != currentPosesVector->end() && currentIterator->first <= _relativeTime)
    {
      firstInterpolant = currentIterator->first;
      firstValue = currentIterator->second;
      currentIterator++;
    }
    if (currentIterator != currentPosesVector->end())
    {
      secondInterpolant = currentIterator->first;
      secondValue = currentIterator->second;
    }

    float alpha= 1.0f;
    if(m_autonomousBehaviourKey != "")
    {
     alpha = m_relatedCharacter->getAutonomousParameterValue(m_autonomousBehaviourKey);
    }

    if (secondInterpolant != firstInterpolant)      // avoid divide by zero
    {
      float result = (_relativeTime-firstInterpolant)/(secondInterpolant-firstInterpolant);
      //m_relatedCharacter->setMorphTargetWeight(morphTargetName, (firstValue + (secondValue-firstValue)*result)*alpha);
      m_result.key = morphTargetName;
      m_result.value = firstValue + ((secondValue-firstValue)*result)*alpha;
    }
  }
}

void ActuatorMorphAnimation::addMorphKey(string _morphName, float _relativeTime, float _value)
{
  if (m_morphAnimations.find(_morphName) == m_morphAnimations.end())
  {
    MorphPosesVector* newPoseVector = new(MorphPosesVector);
    m_morphAnimations[_morphName] = newPoseVector;
  }
  MorphPosesVector* relevantPoseVector = m_morphAnimations[_morphName];
  (*relevantPoseVector)[_relativeTime]=_value;
}

