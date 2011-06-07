#include "ActuatorAutonomousBehaviorParameter.h"

ActuatorAutonomousBehaviorParameter::ActuatorAutonomousBehaviorParameter(Character* _relatedCharacter, string _behaviorParameterName, float _behaviorParameterFinalValue): EMBRRealValuedActuator(_relatedCharacter), m_behaviorParameterName(_behaviorParameterName), m_alreadyProcessed(false),m_behaviorParameterFinalValue(_behaviorParameterFinalValue), m_behaviorParameterInitialValue(0.0f)
{
  m_behaviorParameterInitialValue = m_relatedCharacter->getAutonomousParameterValue(_behaviorParameterName);
  m_result.key = m_behaviorParameterFinalValue;
  m_result.type = AUTOBEHAVIOR;
}

ActuatorAutonomousBehaviorParameter::~ActuatorAutonomousBehaviorParameter()
{
}

void ActuatorAutonomousBehaviorParameter::process(float relativeTime)
{
  if (! m_alreadyProcessed)
  {
    m_behaviorParameterInitialValue = m_relatedCharacter->getAutonomousParameterValue(m_behaviorParameterName);
    m_alreadyProcessed = true;
  }
  float currentWeight = m_behaviorParameterInitialValue + relativeTime *(m_behaviorParameterFinalValue-m_behaviorParameterInitialValue);
  m_relatedCharacter->setAutonomousParameterValue(m_behaviorParameterName, currentWeight);
  m_result.value = currentWeight;

}



