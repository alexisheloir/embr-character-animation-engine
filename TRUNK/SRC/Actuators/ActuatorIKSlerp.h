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

#ifndef _ACTUATORIKSLERP_H
#define _ACTUATORIKSLERP_H

#include "SmrInvJacSolver.h"
#include "SmrGSMMSolver.h"

#include "../EMBRLoggers.h"
#include "EMBRKinematicActuator.h"

/**
 * \class ActuatorIKSlerp
 * \brief implements key-frame based motion generation method. Key frames ars computed using inverse kinematics, in-between frames are linearly interpolated.
 * implements key-frame based motion generation method. Key frames are computed using inverse kinematics, in-between frames are linearly interpolated.
 */
class ActuatorIKSlerp : public EMBRKinematicActuator, public SMRGSMMSolver
{
public:
/**
 * \brief default constructor
 */
  ActuatorIKSlerp(Character* _relatedCharacter);
/**
 * \brief default destructor
 */
  ~ActuatorIKSlerp();
/**
 * \brief compute target pose using inverse kinematics
 */
  void computeTargetPose();
/**
 * \brief compute intermediate pose by interpolating between start pose and final pose (target pose)
 * \param relativeTime interpolation parameter between 0 and 1.
 */
  void process(float relativeTime);
/**
 * \brief get the resulting pose
 */
  SMRSkeleton * getSkeleton(float attenuationFactor=1.0f);
/**
 * \brief set up a kinematic chain
 * \param _chain 
 */
  void setKinematicChain(SMRKinematicChain &_chain, SMRSkeleton _stencil, SMRSkeleton *_refPose);
/**
 * \brief add a target
 */
  void setTarget(const SMRVector3 &target, const char* _jointName, const SMRVector3 &_offset=SMRVector3(0.0,0.0,0.0));
/**
 * \brief add a target, add offsests
 */    
  void setTarget(const char* _jointTarget, const char* _jointName, const SMRVector3 &_offset=SMRVector3(0.0,0.0,0.0), const SMRVector3 &_rotationOffset=SMRVector3(0.0,0.0,0.0));


protected:
  SMRSkeleton *m_targetPose; /**< Reference towards the key-frame to be computed (second (and last) key pose), usually a simple reference towards the actual skeleton stored in the active actuator.*/
  SMRSkeleton m_startPose; /**< Copy of the initial state of the chain when the actuator is instanciated (fisrt key pose) */
  SMRSkeleton m_returnPose; /** resulting pose (interpolated between *m_targetPose and m_startPose) */
  std::vector<SMRIKConstraint*> m_constraints; /**< actuator's spatial Constraints */
  float m_currentError; /** inverse kinematics error for the current iteration */
  float m_lastError; /** Inverse kinematics error for the previous iteration*/
  float m_currentGain; /** Gain applied to the jacobian transpose */
  bool m_slowStart; /** Gain is computed like the window in a tcp connexion. depending on error */
  float m_increment; /** ? */
  float m_iterations; /** how muck IK iterations have been performed so far */

public:
  bool m_ikAlreadyPerformed; /**< is the targetPose ready ? */

};

#endif
/** @} */
