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

#ifndef _PARSER_H
#define _PARSER_H
 
#include "SmrActuator.h"
#include "Scheduler.h"
#include "logger.h"
#include "SchedulerManager.h"
#include "ActuatorFactory.h"
#include "GestureModifier.h"
#include "SmrSmartPointer.h"
#include "SmrUtils.h"

static char alpha[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPGRSTUVWXYZ";

static string xmlc = "0123456789XYZ";

typedef SMRSmartPtr<GestureModifier> GMPtr;

class NUCLEUS
{
public:
  NUCLEUS():nucID(-1),spatialExt(0.0f),temporalExt(0.0f),powerValue(0.0f),fluidityValue(0.0f) {};
  ~NUCLEUS(){};

public:
  int nucID;
  float spatialExt;  
  float temporalExt; 
  float powerValue; 
  float fluidityValue;  

};

class CONSTRAINT
{
public:
  CONSTRAINT(): key(""),
                value(0.0f),
                angle(0.0f),
                type("UNKNOWN"),
                joint(""),
                offset(SMRVector3(0.0,0.0,0.0)),
                target(SMRVector3(0.0,0.0,0.0)),
                direction(SMRVector3(0.0,0.0,0.0)),
                stencil(""),
                morphKey(""),
                normal(""),
                influence(1.0f),
                feedbackID("unlabelled")
  {}
  ~CONSTRAINT(){};
public:
  string key;
  float value;
  float angle;
  string type;
  string joint;
  SMRVector3 offset;
  SMRVector3 target;
  SMRVector3 direction;
  string stencil;
  string morphKey;
  string normal;
  float influence;
  string feedbackID;
};

class GESTURE_COMPONENT
{
public:
  GESTURE_COMPONENT():key(""),type(""),absoluteTimePointInMs(0),holdDuration(0),nucID(-1),id(0){};
  ~GESTURE_COMPONENT(){};

public:
  string key;
  string type;
  unsigned int absoluteTimePointInMs;
  unsigned int holdDuration;
  vector<CONSTRAINT> constraints;
  int nucID;
  string animationClip;
  string stencil;
  MotionSegment* keyFrame;
  int id;
};


class GESTURE
{
public:
  GESTURE():characterName(""),
			key(""),
			absoluteStartMs(0),
			absoluteStopMs(0),
			fadeInMS(0),
      fadeOutMS(0),
			timeWarpSym("sym"),
			timeWarpStipness(1.0f),
      modTime(0)
			{};
  ~GESTURE(){};
public:
  string characterName;
  string key;
  unsigned int absoluteStartMs;
  unsigned int absoluteStopMs;
  unsigned int fadeInMS;
  unsigned int fadeOutMS;
  string timeWarpSym;
  float timeWarpStipness;
  vector<GESTURE_COMPONENT> components;
  int modTime;
  //MotionSequence* motionSequence;
};

/**
 * This class is in charge of  parsing a gesture description
 * chunk and to call appropriate actuators using it's associated
 * ActuatorFactory
 */
class Parser {
public:

  Parser();

  ~Parser();

  /**
   * \brief The big ugly command which parses the EMBRScript
   */
  void parse_commands();
  
 // void parse_commandXML(string &_command);

  /**
   * \brief add a new command line to the command queue (called by the socket listener)
   */
  void enqueueCommand(const string &_command);

  void setSchedulerManager(SchedulerManager *_schedulerManager);

  //XML parsing functionality
  void parse_commandLineXML( stringstream &_commandLine, std::vector<string> &_parsedElements, GESTURE &_currentGesture, GESTURE_COMPONENT &_currentComponent, CONSTRAINT &_currentConstraint, NUCLEUS &_currentNucleus, unsigned int &_timeOffset, int &_lastStopTime,string &_currentElement);

  void closeElementXML(stringstream &_commandLine, std::vector<string> &_parsedElements, string &_currentToken, GESTURE &_currentGesture, GESTURE_COMPONENT &_currentComponent, CONSTRAINT &_currentConstraint, NUCLEUS &_currentNucleus, int &_lastStopTime);

  void parseAttribute(stringstream &_commandLine, std::vector<string> &_parsedElements, string &_currentToken, GESTURE &_currentGesture, GESTURE_COMPONENT &_currentComponent, CONSTRAINT &_currentConstraint, NUCLEUS &_currentNucleus, unsigned int &_timeOffset, int &_lastStopTime);

  void parseElement(stringstream &_commandLine, std::vector<string> &_parsedElements, string &_currentToken, GESTURE &_currentGesture, GESTURE_COMPONENT &_currentComponent, CONSTRAINT &_currentConstraint, NUCLEUS &_currentNucleus,string &_currentElement);
  
  void parseShortElement(stringstream &_commandLine, std::vector<string> &_parsedElements, string &_currentToken, GESTURE &_currentGesture, GESTURE_COMPONENT &_currentComponent, CONSTRAINT &_currentConstraint, NUCLEUS &_currentNucleus,string &_currentElement);

  // old parsing functionality
  void parse_commandLine( stringstream &_commandLine, std::vector<string> &_parsedElements, GESTURE &_currentGesture, GESTURE_COMPONENT &_currentComponent, CONSTRAINT &_currentConstraint, NUCLEUS &_currentNucleus, unsigned int &_timeOffset, int &_lastStopTime);

  void parseParameters(stringstream &_commandLine, std::vector<string> &_parsedElements, string &_currentToken, GESTURE &_currentGesture, GESTURE_COMPONENT &_currentComponent, CONSTRAINT &_currentConstraint, NUCLEUS &_currentNucleus, unsigned int &_timeOffset, int &_lastStopTime);
  
  void closeElement(stringstream &_commandLine, std::vector<string> &_parsedElements, string &_currentToken, GESTURE &_currentGesture, GESTURE_COMPONENT &_currentComponent, CONSTRAINT &_currentConstraint, NUCLEUS &_currentNucleus, int &_lastStopTime);

  void openElement(stringstream &_commandLine, std::vector<string> &_parsedElements, string &_currentToken, GESTURE &_currentGesture, GESTURE_COMPONENT &_currentComponent, CONSTRAINT &_currentConstraint, NUCLEUS &_currentNucleus);
private:
  std::vector<string> m_embotCommands;
  std::map<int, GMPtr *> m_gestureModifiers;
  SchedulerManager *m_schedulerManager;
  int comment;
};

#endif

