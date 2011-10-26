#ifndef _ANIMATIONMOTIONSEGMENT_H
#define _ANIMATIONMOTIONSEGMENT_H

#include "../ActuatorFactory.h"
#include "SmrUtils.h"
/**
 * This class is a container class for an actuator. As 
 * Actuator have no clue about absolute timing information,
 * MacroEvent class wraps a reference towards an Actuator 
 * instance, a start time and an end time. In the future, 
 * information about ease in and ease out will be provided aswell.
 */
class AnimationMotionSegment: public ComplexMotionSegment {
  protected:
    //ActuatorAnimation *m_animation;

  public:

    AnimationMotionSegment(Character *_relatedCharacter, float _relative_fade_in=0.0f, float _relative_fade_out=1.0f):ComplexMotionSegment(_relatedCharacter,_relative_fade_in,_relative_fade_out){
    }
    void process(const unsigned int _absoluteTimeMs, bool _timeForFeedback = true);
};
#endif

