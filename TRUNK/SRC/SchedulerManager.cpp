
#include "SchedulerManager.h"

SchedulerManager::SchedulerManager()
{
}

SchedulerManager::~SchedulerManager()
{
  //boost::mutex::scoped_lock lock(schedulerMutex);
  std::map<string,Scheduler*>::iterator mapIterator;
  for ( mapIterator = m_schedulerPointers.begin(); mapIterator != m_schedulerPointers.end(); ++ mapIterator)
  {
    delete mapIterator->second;
  }
}

//void SchedulerManager::lock(string _character)
//{
//  Scheduler* relevantScheduler = getScheduler(_character);
//  relevantScheduler->lock(EXTERNAL);
//}

//void SchedulerManager::unLock(string _character)
//{
//  Scheduler* relevantScheduler = getScheduler(_character);
//  relevantScheduler->unLock(EXTERNAL);
//}

void SchedulerManager::triggerSchedulers()
{
  for(;;)
  {
    std::map<string,Scheduler*>::iterator schedulerIterator;
    
    for (schedulerIterator = m_schedulerPointers.begin(); schedulerIterator != m_schedulerPointers.end(); schedulerIterator++)
    {
      (schedulerIterator->second)->schedule();
    }
      boost::this_thread::sleep(boost::posix_time::milliseconds(30));
  }
}

SMRSkeleton *SchedulerManager::getCharacterPose(const string &_characterName)
{
  Scheduler* relevantScheduler = getScheduler(_characterName);
  return relevantScheduler->getCurrentPose();
}

void SchedulerManager::addScheduler(string _characterName, Scheduler *_scheduler)
{
  m_schedulerPointers.insert(pair<string,Scheduler*>(_characterName, _scheduler));
}


void SchedulerManager::resetAllSchedulers()
{
  boost::mutex::scoped_lock lock(schedulerMutex);
  if (m_schedulerPointers.size() == 0)
  {
    //cout << "Creating a new scheduler for Character Alfonse" << currentGesture.characterName << endl;
    //Scheduler *newScheduler = new Scheduler();
    //newScheduler->setCurrentPose(ActuatorFactory::getInstance()->getCharacterPosture("Alfonse"));
    //newScheduler->setReferencePose(ActuatorFactory::getInstance()->getCharacterPosture("Alfonse"));
    //m_schedulerPointers.insert( pair<string,Scheduler*>("Alfonse",newScheduler) );
  }
  //reset all timers
  std::map<string,Scheduler*>::iterator schedulerIterator;
  for (schedulerIterator = m_schedulerPointers.begin(); schedulerIterator != m_schedulerPointers.end(); schedulerIterator++)
  {
/*  timeval timeVal;
    gettimeofday(&timeVal, NULL);
    unsigned int timeOffset = (timeVal.tv_sec * 1000.0) + (timeVal.tv_usec / 1000.0);
*/
    unsigned int timeOffset = SMRUtils::getCurrentTime();
    (schedulerIterator->second)->setClockOffset(timeOffset);
    (schedulerIterator->second)->flush();
    (schedulerIterator->second)->emptyMotion();
  }
}

Scheduler *SchedulerManager::getScheduler(const string &_characterName)
{
  //first, check if relevant scheduler exists in the map. If not, create it
  if (m_schedulerPointers.find(_characterName) == m_schedulerPointers.end())
  {
    if (_characterName == "")
    {
      LOG_FATAL(parserLogger,"this gesture sequence is not binded to any character, maybe you forgot a \"CHARACTER\" parameter when specifying a K_POSE_SEQUENCE" );
    }else
    {
      LOG_FATAL(parserLogger,"No scheduler corresponding to character " + _characterName + " has been regitered yet !" );
    }
    exit(1);
  }
  Scheduler *relevantScheduler = m_schedulerPointers[_characterName];
  return relevantScheduler;
}
