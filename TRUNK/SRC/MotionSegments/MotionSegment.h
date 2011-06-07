#ifndef _KEYFRAME_H
#define _KEYFRAME_H


//#include "SmrActuator.h"
#include "EMBRRealValuedActuator.h"
//#include "../GestureModifier.h"
#include "EMBRKinematicActuator.h"
#include "../Character.h"


typedef enum MotionSegmentTypeEnum{KINEMATIC_POSE, KINEMATIC_POSE_EYE, MORPH_TARGET, SHADER_TARGET, ANIMATION, AUTONOMOUS_BEHAVIOR_PARAMETER} MotionSegmentType;

class GestureModifier;

/**
 * \class MotionSegment 
 * \brief A motion segment represents an animation
 * for part of the skeleton  over a period of time . For instance,
 * a motion segment may describe a waving motion of the right arm
 * or the blushing of the face. Each motion segment contains at least one 
 * instance of a specialized actuator  which drives the animation.
 */

class MotionSegment {
protected:

  std::vector<ActuatorGenericEMBR*> m_actuators; /**< collection of actuators driving the skeletal and the real valued animations */

  //std::vector<SMRActuator*> m_skeletalActuators; /**< collection of actuators driving the skeletal animation */

  //std::vector<EMBRRealValuedActuator*> m_realValuedActuators; /**< collection of actuators driving the skeletal animation */

  unsigned int m_absoluteStartTime; /**< programmed start time (number of milliseconds since last call to TIMER_RESET) */

  unsigned int m_absoluteStopTime; /**< programmed stop time (number of milliseconds since last call to TIMER_RESET) */

  unsigned int m_holdDuration; /**< how long the final key posture should be hold, must be smaller that (m_absoluteStopTime-m_absoluteStartTime) */

  SMRHermitePoly<double> m_temporalWarpSpline; /**<  time warp profiles that can be applied on any animation element */

  SMRSkeleton m_skeleton; /**< result skeleton */

  float m_relative_fade_in; /**< fade in time (between 0 and 1), default is 0, if x is fade_in value, will linearly fade in between 0 and x */

  float m_relative_fade_out; /**< fade out time (between 0 and 1), default is 1, if x is fade_in value, will linearly fade out between x and 1 */
  
  SMRSmartPtr<GestureModifier> *m_gestureModifier; /**< reference towards a gesture modifier */

  Character* m_relatedCharacter; /**< reference towards the related character */


  //SocketListener *m_socketListener; /** reference towards the socket manager in charge of sending the feedback messages */
  

public:

  MotionSegmentType m_type; /**< specifies the channel which is modified by this motion segment (skeleton, shaders, morph targets) */
  string m_stencil; /**< specifies the subgroup which is modified by this motion segment (eg.: larm, blushing, smile_closed) */

public:
  MotionSegment(Character* _relatedCharacter, float _relative_fade_in=0.0f, float _relative_fade_out=1.0f);
  virtual ~MotionSegment();

 /**
  * \brief convert \param _absoluteTimeMS into relativetime and fire internal actuators
  * \param _absoluteTimeMS time at which embedded actuators should be triggered (in ms from last call to TIME_RESET)
  */
  virtual void process(const unsigned int _absoluteTimeMs, bool _timeForFeedback = true);

  SMRSkeleton* getSkeletonUnsecured();
  SMRSkeleton* getSkeleton();

  void setAbsoluteStartTime(const int _startTime);
  void setAbsoluteStopTime(const int _stopTime);
  void setHoldDuration(const int _holdDuration);

  unsigned int getAbsoluteStartTime(void) const;
  unsigned int getHoldDuration(void) const;
  unsigned int getAbsoluteStopTime(void) const;

  inline std::vector<ActuatorGenericEMBR*> getActuators() const;

  void setGestureModifier(SMRSmartPtr<GestureModifier> * _gestMod);
  
  SMRSmartPtr<GestureModifier>* getGestureModifier() ;

  bool isReferenced(void) const;

  void addActuator(ActuatorGenericEMBR *_actuator);

  //void addRealValuedActuator(EMBRRealValuedActuator *_actuator);

  ActuatorGenericEMBR* getActuator(int _actuatorIndex) const; 

  float getAmount(unsigned int _absoluteTimeMs);

  void setTempWarpSpline(const double p0, const double m0, const double p1, const double m1, const double _tension);

  SMRHermitePoly<double> getTemporalWarpSpline();

};
#endif
