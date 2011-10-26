#ifndef _RANDOMPOSTINGMOTIONSEGMENT_H
#define _RANDOMPOSTINGMOTIONSEGMENT_H

#include "ComplexMotionSegment.h"
#include "SmrUtils.h"
/**
 * This class is a container class for an actuator. As 
 * Actuator have no clue about absolute timing information,
 * MacroEvent class wraps a reference towards an Actuator 
 * instance, a start time and an end time. In the future, 
 * information about ease in and ease out will be provided aswell.
 */
class RandomPostingMotionSegment: public ComplexMotionSegment {
  protected:
    string m_incrementParameterKey;
    float m_counter;
    float m_increment;
    unsigned int m_nextPostTimeOffsetInMs;
    int m_lastTick;
    bool m_blinked;

  public:

    RandomPostingMotionSegment(Character * _relatedCharacter, string _incrementParameterKey, unsigned int _nextPostTimeOffsetInMs = 0, float _relative_fade_in=0.0f, float _relative_fade_out=1.0f): ComplexMotionSegment(_relatedCharacter, _relative_fade_in,_relative_fade_out), m_incrementParameterKey(_incrementParameterKey), m_counter(0.0f),m_nextPostTimeOffsetInMs(_nextPostTimeOffsetInMs){
    m_lastTick = SMRUtils::getCurrentTime();}

    void process(const unsigned int _absoluteTimeMs, bool _timeForFeedback = true);
};
#endif

