

#include "Scheduler.h"

SMRSkeleton* const Scheduler::getCurrentPose() {
  LOG_TRACE(logger,"get currentPose");
  return m_poseToBeDisplayed;
}

void Scheduler::setCurrentPose(const SMRSkeleton *_pose) {
  //m_resultPose = *_pose;
}

Scheduler::Scheduler(Character *_character, SMRSkeleton* const _referencePose):m_missedFrames(0),m_poseToBeDisplayed(_referencePose),m_clockOffset(0),m_getCurrentPoseWasHere(false)
{
  m_character = _character;
  motion.setTimeStep(40.0);
}

Scheduler::~Scheduler() {
  for_each( m_motionSegments.begin(), m_motionSegments.end(), DeleteObjectPtr());
}

void Scheduler::schedule()
{
  LOG_TRACE(logger,"new scheduling loop ! ");
  boost::mutex::scoped_lock lockSchedulerFlusher(schedulerFlusherMutex);
  std::list<MotionSegment*>::iterator iterator;
  LOG_TRACE(schedulerLogger, m_motionSegments.size()<<" Motionsegments in the heap"<<endl);
  iterator = m_motionSegments.begin();
  bool skeletonHasBeenUpdated = false;
  SMRSkeleton resultPose;
  if (m_motionSegments.size() == 0 ) boost::this_thread::sleep(boost::posix_time::milliseconds(100)); // TODO  use flags and move this to the schedulerManager
  else boost::this_thread::sleep(boost::posix_time::milliseconds(20));
  while ( iterator != m_motionSegments.end() )
  {
    MotionSegment* currentSegment;
    currentSegment = *iterator;
    unsigned int currentTimeMs = getCurrentTimeMs();
    LOG_TRACE(schedulerLogger, "startTime: " << currentSegment->getAbsoluteStartTime() << " || " \
                               "stopTime: " << currentSegment->getAbsoluteStopTime());

    if((currentTimeMs > currentSegment->getAbsoluteStartTime() && currentTimeMs < currentSegment->getAbsoluteStopTime()) || (currentTimeMs > currentSegment->getAbsoluteStartTime() && currentSegment->getAbsoluteStopTime() == 0 )) // find a better way to handle relative postures here, rather use a consistent typology
    {
      boost::mutex::scoped_lock lockSkeletonModifier(skeletonMutex);
      LOG_TRACE(schedulerLogger,"updating posture " << " with absolute time: " << currentTimeMs);
      
      m_character->m_isReadyToBeDisplayed = false;
      currentSegment->process(currentTimeMs);
      m_character->m_isReadyToBeDisplayed = true;
  
      //LOG_TRACE(schedulerLogger,"combining resulting pose with currentSkeleton (" << currentSegment->getSkeleton()->getName() << ") at " << currentTimeMs << "ms");
      SMRSkeleton* resultSkeleton = currentSegment->getSkeleton();
      if(resultSkeleton) skeletonHasBeenUpdated = true;  
      switch (currentSegment->getType()) {
        case MotionSegment::MORPH_TARGET:
        // currently does nothing, but should update the corresponding morph target weight here
          break;
        case MotionSegment::ABSOLUTE_KINEMATIC_POSE:
          resultPose.combine(*(resultSkeleton),currentSegment->getAmount(currentTimeMs)); // amount is for fade_in / fade_out
          break;
        case MotionSegment::ANIMATION:
          resultPose.combine(*(resultSkeleton),currentSegment->getAmount(currentTimeMs)); // amount is for fade_in / fade_out
          break;
        case MotionSegment::RELATIVE_KINEMATIC_POSE:
          (resultPose)*=(*(resultSkeleton));
          break;                
        default:
          break;
      }
    }
    if (currentTimeMs > currentSegment->getAbsoluteStopTime() && currentSegment->getAbsoluteStopTime() > 0) //segment whose stoptime is 0 are autonomous
    {
      LOG_DEBUG(schedulerLogger,"Motion segment is deprecated, remove it: MS time :" << currentSegment->getAbsoluteStopTime() << " current time : " << currentTimeMs);
      MotionSegment* segmentToFlush= *iterator;
      iterator = m_motionSegments.erase(iterator);
      delete(segmentToFlush);// !remove this line when using factory!          
    }else
    {
      iterator++;
    }
  }
  if (skeletonHasBeenUpdated)
  {
    LOG_TRACE(logger,"updating overall pose ");
    *m_poseToBeDisplayed += resultPose;
    LOG_TRACE(logger,"update done ");
  }
  m_getCurrentPoseWasHere = false;
}

void Scheduler::flush(void)
{
  boost::mutex::scoped_lock lock(schedulerFlusherMutex);
  std::list<MotionSegment*>::iterator iterator;
  iterator = m_motionSegments.begin();
  while ( iterator != m_motionSegments.end() )
  {
    MotionSegment* currentSegment;
    currentSegment = *iterator;
    if(currentSegment->getAbsoluteStopTime() != 0 ) 
    {
      MotionSegment* segmentToFlush= *iterator;
      iterator = m_motionSegments.erase(iterator);
      delete(segmentToFlush);// !remove this line when using factory!
    }else
    {
      MotionSegment* segmentToOffset= *iterator;
      float segmentStartTime = segmentToOffset->getAbsoluteStartTime();
      if( segmentStartTime > m_clockOffset)
      {
        segmentToOffset->setAbsoluteStartTime(segmentStartTime-m_clockOffset);
      }else
      {
        segmentToOffset->setAbsoluteStartTime(0.0f);
      }
      iterator++;
    }
  }
}

void Scheduler::setClockOffset(const unsigned int _clockOffset){
  unsigned int currentTime = getCurrentTimeMs();
  m_clockOffset = _clockOffset;
  std::list<MotionSegment*>::iterator motionSegmentIterator;
  for (motionSegmentIterator = m_motionSegments.begin(); motionSegmentIterator != m_motionSegments.end(); motionSegmentIterator++)
  {
    unsigned int segmentStartTime = (*motionSegmentIterator)->getAbsoluteStartTime();
    unsigned int segmentStopTime = (*motionSegmentIterator)->getAbsoluteStopTime();
    if ( segmentStartTime > currentTime)
    {
      (*motionSegmentIterator)->setAbsoluteStartTime( segmentStartTime - currentTime);
      if (segmentStopTime != 0)
      {
        (*motionSegmentIterator)->setAbsoluteStopTime(segmentStopTime - currentTime);
      }
    }else
    {
      (*motionSegmentIterator)->setAbsoluteStartTime(getCurrentTimeMs());
      if (segmentStopTime != 0)
        (*motionSegmentIterator)->setAbsoluteStopTime(getCurrentTimeMs());
    }
  }
}

unsigned int Scheduler::getCurrentTimeMs()
{
  LOG_TRACE(schedulerLogger,"get current time: " << clock() << " CLOCKS_PER_SEC " << CLOCKS_PER_SEC);
  unsigned int currTime = SMRUtils::getCurrentTime() - m_clockOffset;

  LOG_TRACE(schedulerLogger,"get current time: " << currTime );
  return currTime;
}
void Scheduler::enqueueMotionSegment(MotionSegment *_segment)
{
  LOG_DEBUG(schedulerLogger,"adding a new motion segment with stop time: " << _segment->getAbsoluteStopTime() << " current time : " << getCurrentTimeMs());
  boost::mutex::scoped_lock lock(motionSegmentMutex);
  list<MotionSegment*>::iterator segmentIterator = m_motionSegments.begin();
  while (segmentIterator != m_motionSegments.end())
  {
    MotionSegment *currentSegment = *segmentIterator;
    if (currentSegment->getAbsoluteStartTime() > _segment->getAbsoluteStartTime() && currentSegment->getAbsoluteStopTime() != 0.0f )
    {
      m_motionSegments.insert(segmentIterator,_segment);
      return;
    }
    segmentIterator++;
  }
  m_motionSegments.push_back(_segment);
}

void Scheduler::skeletonRequested(){
  if (m_getCurrentPoseWasHere)
  {
    m_missedFrames++;
  }else if (m_missedFrames > 0)
  {
    LOG_DEBUG(schedulerLogger, m_missedFrames << " frame(s) lost!");
    m_missedFrames = 0;
  }
  m_getCurrentPoseWasHere = true;

}

string Scheduler::getRelatedCharacter(void)
{
  return m_character->m_name;
}

