
#include "Character.h"

void Character::addMorphTarget(const string &_morphTargetName)
{
  m_morphTargets[_morphTargetName];
}

void Character::setMorphTargetWeight(const string &_morphTargetName, float _value)
{
  if ( m_morphTargets.find(_morphTargetName) == m_morphTargets.end() )
    LOG_ERROR(logger,"morph target " << _morphTargetName << " has not been registered");
  else
    m_morphTargets[_morphTargetName] = _value;
}

float Character::getMorphTargetWeight(const string &_morphTargetName)
{
  if ( m_morphTargets.find(_morphTargetName) == m_morphTargets.end() )
  {
    LOG_ERROR(logger,"morph target " << _morphTargetName << " has not been registered");
    return 0.0f;
  }else
  {
    return m_morphTargets[_morphTargetName];
  }
}



void Character::addShaderParameter(const string &_shaderParameterName)
{
  m_shaderParameters[_shaderParameterName] = 0.0f;
}
void Character::setShaderParameterWeight(const string &_shaderParameterName, float _value)
{
  if ( m_shaderParameters.find(_shaderParameterName) == m_shaderParameters.end() )
    LOG_ERROR(logger,"shader parameter " << _shaderParameterName << " has not been registered");
  else
    m_shaderParameters[_shaderParameterName] = _value;
}



float Character::getShaderParameterWeight(const string &_shaderParameterName)
{
  if ( m_shaderParameters.find(_shaderParameterName) == m_shaderParameters.end() )
  {
    LOG_ERROR(logger,"shader parameter " << _shaderParameterName << " has not been registered");
    return 0.0f;
  }else
  {
    return m_shaderParameters[_shaderParameterName];
  }
}

void Character::addAutonomousParameter(const string &_autonomousParameterName)
{
  m_autonomousParameters[_autonomousParameterName] = 0.0f;
}

void Character::setAutonomousParameterValue(const string &_autonomousParameterName, float _value)
{
  if ( m_autonomousParameters.find(_autonomousParameterName) == m_autonomousParameters.end() )
    LOG_INFO(logger,"autonomous parameter " << _autonomousParameterName << " has not been registered");
  else
    m_autonomousParameters[_autonomousParameterName] = _value;
}

float Character::getAutonomousParameterValue(const string &_autonomousParameterName)
{
  std::map<string,float>::iterator mapIterator;
  if ( m_autonomousParameters.find(_autonomousParameterName) == m_autonomousParameters.end() )
  {
    LOG_INFO(logger,"autonomous parameter " << _autonomousParameterName << " has not been registered");
    return 0.0f;
  }else
    return m_autonomousParameters[_autonomousParameterName];
}

void Character::appplyRealvaluedResult(const RealValuedResultStruct &_realValuedResultStruct)
{
  string name;
  float value;
  switch(_realValuedResultStruct.type)
  {
    case MORPHTARGET :
      name = _realValuedResultStruct.key;
      value = _realValuedResultStruct.value;
      this->setMorphTargetWeight(name, value);
      m_modifiedMorphTargets.push_back(name);
      break;
    case SHADER :
      name = _realValuedResultStruct.key;
      value = _realValuedResultStruct.value;
      this->setShaderParameterWeight(name, value);
      break;
    case AUTOBEHAVIOR :
      name  = _realValuedResultStruct.key;
      value = _realValuedResultStruct.value;
      this->setAutonomousParameterValue(name, value);
      break;
    case UNKNOWN :
      LOG_ERROR(logger,"this motion segment tries to read an unprocessed realvaluedResult strucutre");
      break;
    default :
      LOG_ERROR(logger,"this motion segment tries to read a misinstanciated strucutre");
      break;
  }
}
