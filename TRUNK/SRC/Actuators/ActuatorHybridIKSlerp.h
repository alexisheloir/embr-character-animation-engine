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

#ifndef _ACTUATORHYBRIDIKSLERP_H
#define _ACTUATORHYBRIDIKSLERP_H

#include "SmrAnalyticalIKSolver.h"
#include "SmrGSMMSolver.h"
#include "SmrSwivel.h"
#include "SmrSmartPointer.h"
#include "../EMBRLoggers.h"
//#include "GestureModifier.h"
#include "EMBRKinematicActuator.h"

/**
 * \class ActuatorIKSlerp
 * \brief implements key-frame based motion generation method. Key frames ars computed using inverse kinematics, in-between frames are linearly interpolated.
 * implements key-frame based motion generation method. Key frames are computed using inverse kinematics, in-between frames are linearly interpolated.
 */
class ActuatorHybridIKSlerp : public SMRActuator, public EMBRKinematicActuator
{
public:
/**
 * \brief default constructor
 */
  ActuatorHybridIKSlerp(Character* _relatedCharacter);

  ~ActuatorHybridIKSlerp();
/**
 * \brief compute intermediate pose by interpolating between start pose and final pose (target pose)
 * \param relativeTime interpolation parameter between 0 and 1.
 */
  void process(float relativeTime);
/**
 * \brief get the resulting pose
 */
  SMRSkeleton *getSkeleton(float attenuationFactor=1.0f);
/**
 * \brief set up a kinematic chain
 * \param _chain 
 */
  void setKinematicChain(const SMRKinematicChain &_chain, SMRSkeleton _stencil, SMRSkeleton* const _refPose);
/**
 * \brief add a target
 */
  void setTarget(const SMRVector3 &target, const char* _jointName, const SMRVector3 &_offset=SMRVector3(0.0,0.0,0.0));

  void setTarget(const char* _jointTarget, const char* _jointName, const SMRVector3 &_offset=SMRVector3(0.0,0.0,0.0), const SMRVector3 &_rotationOffset=SMRVector3(0.0,0.0,0.0));

  void flushConstraintPtrs();

  //inline void setGestureModifier(SMRSmartPtr<GestureModifier> * _gestMod) { m_gestureModifier = _gestMod; };

  void setAngle(float _angle){m_angle = _angle*(float)M_PI/180.0f; m_adjustSwivel = true;}

  void setAdjustSwivel(bool _adjustSwivel){m_adjustSwivel = _adjustSwivel;}

  SMRIKConstraint* getConstraintPtr(const unsigned int index) ;

protected:

  SMRGSMMSolver m_iterativeIKSolver;
  SMRAnalyticalIKSolver m_analyticalIKSolver;
  SMRSwivel m_swivelActuator;

  SMRKinematicChain *m_kinematicChain;
  SMRSkeleton *m_targetPose;                   /** Reference towards the key-frame to be computed (second (and last) key pose). */
  SMRSkeleton m_startPose;                     /** Copy of the initial state of the chain when the actuator is instanciated (fisrt key pose) */
  SMRSkeleton m_returnPose;                    /** resulting pose (interpolated between *m_targetPose and m_startPose) */
  std::vector<SMRIKConstraint*> m_constraints; /** actuator's spatial Constraints */
  float m_currentError;
  float m_lastError;
  float m_iterations;
  bool  m_slowStart;
  bool  m_ikAlreadyPerformed;
  float m_currentGain;
  float m_mean;
  bool m_adjustSwivel;
  bool m_perfomAnalyticalIK;
  float m_angle;
  //SMRSmartPtr<GestureModifier> *m_gestureModifier;

};

#endif
/** @} */

