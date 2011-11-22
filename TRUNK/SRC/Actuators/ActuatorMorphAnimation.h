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


#ifndef _ACTUATORMORPHANIMATION_H
#define _ACTUATORMORPHANIMATION_H

#include "EMBRRealValuedActuator.h"
#include "../Character.h"
//#include "../ActuatorFactory.h"
#include "ActuatorGenericEMBR.h"


typedef std::map<float,float> MorphPosesVector;

/**
 * \class ActuatorMorphAnimation
 * \brief implements pre-recorded animation based motion generation method for morph targets.
 */
class ActuatorMorphAnimation : public EMBRRealValuedActuator
{
public:
	ActuatorMorphAnimation(Character* _relatedCharacter, string _autonomousBehaviourKey=""):EMBRRealValuedActuator(_relatedCharacter),m_autonomousBehaviourKey(_autonomousBehaviourKey){
    m_result.type = MORPHTARGET;
  };
  ~ActuatorMorphAnimation();

  virtual void process(float _relativeTime);

  void addMorphKey(string _morphName, float _relativeTime, float _value);

  SMRSkeleton* getSkeleton( float attenuationFactor = 1.0f ){return NULL;}

protected:
  std::map<string,MorphPosesVector*> m_morphAnimations;
  string m_autonomousBehaviourKey;

};
#endif

