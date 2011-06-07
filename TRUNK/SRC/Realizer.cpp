
#include "Realizer.h"

Realizer::Realizer()
{
  //logger.setLogLevel(TRACE_LOG_LEVEL);
  logger.setLogLevel(DEBUG_LOG_LEVEL);
  //logger.setLogLevel(INFO_LOG_LEVEL);
  //logger.setLogLevel(WARN_LOG_LEVEL);
  //logger.setLogLevel(FATAL_LOG_LEVEL);
  //logger.setLogLevel(FATAL_LOG_LEVEL);

  modifierLogger.setLogLevel(ERROR_LOG_LEVEL);

  ikLogger.setLogLevel(FATAL_LOG_LEVEL);
  xmlLogger.setLogLevel(ERROR_LOG_LEVEL);

  //configure the logger
  config.configure();

  m_schedulerManager = new SchedulerManager;

  //create the parser instance
  m_parser = new Parser;
  m_parser->setSchedulerManager(m_schedulerManager);
  //create the Listener Socket Instance
  m_socketListener = new SocketListener;
  m_socketListener->setParser(m_parser);
  //create the factory instance
  m_factory = ActuatorFactory::getInstance();
  //m_factory->setParser(m_parser);
  m_factory->setSchedulerManager(m_schedulerManager);


};

Realizer::~Realizer()
{
  delete m_socketListener;
  delete m_parser;
  delete m_schedulerManager;
};

SMRSkeleton *Realizer::getCharacterPose(string _character)
{
  return m_schedulerManager->getCharacterPose(_character);
  //return NULL;
}

void Realizer::run()
{
  //launch the socketlistener thread
  //boost::thread thrd1(ReadCommandsFromFileThreadWrapper( boost::ref(socketListenerInstance) )); // boost::ref for non copyiable objects
  boost::thread thrd1(SocketListeningThreadWrapper( boost::ref(m_socketListener) )); // boost::ref for non copyiable objects

  //boost::thread thrd1(cocount(parserInstance));
  boost::thread thrd2(ParsingThreadWrapper( boost::ref(m_parser) )); // boost::ref for non copyiable objects

  //launch the scheduling thread
  boost::thread thrd3(SchedulingThreadWrapper( boost::ref(m_schedulerManager) )); // boost::ref for non copyiable objects

  //wait for parser thread to stop
  thrd2.join();
  LOG_INFO(parserLogger,"The parser thread finished lopping");
  //wait for socketlistener thread to stop
  thrd1.join();
  //wait for socketlistener thread to stop
  thrd3.join();
}

void Realizer::configureLogger(int _loggerID, int _level)
{
  if (_loggerID == Realizer::XMLLOGGER)
  {
    xmlLogger.setLogLevel(_level);    
  }
  if (_loggerID == Realizer::MODIFIERLOGGER)
  {
    modifierLogger.setLogLevel(_level);    
  }
  if (_loggerID == Realizer::PARSERLOGGER)
  {
    parserLogger.setLogLevel(_level);    
  }
  if (_loggerID == Realizer::SCHEDULERLOGGER)
  {
    schedulerLogger.setLogLevel(_level);    
  }
  else if (_loggerID == Realizer::IKLOGGER)
  {
    ikLogger.setLogLevel(_level);
  }else if (_loggerID == Realizer::GLOBALLOGGER)
  {
    logger.setLogLevel(_level);
  }else
  {
    LOG_ERROR(logger,"the required logger " << _loggerID << " doesn't exist");
  }
}

void Realizer::skeletonRequested()
{
  //Scheduler* relevantScheduler = m_schedulerManager->getScheduler("Alfonse");
  //relevantScheduler->skeletonRequested();
}

string Realizer::getFeedbackMessage(void)
{
  return m_factory->getFeedbackMessage();
}