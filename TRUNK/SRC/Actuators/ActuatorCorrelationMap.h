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


/** \addtogroup Actuators
 *  Additional documentation for group `actuator'
 *  @{
 */

#ifndef _ACTUATORCORRELATIONMAP_H
#define _ACTUATORCORRELATIONMAP_H

#include "EMBRKinematicActuator.h"

/**
 * \class ActuatorIKSlerp
 * \brief implements key-frame based motion generation method. Key frames ars computed using inverse kinematics, in-between frames are linearly interpolated.
 * implements key-frame based motion generation method. Key frames are computed using inverse kinematics, in-between frames are linearly interpolated.
 */
class ActuatorCorrelationMap : public SMRActuator, public EMBRKinematicActuator
{
public:
/**
 * \brief default constructor
 */
  ActuatorCorrelationMap(Character* _relatedCharacter, string _sourceJointName, float _sourceMinValue, float _sourceMaxValue,  string _targetJointName, float _targetMinValue, float _targetMaxValue, SMRSkeleton *_refSkeleton);

  ~ActuatorCorrelationMap();
/**
 * \brief compute intermediate pose by interpolating between start pose and final pose (target pose)
 * \param relativeTime interpolation parameter between 0 and 1.
 */
  void process(float relativeTime);
/**
 * \brief get the resulting pose
 */
  SMRSkeleton *getSkeleton(float attenuationFactor=1.0f);
  SMRJoint* m_sourceJoint;                      /** this joint will drive the correlation map */
  SMRKinematicChain m_kinematicJoint;           /** a kinematic chain which only contains one joint */
  SMRSkeleton m_referenceChain;

  float m_sourceMinValue;
  float m_sourceMaxValue;
  float m_targetMinValue;
  float m_targetMaxValue;


};

#endif
/** @} */

