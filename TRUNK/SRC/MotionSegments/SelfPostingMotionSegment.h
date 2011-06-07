#ifndef _SELFPOSTINGMOTIONSEGMENT_H
#define _SELFPOSTINGMOTIONSEGMENT_H

#include "../ActuatorFactory.h"
#include "SmrUtils.h"
/**
 * This class is a container class for an actuator. As 
 * Actuator have no clue about absolute timing information,
 * MacroEvent class wraps a reference towards an Actuator 
 * instance, a start time and an end time. In the future, 
 * information about ease in and ease out will be provided aswell.
 */
class SelfPostingMotionSegment: public ComplexMotionSegment {
  protected:
    string m_incrementParameterKey;
    float m_counter;
    float m_increment;
    unsigned int m_nextPostTimeOffsetInMs;
    int m_lastTick;

  public:

    SelfPostingMotionSegment(Character *_relatedCharacter, string _incrementParameterKey, unsigned int _nextPostTimeOffsetInMs = 0, float _relative_fade_in=0.0f, float _relative_fade_out=1.0f):ComplexMotionSegment(_relatedCharacter,_relative_fade_in,_relative_fade_out), m_incrementParameterKey(_incrementParameterKey), m_counter(0.0f), m_nextPostTimeOffsetInMs(_nextPostTimeOffsetInMs){
    /*timeval timeVal;
    gettimeofday(&timeVal, NULL);
    m_lastTick = (unsigned int) ((timeVal.tv_sec * 1000.0) + (timeVal.tv_usec / 1000.0));
    */
    m_lastTick = SMRUtils::getCurrentTime();
    }
    void process(const unsigned int _absoluteTimeMs, bool _timeForFeedback = true);
};
#endif

