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

#ifndef SCHEDULERMANAGER_H
#define SCHEDULERMANAGER_H


#include "Scheduler.h"
#include "logger.h"
#include "SmrUtils.h"

//#include "ActuatorFactory.h"

class SchedulerManager {

public:

  SchedulerManager();
  ~SchedulerManager();

 /**
  * calls the schedule() method on every registered scheduler
  */
  void triggerSchedulers();

  void lock(string agentName);
  void unLock(string agentName);

  /**
   * add a new scheduler
   * (done each time a new agent is added)
   */
  void addScheduler(string _characterName, Scheduler *_scheduler);

  /**
   * get the current pose for one character
   */
  SMRSkeleton *getCharacterPose(const string &_characterName);

  void resetAllSchedulers();

  Scheduler *getScheduler(const string &_characterName);

  public:

   std::map<string, Scheduler*> m_schedulerPointers;

};

#endif
