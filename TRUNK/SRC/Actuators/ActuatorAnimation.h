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

#ifndef _ACTUATORANIMATION_H
#define _ACTUATORANIMATION_H

#include "SmrMotionPlayer.h"

#include "EMBRKinematicActuator.h"
#include "../Character.h"
//#include "../ActuatorFactory.h"

/**
 * \class ActuatorAnimation
 * \brief implements pre-recorded animation based motion generation method.
 */
class ActuatorAnimation : public SMRMotionPlayer, public EMBRKinematicActuator
{
public:
/**
 * \brief default constructor
 */
  ActuatorAnimation(Character* _relatedCharacter, SMRMotion* motion,const SMRSkeleton &_stencil,const SMRSkeleton *_referencePose, bool _isDerivative=false, string _weightKey="");
/**
 * \brief default destructor
 */
  ~ActuatorAnimation();
/**
 * \brief compute intermediate pose by interpolating between start pose and final pose (target pose)
 * \param relativeTime interpolation parameter between 0 and 1.
 */
  void process(float _relativeTime);
/**
 * \brief get the resulting pose
 */
  SMRSkeleton * getSkeleton(float attenuationFactor=1.0f);
/**
 * \brief set up a motion
 * \param _chain 
 */
  void setMotion(string motionKey);

/**
 * \brief set ease_in value (between 0.0f and 1.0f)
 * \param _ease_in relative ease_in value
 */
  void setEaseIn(float _ease_in);

  void setAlpha(float alpha);

protected:
  float m_fade_in;
  float m_fade_out;
  float m_alpha;
  SMRSkeleton m_returnPose;
  SMRSkeleton m_startPose;
  SMRSkeleton m_neutralPose;
  SMRSkeleton m_cumuledDrift; 
  bool m_firstTimeProcess;
  string m_weightKey;
  float m_lastRelativeTime;
  bool m_isDerivative;
};

#endif
/** @} */

