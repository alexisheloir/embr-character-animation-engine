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


#ifndef _AUTONOMOUSBEHAVIORPARAMETER_H
#define _AUTONOMOUSBEHAVIORPARAMETER_H

#include "EMBRRealValuedActuator.h"
#include "../Character.h"
//#include "../ActuatorFactory.h"

/**
 * \class ActuatorShaderParameter
 * \brief some effects like blushing are conveyed using shader that accept input parameters
 */

class ActuatorAutonomousBehaviorParameter : public EMBRRealValuedActuator
{
public:
/**
 * \brief default constructor
 */
  ActuatorAutonomousBehaviorParameter(Character* _relatedCharacter, string _behaviorParameterName, float _behaviorParameterFinalValue);
  
  SMRSkeleton* getSkeleton(float attenuationFactor=1.0f){return NULL;}

/**
 * \brief default destructor
 */
  ~ActuatorAutonomousBehaviorParameter();

/**
 * \brief compute intermediate value by interpolating between initial weight value and final weight value
 * \param relativeTime interpolation parameter between 0 and 1.
 */
  void process(float relativeTime);


protected:
string m_behaviorParameterName;
bool m_alreadyProcessed;
float m_behaviorParameterFinalValue;
float m_behaviorParameterInitialValue;

};
#endif


