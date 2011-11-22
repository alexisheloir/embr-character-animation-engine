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


#ifndef _ACTUATORFACTORY_H
#define _ACTUATORFACTORY_H

#include "Scheduler.h"
#include "MotionSegment.h"
#include "Scheduler.h"
#include "SchedulerManager.h"

#include "SmrActuator.h"
#include "SmrMotionPlayer.h"
#include "ActuatorMorphAnimation.h"
#include "SmrIKSolver.h"
#include "SmrLoader.h"
#include "ActuatorIKSlerp.h"
#include "ActuatorHybridIKSlerp.h"
#include "ActuatorMorphTarget.h"
#include "ActuatorShaderParameter.h"
#include "ActuatorAutonomousBehaviorParameter.h"
#include "ActuatorAnimation.h"
#include "ActuatorCorrelationMap.h"
#include "SmrIO.h"
#include "SelfPostingMotionSegment.h"
#include "RandomPostingMotionSegment.h"
#include "AnimationMotionSegment.h"
#include "SmrSmartPointer.h"
#include "GestureModifier.h"
#include "ComplexMotionSegment.h"
#include "Character.h"

#include "MotionSegments/MotionSequence.h"
#include "MotionSegments/MotionSegment.h"

#include <libxml/xmlreader.h>
#include <libxml/tree.h>




/**
* \class ActuatorFactory
* This class is a factory whose goal is to create and configure the MotionSegments according to their parsed parameters
*/
class ActuatorFactory {

public:
  /**
  * Default constructor
  */
  ActuatorFactory();
  /**
  * Destructor
  */
  ~ActuatorFactory();

public:
  /**
  * Function called by the Parser in order to create a motion segemnt and post the created segment to the cheduler's queue.
  * \param _scheduler reference towards the scheduler in charge of handling the segment
  * \param _startTime when shall the motion segment start (timestamp relative to the last time NEWCOMMAND has been parsed)
  * \param _stopTime when shall the motion segment stop (timestamp relative to the last time NEWCOMMAND has been parsed)
  * \param _holdDuration how long the pose should be hold
  * \param _actuatorType what kind of actuator should contain the motion segment
  * \param _stencil joint group that is modified by the motion segment
  * \param _firstActuatorParam first parameter of the motionSegment's actuator
  * \param _secondActuatorParam second parameter of the motionSegment's actuator
  * \param _thirdActuatorParam third parameter of the motionSegment'S actuator
  * \param _gestMod a reference towarda gesture modifier mdifying the actual motion segment
  */
  void setupAndPostMotionSegment(Scheduler * _scheduler, int _startTime, int _stopTime, int _holdDuration, string _actuatorType, string _stencil, string _firstActuatorParam="", string _secondActuatorParam="", string _thirdActuatorParam="", SMRSmartPtr<GestureModifier> * _gestMod=NULL, bool _feedBackRequired=false, string messageID="");

  void setupAndPostMorphTargetSegment(Scheduler * _scheduler, int _startTime, int _stopTime, int _holdDuration, string _morphKey, float _morphWeight, float relativeFadeIn=0.0f, float relativeFadeOut=0.0f);

  void setupAndPostShaderParameterSegment(Scheduler * _scheduler, int _startTime, int _stopTime, int _holdDuration, string _shaderKey, float _shaderValue, float relativeFadeIn=0.0f, float relativeFadeOut=0.0f);

  void setupAndPostAnimationSegment(Scheduler * _scheduler, int _startTime, int _stopTime, int _holdDuration, string _stencil, string _animationKey, float relativeFadeIn=0.0f, float relativeFadeOut=0.0f);

  void setupAndPostStoredPoseSegment(Scheduler * _scheduler, int _startTime, int _stopTime, int _holdDuration, string _stencil, string _animationKey, float _attenuationFactor=1.0f, float relativeFadeIn=0.0f, float relativeFadeOut=0.0f);

  void setupAndPostAutonomousBehaviorParameterSegment(Scheduler * _scheduler, int _startTime, int _stopTime, string _behaviorParameterKey, float _behaviorparameterValue, float relativeFadeIn=0.0f, float relativeFadeOut=0.0f);

  void setupAndPostCorrelationMapMotionSegment(Scheduler * _scheduler);

  void setupAndPostMotionSegment(Scheduler * _scheduler, int _startTime, float relativeFadeIn=0.0f, float relativeFadeOut=0.0f); //TODO: implement this


  void addMotionSegmentToMotionSegment(MotionSequence * _motionSequence, int _timePoint); //TODO: implement this
  void addSkeletalConstraintToMotionSegment(MotionSegment * _keyFrame); //TODO: implement this

  void transformToPandaSkeleton(SMRMotion & _bvhMotion,SMRSkeleton  _pandaSkeleton);
  /**
  * \brief get the T-pose of a character by it's name
  * \param _characterName name of the character whose pose belongs
  */
  const SMRSkeleton* getCharacterTPosture(const string &_characterName);

  /**
  * \brief get the Animation-pose of a character by it's name
  * \param _characterName name of the character whose pose belongs
  */
  SMRSkeleton* const getCharacterAnimationPosture(const string &_characterName);

 /**
  * \brief add a new character
  * \param _characterName name of the character to be added
  */
  void addCharacter(const string &_characterName, SMRSkeleton* const _referencePose);

  /**
  * \brief send the current motion segments to the scheduler
  * \param _scheduler reference towards the scheduler in charge of handling the Motion Segmemts
  */
  void sendMotionSegments(Scheduler *_scheduler);

  /**new papo
  * initializes the combine process for a K_POSE_SEQ to a ComplexMotionSegment
  */
  void combinePoseSequence(string _characterName, float fadeIn, float fadeOut, string _timeWarpSym, float _timeWarpStipness);

 /**
  * \brief Add a new animation
  */
  void addAnimation(string _characterName, string _animationKey);

 /**
  * \brief Add a pose to an animation
  */
  void addPoseToAnimation(string _characterName, string _animationName, SMRSkeleton _pose);

 /**
  * \brief register the actual scheduler manager to the current actuator factory
  */
  void setSchedulerManager(SchedulerManager* _schedulerManager);

 /**
  * \brief retrieve a character by its name
  * return a reference toward the requested character
  */
  Character* getCharacter(const string &_characterName);

/**
 * \brief change the current feedback message
 * this method update the value of the feedback meesage which is currently used 
 * to bring some information from the control layer to the environment (i.e. the
 * scene graph manager of the 3D engine)
 */
void setFeedbackMessage(const string &message);

/**
 * \brief get the last registered feedback message
 * this method is exposed in Python and currentls only used in the panda3D application runtime
 */
string getFeedbackMessage(void);

public:
 /**
  * \brief parse a character configuration file (describes characters)
  */
  void init(string _characterConfigurationFileContent);

protected:
 /**
  * \brief ActuatorFactory is a singleton class (one instance through the runtime)
  */
  static ActuatorFactory *m_singleton;


public:
  static ActuatorFactory *getInstance( void );
  static void kill( void );

protected:
  std::vector<MotionSegment*> m_segmentToSend;
  std::vector<MotionSegment*> m_segmentToCombine;

  std::map<string,Character*> m_characters; 

  SchedulerManager *m_schedulerManager;
  SMRVector3 m_previousVector;

  string m_feedbackMessage;

public:
};

#endif
