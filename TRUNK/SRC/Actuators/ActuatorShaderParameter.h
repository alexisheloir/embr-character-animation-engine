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


#ifndef _ACTUATORSHADERPARAMETER_H
#define _ACTUATORSHADERPARAMETER_H

#include "EMBRRealValuedActuator.h"
#include "../Character.h"
//#include "../ActuatorFactory.h"
#include "ActuatorGenericEMBR.h"

/**
 * \class ActuatorShaderParameter
 * \brief some effects like blushing are conveyed using shader that accept input parameters
 */

class ActuatorShaderParameter : public EMBRRealValuedActuator
{
public:
/**
 * \brief default constructor
 */
  ActuatorShaderParameter(Character* _relatedCharacter, string _shaderParameterName, float _shaderParameterFinalValue);
  
  SMRSkeleton* getSkeleton(float attenuationFactor=1.0f){return NULL;}

/**
 * \brief default destructor
 */
  ~ActuatorShaderParameter();

/**
 * \brief compute intermediate value by interpolating between initial weight value and final weight value
 * \param relativeTime interpolation parameter between 0 and 1.
 */
  void process(float relativeTime);


protected:
string m_shaderParameterName;
bool m_alreadyProcessed;
float m_shaderParameterFinalValue;
float m_shaderParameterInitialValue;

};
#endif



