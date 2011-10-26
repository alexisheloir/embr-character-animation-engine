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

#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include "ComplexMotionSegment.h"
#include "SmrMotion.h"
#include <time.h>
#include "SmrUtils.h"
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
//#include "ComplexMotionSegment.h"


#include "EMBRLoggers.h"

static boost::mutex parserMutex;
static boost::mutex schedulerMutex;
static boost::mutex motionSegmentMutex;
static boost::mutex schedulerFlusherMutex;
static boost::mutex skeletonMutex;

typedef enum{LOCAL,EXTERNAL} LOCKER_ID;

typedef
struct{
  bool isLocked;
  LOCKER_ID lockerID;
} pythonCompliantExoLock;


/**
 * \class Scheduler 
 * \brief this class is in charge of  regularly invoking the all the
 * Actuators referenced into its MacroEvents collection. Plus 
 * it is in charge of removing the references to outdated MacroEvents
 */
class Scheduler{

  protected:
    std::list<MotionSegment*> m_motionSegments; /**< list of motion segments handled by the scheduler */
    int m_missedFrames;  /**< missed frames between two skeleton requests. performance evaluation purpose */
    string m_character;  /**< name of the character managed by the scheduler */

  public:
    SMRMotion motion; /**< a data structure which stores a sequence of poses (to be exported as a bvh file for debugging purpose) */


    /**
     * \brief skeleton has been requested by display (performance evaluation purpose)
     */
    void skeletonRequested();

    /**
     * \brief return the name of character handled by scheduler
     * \return name of the character handled by scheduler 
     */
    string getRelatedCharacter(void);

    /**
     * \brief get the number of milliseconds elapsed since last call to clock reset
     * \return number of milliseconds elapsed since last call to clock reset
     */
    unsigned int getCurrentTimeMs();

    /**
     * \brief get current skeleton configuration for the current character
     * \return current skeleton configuration for the current character
     */
    SMRSkeleton* const getCurrentPose();

    /**
     * \brief set the initial pose for the character handled by this scheduler
     * \param _pose initial for the character handled by this scheduler
     */
    void setCurrentPose(const SMRSkeleton *_pose);

    void enqueueMotionSegment(MotionSegment *_segment);
    void schedule();
    void setClockOffset(const unsigned int _clockOffset);
    void emptyMotion(void){ motion = SMRMotion(); motion.setTimeStep(40.0);}
    void flush(void);

  public:
    Scheduler(string _characterName, SMRSkeleton* const _referencePose);

  protected:
    SMRSkeleton m_bufferedPose;  /**< */
    SMRSkeleton* const m_poseToBeDisplayed;  /**< */
    //SMRSkeleton m_resultPose;  /**< */
    unsigned int m_clockOffset;  /**< */

    bool m_getCurrentPoseWasHere; /**< debug purpose */

  public:
    ~Scheduler();
};
#endif

