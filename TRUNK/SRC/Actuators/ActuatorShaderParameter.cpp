#include "ActuatorShaderParameter.h"

ActuatorShaderParameter::ActuatorShaderParameter(Character* _relatedCharacter, string _shaderParameterName, float _shaderParameterFinalValue):EMBRRealValuedActuator(_relatedCharacter), m_shaderParameterName(_shaderParameterName), m_alreadyProcessed(false), m_shaderParameterFinalValue(_shaderParameterFinalValue), m_shaderParameterInitialValue(0.0f)
{
  m_shaderParameterInitialValue = m_relatedCharacter->getShaderParameterWeight(_shaderParameterName);
  m_result.key = m_shaderParameterName;
  m_result.type = SHADER;
}

ActuatorShaderParameter::~ActuatorShaderParameter()
{
}

void ActuatorShaderParameter::process(float relativeTime)
{
  if (! m_alreadyProcessed)
  {
    m_shaderParameterInitialValue = m_relatedCharacter->getShaderParameterWeight(m_shaderParameterName);
    m_alreadyProcessed = true;
  }
  float currentWeight = m_shaderParameterInitialValue + relativeTime *(m_shaderParameterFinalValue-m_shaderParameterInitialValue);
  //m_relatedCharacter->setShaderParameterWeight(m_shaderParameterName, currentWeight);
  m_result.value = currentWeight;
}




