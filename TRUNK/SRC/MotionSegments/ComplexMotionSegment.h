#ifndef _COMPLEXMOTIONSEGMENT_H
#define _COMPLEXMOTIONSEGMENT_H

//#include "../ActuatorFactory.h"
#include "MotionSegment.h"
#include "../EMBRLoggers.h"
#include <limits>
#include "../GestureModifier.h"

typedef enum InterpolationType {LINEAR, CUBIC};
/**
 * This class is a container class for an actuator. As 
 * Actuator have no clue about absolute timing information,
 * MacroEvent class wraps a reference towards an Actuator 
 * instance, a start time and an end time. The ComplexMotionSegment
 * combines several MotionSegments and allows squad interpolation
 * over these elements
 */
class ComplexMotionSegment: public MotionSegment {
  protected:
    // string m_interpolationMethod;
    map<float, SMRSkeleton* const> m_timedSkeletons;
    
    // vector containing original motion segments added to the ComplexMotionSegment
    vector<MotionSegment *> m_motionSegments;
    vector<MotionSegment *>::iterator m_moSegIterator;
    // array storing interpolated skeletons (temporary)
    vector<SMRSkeleton> m_interpolatedSkeletons;
      
    unsigned int m_firstNucleusPose;
    unsigned int m_lastNucleusPose;
    
    bool m_interpolationDone;      
      
    map< float, SMRSkeleton*>::iterator m_skelForIntIt;
    SMRHermitePoly<double> m_currentTimeWarpSpline;
    
    float update(const float _relativeTimePoint);

    SMRSkeleton m_startPose;

public:
    ComplexMotionSegment(Character* _currentCharacter, float _relative_fade_in=0.0f, float _relative_fade_out=1.0f);
    ~ComplexMotionSegment();

    void addMotionSegment(MotionSegment * _newMotion);
    void combine();
    void process(const unsigned int _absoluteTimeMs, bool _timeForFeedback = true);
    
};
#endif
