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

#ifndef _REALIZER_H
#define _REALIZER_H

#include "SocketListener.h"
#include "Parser.h"
#include "SchedulerManager.h"
#include "ActuatorFactory.h"

#include "SmrSkeleton.h"

#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>


 /**
  *  \struct ParsingThreadWrapper
  *..\brief this struct wraps the threaded parse() method of the Parser class 
  */
struct ParsingThreadWrapper
{
  ParsingThreadWrapper(Parser *_parser) : m_parserPointer(_parser) {}
  void operator()()
  {
    m_parserPointer->parse_commands();
  }
  Parser *m_parserPointer;
};

/**
  *  \struct SocketListeningThreadWrapper
  *..\brief this struct wraps the threaded listenSocket() method of the SocketListener class 
 */
struct SocketListeningThreadWrapper
{
  SocketListeningThreadWrapper(SocketListener *_socketListener) : m_socketListenerPointer(_socketListener) {}
  void operator()()
  {
    m_socketListenerPointer->listenSocket();
  }
  SocketListener *m_socketListenerPointer;
};

/**
 *  \struct SchedulingThreadWrapper
 *..\brief this struct wraps the threaded triggerSchedulers() method of the SchedulerManager class 
 */
struct SchedulingThreadWrapper
{
  SchedulingThreadWrapper(SchedulerManager *_schedulerManager) : m_schedulerManager(_schedulerManager) {}
  void operator()()
  {
    m_schedulerManager->triggerSchedulers();
  }
  SchedulerManager *m_schedulerManager;
};

/**
 *  \class Realizer
 *  \brief this class is the entry point to the EMBR realizer, it is in charge of configuring and launching all the required components
 *  - Loggers
 *  - socket listener
 *  - parser
 *  - scheduler manager
 */
class Realizer
{

typedef enum{GLOBALLOGGER,IKLOGGER,SCHEDULERLOGGER,PARSERLOGGER,MODIFIERLOGGER,XMLLOGGER} LOGGER_ID;

public:
 /**
  *..\brief Default constructor
  */
  Realizer();

 /**
  *..\brief Default destructor
  */
  ~Realizer();

 /**
  *..\brief this function launches the realizer's threads (socket listener, scheduler and parser)
  */
  void run();

 /**
  *  \brief this method sets the logging level of a logger
  *  \param _loggerID id of the logger ( cf. typedef enum LOGGER_ID in class Realizer)
  *  \param _level logging level (0 is trace, 3 is debug, 6 is none)
  */
  void configureLogger(int _loggerID, int _level);

 /**
  *  \brief methods which sets up a flag each time a skeleton is requested from a client application (usually written in python)
  *  This method exists mainly for performance evaluation purpose
  */
  void skeletonRequested();

 /**
  *  \brief retrieves a reference towards the actual _character skeleton
  *  \param _character the name of the character whose skeleton needs to be retrieved
  *  \return a reference toward the current skeletal configuration of _character
  */
  SMRSkeleton *getCharacterPose(string _character);

/**
 * \brief change the current feedback message
 * this method update the value of the feedback meesage which is currently used 
 * to bring some information from the control layer to the environment (i.e. the
 * scene graph manager of the 3D engine)
 */
  string getFeedbackMessage(void);

protected:
  Parser *m_parser;
  ActuatorFactory *m_factory;
  SocketListener *m_socketListener;
  SchedulerManager *m_schedulerManager;
};

#endif

