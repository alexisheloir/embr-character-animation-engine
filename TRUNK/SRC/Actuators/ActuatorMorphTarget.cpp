#include "ActuatorMorphTarget.h"

ActuatorMorphTarget::ActuatorMorphTarget(Character* _relatedCharacter, string _morphTargetName, float _morphTargetFinalValue, string _weightKey):EMBRRealValuedActuator(_relatedCharacter), m_morphTargetName(_morphTargetName), m_alreadyProcessed(false), m_morphTargetFinalValue(_morphTargetFinalValue), m_morphTargetInitialValue(0.0f), m_weightKey(_weightKey)
{
  m_morphTargetInitialValue = m_relatedCharacter->getMorphTargetWeight(_morphTargetName);
  m_result.key = m_morphTargetName;
  m_result.type = MORPHTARGET;
}

ActuatorMorphTarget::~ActuatorMorphTarget()
{
}

void ActuatorMorphTarget::process(float relativeTime)
{
  if (! m_alreadyProcessed)
  {
    m_morphTargetInitialValue = m_relatedCharacter->getMorphTargetWeight(m_morphTargetName);
    m_alreadyProcessed = true;
  }
  // CurrentWeight interpolates between m_morphTargetInitialValue and m_morphTargetFinalValue
  float currentWeight = m_morphTargetInitialValue + relativeTime *(m_morphTargetFinalValue-m_morphTargetInitialValue);
  // Alpha is an attenuation parameter usually defined by an autonomousParameter
  if (currentWeight < 0.0f) currentWeight = 0.0f;
  if (currentWeight > 1.51f) currentWeight = 1.51f;

  float alpha = 1.0;
  if (m_weightKey != "")
  {
    alpha = m_relatedCharacter->getAutonomousParameterValue(m_weightKey);
    //if ( alpha == -1.0f)
    //  alpha = 1.0f;
  }
  m_result.value = currentWeight*alpha;
  //m_relatedCharacter->setMorphTargetWeight(m_morphTargetName, currentWeight*alpha);

}



