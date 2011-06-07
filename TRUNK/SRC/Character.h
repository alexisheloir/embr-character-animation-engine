//EMBR is a real time character animation engine.
//Copyright (C) 2010 Alexis Heloir
//
//This program is free software: you can redistribute it and/or
//modify it under the terms of the GNU Lesser General Public
//License as published by the Free Software Foundation, version 3
//of the License.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY, without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//Lesser General Public License for more details.
//
//You should have received a copy of the GNU Lesser General Public
//License along with this program.  If not, see
//<http://www.gnu.org/licenses/>.


#ifndef _CHARACTER_H
#define _CHARACTER_H

#include "Smr.h"

typedef enum RealValuedActuatorTypeEnum{SHADER,MORPHTARGET,AUTOBEHAVIOR,UNKNOWN} RealValuedActuatorType;

typedef struct RealValuedResult{
  RealValuedResult():key(""),value(0.0f),type(UNKNOWN){};
  string key;
  float value;
  RealValuedActuatorType type;
} RealValuedResultStruct;

/**
 *  \class Character
 *  \brief this class represents and control a character in the EMBR framework
 */
class Character{

public:
  Character(SMRSkeleton* const _referencePose):m_TPose(*(_referencePose)),m_animationPose(_referencePose)
  {
    m_jointGroups["all"] = m_TPose;
  }
  ~Character(){;}

public:

 /**
  * \brief Add a new morph Target
  * \param _morphTargetName name of the morph target
  */
  void addMorphTarget(const string &_morphTargetName);

 /**
  * \brief Set the weight of a morphTarget
  * \param _morphTargetName name of the morph target
  * \param _value value of the morph target
  */
  void setMorphTargetWeight(const string &_morphTargetName, float _value);
 /**
  * \brief get the weight of a morph target
  * \return the weight of the morph target
  */
  float getMorphTargetWeight(const string &_morphTargetName);

 /**
  * \brief Add a new shader parameters
  * \param _shaderParameterName namr of the shader parameter to be added
  */
  void addShaderParameter(const string &_shaderParameterName);

 /**
  * \brief Set the weight of a shader parameter
  * \param _shaderParameterName of the sader whose value is to be modified
  * \param _value value of the shader parameter
  */
  void setShaderParameterWeight(const string &_shaderParameterName, float _value);

 /**
  * \brief get a shader parameter's weight
  * \return the weight of the shader parameter referenced by _shaderParameterName
  */
  float getShaderParameterWeight(const string &_shaderParameterName);

 /**
  * \brief add a new autonomous parameter (like breathing or blinking)
  * \param _autonomousParameterName name of the new autonomous parameter
  */
  void addAutonomousParameter(const string &_autonomousParameterName);

 /**
  * \brief set the value for the auonomous parameter rferenced by _autonomousParameterName
  * \param _autonomousParameterName name of the new autonomous parameter whose value has to be modified
  * \param _value the new value of the _autonomousParameterName
  */
  void setAutonomousParameterValue(const string &_autonomousParameterName, float _value);

 /**
  *  get the value of the autonomous parameter referenced by _autonomousParameterName
  *  \param _autonomousParameterName the value of the autonomous parameter
  *  \return its current value
  */
  float getAutonomousParameterValue(const string &_autonomousParameterName);

  void appplyRealvaluedResult(const RealValuedResultStruct &_realValuedResultStruct);

public:
  std::map<string, SMRMotion> m_motions;
  std::map<string, SMRKinematicChain> m_IKChains;

  const SMRSkeleton m_TPose; /** reference T-pose used to initiante IK and other procedual motion generation methods */
  SMRSkeleton* const m_animationPose; /** this pointer references a skeleton which has been instanciated in the rendering engine and which is displayed by the rendering engine */  

  std::map<string, float> m_morphTargets; /** a dictionnary of morph targets (name and current value) */
  std::map<string, float> m_shaderParameters; /** a dictionnary of shader parameters (name and current value) */
  std::map<string, float> m_autonomousParameters; /** a dictionnary of autonomous parameters (name and current value) */

  std::map<string, SMRSkeleton> m_jointGroups; /** the joint groups defined for this actor (stored as skeletons) */
  std::map<string, SMRMotion> m_animations; /** the animations defined for this actor */
};

#endif
