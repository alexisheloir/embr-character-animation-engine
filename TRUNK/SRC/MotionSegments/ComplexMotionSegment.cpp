#include "ComplexMotionSegment.h"

ComplexMotionSegment::~ComplexMotionSegment()
{
  for_each( m_motionSegments.begin(), m_motionSegments.end(), DeleteObjectPtr());
}

ComplexMotionSegment::ComplexMotionSegment(Character* _currentCharacter, float _relative_fade_in, float _relative_fade_out):MotionSegment(_currentCharacter, _relative_fade_in, _relative_fade_out)
{
  m_type = MotionSegment::ABSOLUTE_KINEMATIC_POSE;
  m_holdDuration = 0;
  m_gestureModifier = NULL;
  m_firstNucleusPose = 0;
  m_lastNucleusPose = 0;
  m_temporalWarpSpline = SMRHermitePoly<double>(-1.0,0.0,1.0,2.0);
}

/* function adding the given MotionSegment to the complex one, adaptes global
* start and stop time values
*/
void ComplexMotionSegment::addMotionSegment(MotionSegment * _newMotion)
{
  m_motionSegments.push_back(_newMotion);
  //update segment timing
  unsigned int startTime = this->getAbsoluteStartTime();
  unsigned int stopTime = this->getAbsoluteStopTime();
  if (startTime > _newMotion->getAbsoluteStartTime() || startTime == 0) this->setAbsoluteStartTime(_newMotion->getAbsoluteStartTime());
  if (stopTime < _newMotion->getAbsoluteStopTime()) this->setAbsoluteStopTime(_newMotion->getAbsoluteStopTime());
  if (_newMotion->getGestureModifier() != NULL)
  { 
    if (m_gestureModifier == NULL)  m_gestureModifier  = _newMotion->getGestureModifier();
    if (m_firstNucleusPose == 0) m_firstNucleusPose = static_cast<unsigned int> (m_motionSegments.size() - 1);
    m_lastNucleusPose = static_cast<unsigned int> (m_motionSegments.size() - 1);
  }    
}

/* 
 * After closing the complex motion segment, combine processed key point skeletons and fills the map 
 * with the time/skel pairs, if less then 3 poses, interpolation mode is set to LINEAR
 */
void ComplexMotionSegment::combine()
{
  m_absoluteStartTime = (*m_motionSegments.begin())->getAbsoluteStartTime();
  m_absoluteStopTime = (*(m_motionSegments.end()-1))->getAbsoluteStopTime();  
  if (m_gestureModifier != NULL)
  { 
    LOG_INFO(modifierLogger,"applying modifier to the Complex Motion Segment");
    GestureModifier * gestMod = m_gestureModifier->getPointee(); 
    SMRVector3 lowerBound(0.0,0.0,0.0),higherBound(0.0,0.0,0.0);
    LOG_INFO(modifierLogger,"Nucleus reaches from pose "<<m_firstNucleusPose<<" to "<<m_lastNucleusPose);
    gestMod->applyPower(m_motionSegments,m_firstNucleusPose);
    gestMod->computeBoundingBox(m_motionSegments,lowerBound,higherBound);
    gestMod->spatialExtent(m_motionSegments,lowerBound,higherBound);
    unsigned int delay = 0;
    gestMod->applyTemporalExtent(m_motionSegments,delay,m_firstNucleusPose,m_lastNucleusPose);
    m_absoluteStopTime +=delay;
    LOG_INFO(modifierLogger,"CMSSTOP Time is: "<<m_absoluteStopTime);   
  }
  const unsigned int duration = this->getAbsoluteStopTime() - this->getAbsoluteStartTime();
  MotionSegment *firstSegment = *m_motionSegments.begin();
  m_startPose = *(firstSegment->getSkeletonUnsecured());//what if motion segment doesn't have a skeleton?
  m_timedSkeletons.insert(std::pair<float,SMRSkeleton*>(0.0f, &m_startPose)); // put the current skeleton there
  for(vector<MotionSegment*>::iterator motionSegmentIterator = this->m_motionSegments.begin(); motionSegmentIterator < this->m_motionSegments.end();motionSegmentIterator++)
  {
    MotionSegment* currentMotionSegment = *motionSegmentIterator;
    unsigned int timePointMS = currentMotionSegment->getAbsoluteStopTime() - this->getAbsoluteStartTime();
    if (currentMotionSegment->getHoldDuration() > 0)
    {
      float timePoint = (float)(timePointMS - currentMotionSegment->getHoldDuration())/(float)(duration);
      m_timedSkeletons.insert(std::pair<float,SMRSkeleton*>(timePoint,(*motionSegmentIterator)->getSkeletonUnsecured()));
    }
    float timePoint = (float)(timePointMS)/(float)(duration);
    //Should be the last pose of the complex motionSegment. Remove possible pose that would occur after because of a too long hold time
    std::map<float,SMRSkeleton* const>::iterator lastPose = m_timedSkeletons.end();
    lastPose--;
    if (lastPose->first > timePoint)
    {
      //m_timedSkeletons.erase(lastPose);
      LOG_ERROR(logger, "the currently processed hold phase should override a  keypose");
    }
    m_timedSkeletons.insert(std::pair<float,SMRSkeleton*>(timePoint,(*motionSegmentIterator)->getSkeletonUnsecured()));
  }
  m_moSegIterator = m_motionSegments.begin();
  m_currentTimeWarpSpline = (*m_moSegIterator)->getTemporalWarpSpline();
  m_interpolationDone = false;
}

/*
 *  this version computes all skeletons in the first call, then outputs the skeleton
 *  corresponding to the given timevalue _absoluteTimePoint
 */ 

void ComplexMotionSegment::process(const unsigned int _absoluteTimePoint, bool _timeForFeedback)
{

  MotionSegment *firstSegment = *m_motionSegments.begin();
  firstSegment->process(firstSegment->getAbsoluteStartTime());
  m_startPose += *(firstSegment->getSkeleton());
  m_skeleton = m_startPose;
  for(vector<MotionSegment*>::iterator motionSegmentIterator = this->m_motionSegments.begin(); motionSegmentIterator < this->m_motionSegments.end();motionSegmentIterator++)
  { 
    
    unsigned int motionSegmentStopTime = (*motionSegmentIterator)->getAbsoluteStopTime();
    if( _absoluteTimePoint > motionSegmentStopTime-5 && _absoluteTimePoint < motionSegmentStopTime+5)
    {
      (*motionSegmentIterator)->process(motionSegmentStopTime, true);
    }else
    {
      (*motionSegmentIterator)->process(motionSegmentStopTime, false);
    }
  } // could be optimized: no need to process all the key Poses, just the 4 (max) we need !
  float relativeTime = (float)(_absoluteTimePoint-this->getAbsoluteStartTime()) / (float)((this->getAbsoluteStopTime()-this->getAbsoluteStartTime()));
  if (relativeTime > 1.0f)
    relativeTime = 1.0f;
  //float warpedRelativeTime = static_cast<float>(m_temporalWarpSpline.evaluate(relativeTime)); 
  float warpedRelativeTime = relativeTime;// static_cast<float> ( m_currentTimeWarpSpline.evaluate(relativeTime));
  // get the current interpolation parameters from the active motionSegment
  vector<MotionSegment*>::iterator motionSegmentIterator = m_motionSegments.begin();
  unsigned int position = 0;
  while ( motionSegmentIterator < m_motionSegments.end() &&(*motionSegmentIterator)->getAbsoluteStartTime() < _absoluteTimePoint)
  {
    motionSegmentIterator++;
    position++;
  }
  if (position > 0)
  {
    position--;                  
    motionSegmentIterator--;     
  }

  SMRVector3* tensBiasCons = new SMRVector3(0.0,0.0,0.0);
  if (this->getGestureModifier() != NULL)
  {
    if ( (position >= m_firstNucleusPose -1) && (position <= m_lastNucleusPose+1))
    {
      this->getGestureModifier()->getPointee()->getInterpolationParameters(tensBiasCons);
      if (position<m_firstNucleusPose || position >m_lastNucleusPose)
      {
        tensBiasCons->m_x = 0.0;
        tensBiasCons->m_z = 0.0;
      }
      unsigned int steps = m_lastNucleusPose - m_firstNucleusPose +2;
      float bias =  (float) tensBiasCons->Y();
      this->getGestureModifier()->getPointee()->interpolateBias(bias,position - (m_firstNucleusPose -1),steps);
      tensBiasCons->m_y = bias;
    } 
  }

  LOG_TRACE(logger," processing complex motion segment with relative time: "<<relativeTime<<" warped relativeTime: " << warpedRelativeTime <<" m_absoluteStopTim= " << this->getAbsoluteStopTime()<< " absoluteTimeMs= " << _absoluteTimePoint);
  LOG_INFO(modifierLogger,"Interpolating with parameters tension = "<<tensBiasCons->X()<<", bias = "<<tensBiasCons->Y()<<", continuity "<<tensBiasCons->Z());
  if ( this->m_type == MotionSegment::ABSOLUTE_KINEMATIC_POSE)
  {
    InterpolateSkeletons(m_timedSkeletons, m_skeleton, warpedRelativeTime, static_cast<float>(tensBiasCons->X()),static_cast<float>(tensBiasCons->Y()),static_cast<float>(tensBiasCons->Z()));
  }

}



