#include "MotionSequence.h"


MotionSequence::MotionSequence(Character* _relatedCharacter, unsigned int _fade_in, unsigned int _fade_out):m_absoluteStartTime(0), m_absoluteStopTime(0), m_fade_in(_fade_in), m_fade_out(_fade_out), m_relatedCharacter(_relatedCharacter), m_gestureModifier(NULL)
{
  m_firstNucleusPose = 0;
  m_lastNucleusPose = 0;
  m_timeWarpProfile = SMRHermitePoly<double>(-1.0,0.0,1.0,2.0);
}

MotionSequence::~MotionSequence()
{
  for_each( m_keyFrames.begin(), m_keyFrames.end(), DeleteObjectPtr());
}

void MotionSequence::setAbsoluteStartTime(const int &_startTime)
{
  m_absoluteStartTime = _startTime;
}

void MotionSequence::setAbsoluteStopTime(const int &_stopTime)
{
  m_absoluteStopTime = _stopTime;
}

unsigned int MotionSequence::getAbsoluteStartTime(void) const 
{
  return m_absoluteStartTime;
}


unsigned int MotionSequence::getAbsoluteStopTime(void) const 
{
  return m_absoluteStopTime;
}

/* function adding the given MotionSegment to the complex one, adaptes global
* start and stop time values
*/
void MotionSequence::addMotionSegment(MotionSegment * _newMotionSegment)
{
  m_keyFrames.push_back(_newMotionSegment);
  //update segment timing
  unsigned int startTime = this->getAbsoluteStartTime();
  unsigned int stopTime = this->getAbsoluteStopTime();
  if (startTime > _newMotionSegment->getAbsoluteStartTime() || startTime == -1) this->setAbsoluteStartTime(_newMotionSegment->getAbsoluteStartTime());
  if (stopTime < _newMotionSegment->getAbsoluteStopTime()) this->setAbsoluteStopTime(_newMotionSegment->getAbsoluteStopTime());
  if (_newMotionSegment->getGestureModifier() != NULL)
  { 
    if (m_gestureModifier == NULL)  m_gestureModifier  = _newMotionSegment->getGestureModifier();
    if (m_firstNucleusPose == 0) m_firstNucleusPose = static_cast<unsigned int> (m_keyFrames.size() - 1);
    m_lastNucleusPose = static_cast<unsigned int> (m_keyFrames.size() - 1);
  }    
}

/*
*  this version computes all skeletons in the first call, then outputs the skeleton
*  corresponding to the given timevalue _absoluteTimePoint
*/ 

void MotionSequence::process(const unsigned int _absoluteTimePoint)
{
  //MotionSegment *firstSegment = *m_keyFrames.begin();
  //firstSegment->process(firstSegment->getAbsoluteStartTime());
  //m_startPose += *(firstSegment->getSkeleton());
  //m_skeleton = m_startPose;
  //for(vector<MotionSegment*>::iterator motionSegmentIterator = this->m_keyFrames.begin(); motionSegmentIterator < this->m_keyFrames.end();motionSegmentIterator++)
  //{ 
  //  (*motionSegmentIterator)->process((*motionSegmentIterator)->getAbsoluteStopTime());
  //} // could be optimized: no need to process all the key Poses, just the 4 (max) we need !
  //float relativeTime = (float)(_absoluteTimePoint-this->getAbsoluteStartTime()) / (float)((this->getAbsoluteStopTime()-this->getAbsoluteStartTime()));
  //if (relativeTime > 1.0f)
  //  relativeTime = 1.0f;
  ////float warpedRelativeTime = static_cast<float>(m_temporalWarpSpline.evaluate(relativeTime));          // WTF ?
  //float warpedRelativeTime = warpedRelativeTime = relativeTime;// static_cast<float> ( m_currentTimeWarpSpline.evaluate(relativeTime));       // WTF ?
  //// get the current interpolation parameters from the active motionSegment
  //vector<MotionSegment*>::iterator motionSegmentIterator = m_keyFrames.begin();
  //unsigned int position = 0;
  //while ( motionSegmentIterator < m_keyFrames.end() &&(*motionSegmentIterator)->getAbsoluteStartTime() < _absoluteTimePoint)
  //{
  //  motionSegmentIterator++;
  //  position++;
  //}
  //if (position > 0)
  //{
  //  position--;                  // WTF ?
  //  motionSegmentIterator--;     // WTF ?
  //}
  
  /**
  WTF?
  //SMRVector3* tensBiasCons = new SMRVector3(0.0,0.0,0.0);
  //if (this->getGestureModifier() != NULL)
  //{
  //  if ( (position >= m_firstNucleusPose -1) && (position <= m_lastNucleusPose+1))
  //  {
  //    this->getGestureModifier()->getPointee()->getInterpolationParameters(tensBiasCons);
  //    if (position<m_firstNucleusPose || position >m_lastNucleusPose)
  //    {
  //      tensBiasCons->m_x = 0.0;
  //      tensBiasCons->m_z = 0.0;
  //    }
  //    unsigned int steps = m_lastNucleusPose - m_firstNucleusPose +2;
  //    float bias =  (float) tensBiasCons->Y();
  //    this->getGestureModifier()->getPointee()->interpolateBias(bias,position - (m_firstNucleusPose -1),steps);
  //    tensBiasCons->m_y = bias;
  //  }
  **/
  }