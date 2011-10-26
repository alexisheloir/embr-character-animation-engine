#ifndef _MOTIONSEQUENCE_H
#define _MOTIONSEQUENCE_H

#include "../Character.h"
#include "../Actuators/ActuatorGenericEMBR.h"
#include "ComplexMotionSegment.h"

#include <limits>

//typedef enum InterpolationType {LINEAR, CUBIC};

class MotionSequence{
  protected:
    map<float, SMRSkeleton*> m_timedSkeletons; /** results as skeleton **/
    map<float, RealValuedResult*> m_timedRealResults; /** results as reals **/

    bool m_interpolationDone; 

    unsigned int m_absoluteStartTime; /**< programmed start time (number of milliseconds since last call to TIMER_RESET) */
    unsigned int m_absoluteStopTime;  /**< programmed stop time (number of milliseconds since last call to TIMER_RESET) */
    unsigned int m_fade_in;         /**< fade in time (between 0 and 1), default is 0, if x is fade_in value, will linearly fade in between 0 and x */
    unsigned int m_fade_out;        /**< fade out time (between 0 and 1), default is 1, if x is fade_in value, will linearly fade out between x and 1 */

    vector<MotionSegment *> m_keyFrames;
    vector<MotionSegment *>::iterator m_KFramesIterator;

    unsigned int m_firstNucleusPose;
    unsigned int m_lastNucleusPose;

    SMRHermitePoly<double> m_timeWarpProfile;

    Character* m_relatedCharacter;

    SMRSmartPtr<GestureModifier> *m_gestureModifier; /**< reference towards a gesture modifier */


public:
  MotionSequence(Character* _relatedCharacter, unsigned int _fade_in=0, unsigned int _fade_out=0);
  ~MotionSequence();

  void addMotionSegment(MotionSegment * _newMotionSegment);
  void combine();
  void process(const unsigned int _absoluteTimeMs);

  unsigned int getAbsoluteStartTime(void) const;
  int getAbsoluteStopTime(void) const;

  void setAbsoluteStartTime(const int &_startTime);
  void setAbsoluteStopTime(const int &_stopTime);

  void setFadeInMs(const unsigned int &_fade_in){m_fade_in = _fade_in;}
  void setFadeOutMs(const unsigned int &_fade_out){m_fade_out = _fade_out;}

};


#endif