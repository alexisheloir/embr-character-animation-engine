//because I've got to commit
#include "Parser.h"
 
float tokenize(string &_inputString, string _separator)
{
  string token;
  float result;
  size_t pos = _inputString.find(_separator,0);
  token = _inputString.substr(0,pos);
  if (pos != string::npos)
  {
    _inputString.erase(0,pos+1);
  }
  stringstream ss(token);
  ss >> result;
  return result;
}

string stringTokenize(string &_inputString, string _separator)
{
  string token;
  size_t pos = _inputString.find(_separator,0);
  if (pos != string::npos)
  {
    token = _inputString.substr(0,pos);
    _inputString.erase(0,pos+1);
  }else
  {
    token = _inputString;
    _inputString.clear();
  }
  return token;
}

string removeComments(string &_inputString)
{
  string token;
  size_t pos = _inputString.find('#',0);
  if (pos != string::npos)
  {  
    token = _inputString.substr(0,pos);
    size_t k = token.find_last_not_of(" ");
    token = token.substr(0, k+1);
    _inputString.erase(k+1,_inputString.size());
  }else
  {
    token = _inputString;
  }
  return token;

}

  void Parser::setSchedulerManager(SchedulerManager *_schedulerManager)
  {
    m_schedulerManager = _schedulerManager;
  }



// Singleton initialization.  At first, there is no object created.
//Parser *Parser::m_singleton = NULL;

Parser::Parser():m_schedulerManager(NULL)
{
}

Parser::~Parser() {
}

void Parser::enqueueCommand(const string &_command)
{
  string TIME_RESET = _command;
  // chunk the command line per line
  while(TIME_RESET.length() > 0)
  {
    boost::mutex::scoped_lock lock(parserMutex);
    string line = stringTokenize(TIME_RESET,"\n");
    LOG_TRACE(parserLogger,"Current command to enqueue is " <<line<< endl);
    line = removeComments(line);
    m_embotCommands.push_back(line);
  }
}

void Parser::parse_commands(void)
{ 
  stringstream commandLine;
  std::vector<string> parsedElements;

  GESTURE currentGesture;
  GESTURE_COMPONENT currentComponent;
  CONSTRAINT currentConstraint;
  NUCLEUS currentNucleus;

  unsigned int timeOffset = 0;
  int lastStopTime = 0;
  
  while(1)
  {
    LOG_TRACE(parserLogger,"parser is looping" << endl);
    if (m_embotCommands.size() > 0)
    {
      boost::mutex::scoped_lock lock(parserMutex);
      commandLine.str((*(m_embotCommands.begin())).c_str()) ;
      m_embotCommands.erase(m_embotCommands.begin());
      parse_commandLine(commandLine, parsedElements, currentGesture, currentComponent, currentConstraint, currentNucleus, timeOffset, lastStopTime);
    }else
    {
      LOG_TRACE(parserLogger,"no command in heap, sleeping" << endl);
#ifdef WIN32
      Sleep(1);
#else
      usleep(1);
#endif
    }
  }
}

void Parser::parse_commandLine( stringstream &_commandLine, std::vector<string> &_parsedElements, GESTURE &_currentGesture, GESTURE_COMPONENT &_currentComponent, CONSTRAINT &_currentConstraint, NUCLEUS &_currentNucleus, unsigned int &_timeOffset, int &_lastStopTime   )
{
  if (_commandLine.str().length() > 0 && _commandLine.str().find(" #") == string::npos && _commandLine.str().find("\t#") == string::npos && _commandLine.str()[0] != '#')
  {
    string currentToken = "";
    string prevToken = "";
    _commandLine >> currentToken;
    while(prevToken != currentToken) 
    {
      if (currentToken == "TIME_RESET")
      {
        m_schedulerManager->resetAllSchedulers();
      /*  timeval currentTime;
        gettimeofday(&currentTime,NULL);
        _timeOffset = (unsigned int) ((currentTime.tv_sec * 1000.0) + (currentTime.tv_usec / 1000.0));
        */
        _timeOffset = SMRUtils::getCurrentTime();
        //clear nucleus map
        m_gestureModifiers.clear();
      }else if (currentToken == "BEGIN") // this is the begin a a new command/constraint set
      {
        openElement(_commandLine, _parsedElements, currentToken, _currentGesture, _currentComponent, _currentConstraint, _currentNucleus);
      }else if (currentToken == "END"  && _parsedElements.size() > 0 )
      {
        closeElement(_commandLine, _parsedElements, currentToken, _currentGesture, _currentComponent, _currentConstraint, _currentNucleus, _lastStopTime);
      }else if (currentToken.find(":") != string::npos)// && _parsedElements.size() > 0 )   // this is a parameter
      {
        parseParameters(_commandLine, _parsedElements, currentToken, _currentGesture, _currentComponent, _currentConstraint, _currentNucleus, _timeOffset, _lastStopTime);
      }else
      {
        LOG_ERROR(parserLogger,"syntax error: " << currentToken << " is unknown");
      }
      prevToken = currentToken;
      _commandLine >> currentToken;
    }
    _commandLine.seekp(ios_base::beg);
    _commandLine.seekg(ios_base::beg);
    _commandLine.str("");
    _commandLine.clear();
  }
}

void Parser::openElement(stringstream &_commandLine, std::vector<string> &_parsedElements, string &_currentToken, GESTURE &_currentGesture, GESTURE_COMPONENT &_currentComponent, CONSTRAINT &_currentConstraint, NUCLEUS &_currentNucleus)
{

  _commandLine >> _currentToken;
  if (_currentToken == "K_POSE_SEQUENCE")
  {
    if (_parsedElements.size()>0 && _parsedElements.back() != "END")
    {
      LOG_ERROR(parserLogger,"error : a K_POSE_SEQUENCE element cannot be nested into another ELEMENT" << endl);
      //exit(1);
    }else
    {
      _currentGesture = GESTURE();
      _parsedElements.push_back("K_POSE_SEQUENCE");
    }
  }else if (_currentToken == "K_POSE" || _currentToken == "ANIMATION")
  {
    _currentComponent = GESTURE_COMPONENT();
    Character* character = ActuatorFactory::getInstance()->getCharacter(_currentGesture.characterName);
    _currentComponent.keyFrame = new MotionSegment(character);
    _currentComponent.type = _currentToken;
    _currentComponent.nucID = -1; //papo
    _parsedElements.push_back("GESTURE_COMPONENT");
  }else if (_currentToken == "SWIVEL_CONSTRAINT" || _currentToken == "POSITION_CONSTRAINT" || _currentToken == "ORIENTATION_CONSTRAINT" || _currentToken == "LOOK_AT_CONSTRAINT" || _currentToken == "MORPH_TARGET" || _currentToken == "SHADER" || _currentToken == "AUTONOMOUS_BEHAVIOR" || _currentToken == "POSE_TARGET")
  {
    _currentConstraint = CONSTRAINT();
    _currentConstraint.type = _currentToken;
    _parsedElements.push_back("CONSTRAINT");
  }else if (_currentToken == "NUCLEUS")
  { 
    if (_parsedElements.size() > 0 && _parsedElements.back() != "END")
    {
      LOG_ERROR(parserLogger,"error : a NUCLEUS element cannot be nested into another ELEMENT" << endl);
      //exit(1);
    }else 
    { 
      _currentNucleus = NUCLEUS();
      _parsedElements.push_back("GESTURE_NUCLEUS");
    }
  }
  else
  {
    //it is a comment or an unknown element
  }
}

void Parser::closeElement(stringstream &_commandLine, std::vector<string> &_parsedElements, string &_currentToken, GESTURE &_currentGesture, GESTURE_COMPONENT &_currentComponent, CONSTRAINT &_currentConstraint, NUCLEUS &_currentNucleus, int &_lastStopTime)
{
  if(_parsedElements.back() == "K_POSE_SEQUENCE")
  {
    //currentGesture = GESTURE();
    //go through every component of the gesture, extract the constraints, set up and post the actuators.
    std::vector<GESTURE_COMPONENT>::iterator componentIterator;
    std::vector<GESTURE_COMPONENT> *components;
    components = &_currentGesture.components;
    //ActuatorIKSlerp *currentActuator = NULL;
    for (componentIterator = components->begin(); componentIterator < components->end(); componentIterator++)
    {
      GESTURE_COMPONENT currentComponent = *componentIterator;
    }
    float sequenceAbsoluteStartTime = (float)_currentGesture.absoluteStartMs;
    float sequenceAbsoluteStopTime = (float)_currentGesture.absoluteStopMs;
    float relativeFadeIn = (float)_currentGesture.fadeInMS / (float)(sequenceAbsoluteStopTime - sequenceAbsoluteStartTime);
    float relativeFadeOut = 1.0f - ((float)_currentGesture.fadeOutMS / (float)(sequenceAbsoluteStopTime - sequenceAbsoluteStartTime));
    //gesturesHeap.push_back(_currentGesture);
    //_currentGesture = GESTURE();
    string timeWarpSym = _currentGesture.timeWarpSym;
    float timeWarpStipness = _currentGesture.timeWarpStipness;
    ActuatorFactory::getInstance()->combinePoseSequence(_currentGesture.characterName,relativeFadeIn,relativeFadeOut, timeWarpSym, timeWarpStipness);
  }
  else if(_parsedElements.back() == "GESTURE_COMPONENT")
  {
    Scheduler *relevantScheduler = m_schedulerManager->getScheduler(_currentGesture.characterName);

    if (_currentComponent.type == "ANIMATION")
    {
      int segmentAbsoluteStartTime = 0;
      int segmentAbsoluteStopTime = _currentComponent.absoluteTimePointInMs;
      segmentAbsoluteStartTime = _lastStopTime;
      float relativeFadeIn = (float)_currentGesture.fadeInMS / (float)(segmentAbsoluteStopTime - segmentAbsoluteStartTime);
      float relativeFadeOut = (float)_currentGesture.fadeOutMS / (float)(segmentAbsoluteStopTime - segmentAbsoluteStartTime);

      ActuatorFactory::getInstance()->setupAndPostAnimationSegment(relevantScheduler,
        segmentAbsoluteStartTime,
        segmentAbsoluteStopTime,
        _currentComponent.holdDuration,
        _currentComponent.stencil,
        _currentComponent.animationClip,
        relativeFadeIn,
        relativeFadeOut
        //TODO: add feedback mechanism for animations
        );
      ActuatorFactory::getInstance()->sendMotionSegments(relevantScheduler);
    }
    else if (_currentComponent.type == "K_POSE") // this is a pose, build first posture
    {
      //get the position constraint (at this state of advancement, this should be the only one)
      std::vector<CONSTRAINT>::iterator constraintIterator;
      std::vector<CONSTRAINT> *constraints;
      constraints = &_currentComponent.constraints;
      int segmentAbsoluteStartTime = _lastStopTime;
      int segmentAbsoluteStopTime = _currentComponent.absoluteTimePointInMs;
      if (segmentAbsoluteStartTime == segmentAbsoluteStopTime)
      {
        segmentAbsoluteStopTime += 40; // in case of overlap, delay stop time a little bit.
      }
      if (segmentAbsoluteStartTime > segmentAbsoluteStopTime)
      {
        segmentAbsoluteStartTime = segmentAbsoluteStopTime-40; // in case of overlap, delay stop time a little bit.
      }
      for (constraintIterator = constraints->begin(); constraintIterator < constraints->end(); constraintIterator++) // may occur if key frames are not ordered correctly...
      {
        CONSTRAINT constraint = * constraintIterator;
        if (constraint.type == "MORPH_TARGET")
        {
          //float relativeFadeIn = 0.0f;
          //float relativeFadeOut = 0.0f;

          LOG_INFO(parserLogger, "Posting MORPH_TARGET, key is " << constraint.morphKey << " value is: " << constraint.value << " start time is: " << segmentAbsoluteStartTime << " stop time is: " << segmentAbsoluteStopTime);
          ActuatorFactory::getInstance()->setupAndPostMorphTargetSegment(relevantScheduler,
            segmentAbsoluteStartTime,
            segmentAbsoluteStopTime,
            _currentComponent.holdDuration,
            constraint.morphKey,
            constraint.value
            );
        }
        else if (constraint.type == "SHADER")
        {
          //float relativeFadeIn = 0.0f;
          //float relativeFadeOut = 0.0f;

          ActuatorFactory::getInstance()->setupAndPostShaderParameterSegment(relevantScheduler,
            segmentAbsoluteStartTime,
            segmentAbsoluteStopTime,
            _currentComponent.holdDuration,
            constraint.morphKey,
            constraint.value
            );
        }
        else if (constraint.type == "AUTONOMOUS_BEHAVIOR")
        {

          ActuatorFactory::getInstance()->setupAndPostAutonomousBehaviorParameterSegment(relevantScheduler,
            segmentAbsoluteStartTime,
            segmentAbsoluteStopTime,
            constraint.morphKey,
            constraint.value
            );
        }
        else if (constraint.type == "POSE_TARGET")
        {
          //float relativeFadeIn = 0.0f;
          //float relativeFadeOut = 0.0f;

          ActuatorFactory::getInstance()->setupAndPostStoredPoseSegment(relevantScheduler,
            segmentAbsoluteStartTime,
            segmentAbsoluteStopTime,
            _currentComponent.holdDuration,
            constraint.stencil,
            constraint.key,
            constraint.influence
            );
        }
        else if (constraint.type == "POSITION_CONSTRAINT")
        {

          //<PP> gesture modifier assignment
          SMRSmartPtr<GestureModifier> *gestMod(NULL);
          //Check if a nucleus is assigned to this K-Pose
          if (_currentComponent.nucID != -1)
          {
            //get reference to the specific GestureModifier(GM)
            LOG_INFO(parserLogger,"Nucleus to use is:" <<_currentComponent.nucID);
            map<int,GMPtr*>::iterator gestModIt; 
            gestModIt = m_gestureModifiers.find(_currentComponent.nucID);
            if (gestModIt == m_gestureModifiers.end())
            {
              LOG_FATAL(parserLogger,"nucleus description for ID "<<_currentComponent.nucID<< " not found!");
            }
            else
            { 
              //create a reference to the gm
              gestMod = new SMRSmartPtr<GestureModifier>(*(gestModIt->second));
              LOG_INFO(parserLogger,"modifying target!!");
            }
          }
          //end <PP>
          stringstream target("");
          target << constraint.target.X() << " ";
          target << constraint.target.Y() << " ";
          target << constraint.target.Z();
          stringstream offset("");
          offset << constraint.offset.X() << " ";
          offset << constraint.offset.Y() << " ";
          offset << constraint.offset.Z();
          string target_position = target.str();

          //float relativeFadeIn = 0.0f;
          //float relativeFadeOut = 0.0f;

          bool feedbackRequired = false;
          if (constraint.feedbackID != "unlabelled")
          {
            feedbackRequired = true;
          }

          LOG_INFO(parserLogger,"posting position constraint, start= " << segmentAbsoluteStartTime << " stop= " << segmentAbsoluteStopTime);
          ActuatorFactory::getInstance()->setupAndPostMotionSegment(relevantScheduler,
            segmentAbsoluteStartTime,
            segmentAbsoluteStopTime,
            _currentComponent.holdDuration,
            "positionConstraint",
            constraint.stencil,
            target.str(),
            constraint.joint,
            offset.str(),
            gestMod,
            feedbackRequired,
            constraint.feedbackID
            );
          LOG_DEBUG(parserLogger,"segment posted");
        }else if (constraint.type == "ORIENTATION_CONSTRAINT")
        {
          stringstream direction("");
          direction << constraint.direction.X() << " ";
          direction << constraint.direction.Y() << " ";
          direction << constraint.direction.Z();
          string target_position = direction.str();
          LOG_DEBUG(parserLogger,"posting orientation constraint, start= " << segmentAbsoluteStartTime << " stop= " << segmentAbsoluteStopTime);
          ActuatorFactory::getInstance()->setupAndPostMotionSegment(relevantScheduler,
            segmentAbsoluteStartTime,
            segmentAbsoluteStopTime,
            _currentComponent.holdDuration,
            "orientationConstraint",
            constraint.stencil,
            direction.str(),
            constraint.joint,
            constraint.normal
            );
          LOG_DEBUG(parserLogger,"segment posted");
        }else if (constraint.type == "SWIVEL_CONSTRAINT")
        {
          stringstream _angle;
          _angle << constraint.angle;
          LOG_DEBUG(parserLogger,"posting swivel constraint, start= " << segmentAbsoluteStartTime << " stop= " << segmentAbsoluteStopTime);
          ActuatorFactory::getInstance()->setupAndPostMotionSegment(relevantScheduler,
            segmentAbsoluteStartTime,
            segmentAbsoluteStopTime,
            0,//_currentComponent.holdDuration,
            "swivelConstraint",
            constraint.stencil,
            _angle.str(),
            constraint.joint,
            constraint.normal
            );
          LOG_DEBUG(parserLogger,"segment posted");
        }else if (constraint.type == "LOOK_AT_CONSTRAINT")
        {
          stringstream target("");
          target << constraint.target.X() << " ";
          target << constraint.target.Y() << " ";
          target << constraint.target.Z();
          string target_position = target.str();
          LOG_DEBUG(parserLogger,"posting look_at constraint, start= " << segmentAbsoluteStartTime << " stop= " << segmentAbsoluteStopTime);
          ActuatorFactory::getInstance()->setupAndPostMotionSegment(relevantScheduler,
            segmentAbsoluteStartTime,
            segmentAbsoluteStopTime,
            _currentComponent.holdDuration,
            "lookAtConstraint",
            constraint.stencil,
            target.str()
            );
          LOG_DEBUG(parserLogger,"segment posted");
        }else
        {
          LOG_ERROR(parserLogger, constraint.type << " : THIS CONSTRAINT IS UNKNOWN");
        }

      }
      constraints->clear();
      ActuatorFactory::getInstance()->sendMotionSegments(relevantScheduler);
      _lastStopTime = segmentAbsoluteStopTime + _currentComponent.holdDuration;
    }
    //store the GESTURE_COMPONENT into GESTURE's component vector
    _currentGesture.components.push_back(_currentComponent);
    _currentComponent = GESTURE_COMPONENT();
  }
  else if(_parsedElements.back() == "CONSTRAINT")
  {
    _currentComponent.constraints.push_back(_currentConstraint);
    _currentConstraint = CONSTRAINT();
  }
  // <PP> new: end of a nucleus tag found, so set parameters.
  else if(_parsedElements.back() == "GESTURE_NUCLEUS")
  {
    if (_currentNucleus.nucID == -1)
    { 
      LOG_ERROR(parserLogger,"No ID assigned to the nucleus!");
    } 
    else{
      GestureModifier *GM(new GestureModifier(_currentNucleus.spatialExt,_currentNucleus.temporalExt,_currentNucleus.powerValue,_currentNucleus.fluidityValue));
      SMRSmartPtr<GestureModifier> * currentGM = (new SMRSmartPtr<GestureModifier>(GM));
      m_gestureModifiers.insert(pair<int,SMRSmartPtr<GestureModifier> *>(_currentNucleus.nucID,currentGM));
    }
  }
  //end<PP>
  else
  {
    LOG_ERROR(parserLogger,"syntax error : this end element does not close anything");
    //exit(1);
  }        
  _parsedElements.pop_back();
}

void Parser::parseParameters(stringstream &_commandLine, std::vector<string> &_parsedElements, string &_currentToken, GESTURE &_currentGesture, GESTURE_COMPONENT &_currentComponent, CONSTRAINT &_currentConstraint, NUCLEUS &_currentNucleus, unsigned int &_timeOffset, int &_lastStopTime)
{
  string::size_type idx = _currentToken.find(':');
  string parameterKey = _currentToken.substr(0, idx);
  string parameterValue = _currentToken.substr(idx+1);
  if (parameterKey == "CHARACTER")
  {
    if (_parsedElements.back() == "K_POSE_SEQUENCE")
    {
      stringstream ss(parameterValue);
      string characterName;
      ss >> characterName;
      Character* character = ActuatorFactory::getInstance()->getCharacter(characterName);
      //_currentGesture.motionSequence = new MotionSequence(character);
      _currentGesture.characterName = characterName;
    }else
    {
      LOG_ERROR(parserLogger,"CHARACTER parameter can only be nested in a K_POSE_SEQUENCE element");
    }
  }else if (parameterKey == "START")
  {
    if (_parsedElements.back() == "K_POSE_SEQUENCE")
    {
      stringstream ss(parameterValue);
      string startInfo;
      ss >> startInfo;
      if (startInfo == "asap")
      {/*
        timeval currentTime;
        gettimeofday(&currentTime,NULL);
        unsigned int clock = (unsigned int) ((currentTime.tv_sec * 1000.0) + (currentTime.tv_usec / 1000.0)); */
       unsigned int clock = SMRUtils::getCurrentTime();
        _currentGesture.absoluteStartMs = clock -_timeOffset;
        //_currentGesture.motionSequence->setAbsoluteStartTime(clock - _timeOffset); // Refactoring
        _lastStopTime = _currentGesture.absoluteStartMs;
      }
      else if (startInfo[0] == '+')
      {
        startInfo.erase(0,1);
        stringstream ss1(startInfo);
        int time;
        ss1 >> time;
        /*
        timeval currentTime;
        gettimeofday(&currentTime,NULL);
        unsigned int clock = (unsigned int) ((currentTime.tv_sec * 1000.0) + (currentTime.tv_usec / 1000.0)); */
        unsigned int clock = SMRUtils::getCurrentTime();
        _currentGesture.absoluteStartMs = clock-_timeOffset+time ;
        //_currentGesture.motionSequence->setAbsoluteStartTime(clock - _timeOffset+time);  // Refactoring
        _lastStopTime = _currentGesture.absoluteStartMs;
      }
      else
      {
        stringstream ss1(parameterValue);
        int time;
        ss1 >> time;
        _currentGesture.absoluteStartMs = time;
        //_currentGesture.motionSequence->setAbsoluteStartTime(time); // Refactoring
        _lastStopTime = _currentGesture.absoluteStartMs;
      }
    }else
    {
      LOG_ERROR(parserLogger,"START parameter can only be nested in a K_POSE_SEQUENCE element");
    }
  }else if (parameterKey == "TIME_POINT")
  {
    if (_parsedElements.back() == "GESTURE_COMPONENT")
    {
      stringstream ss(parameterValue);
      string value;
      ss >> value;
      if (value[0] == '+')
      {
        value.erase(0,1);
        stringstream ss1(value);
        int timeOffset;
        ss1 >> timeOffset;
        _currentComponent.absoluteTimePointInMs = _currentGesture.absoluteStartMs + timeOffset ;
        _currentComponent.keyFrame->setAbsoluteStartTime(_currentGesture.absoluteStartMs + timeOffset);
        _currentGesture.absoluteStopMs = _currentGesture.absoluteStartMs + timeOffset;
        //_currentGesture.motionSequence->setAbsoluteStopTime(_currentGesture.absoluteStartMs + timeOffset);
      }else
      {
        stringstream ss1(parameterValue);
        int time;
        ss1 >> time;
        _currentComponent.absoluteTimePointInMs = time;
        _currentComponent.keyFrame->setAbsoluteStartTime(time);
        _currentGesture.absoluteStopMs = time;
        //_currentGesture.motionSequence->setAbsoluteStopTime(time);
      }
    }else
    {
      LOG_ERROR(parserLogger,"REL_START parameter can only be nested in a K_POSE element");
    }
  }else if (parameterKey == "HOLD")
  {
    if (_parsedElements.back() == "GESTURE_COMPONENT")
    {
      stringstream ss(parameterValue);
      ss >> _currentComponent.holdDuration;
    }else
    {
      LOG_ERROR(parserLogger,"HOLD parameter can only be nested in a K_POSE element");
    }
  }
  else if (parameterKey == "NUCLEUS")
  {
    if (_parsedElements.back() == "GESTURE_COMPONENT")
    {
      stringstream ss(parameterValue);
      ss >> _currentComponent.nucID;
      //compute time to start with next motion
      _currentGesture.modTime = _currentComponent.absoluteTimePointInMs + _currentComponent.holdDuration;
    }else
    {
      LOG_ERROR(parserLogger,"NUCLEUS can only be assigned to a K_POSE element");
    }
  }else if (parameterKey == "MORPH_KEY")
  {
    if (_parsedElements.back() == "CONSTRAINT" && _currentConstraint.type == "MORPH_TARGET")
    {
      _currentConstraint.morphKey = parameterValue;
    }else
    {
      LOG_ERROR(parserLogger,"MORPH_KEY parameter can only be nested in a MORPH_TARGET element");
    }
  }else if (parameterKey == "MORPH_VALUE")
  {
    if (_parsedElements.back() == "CONSTRAINT" && _currentConstraint.type == "MORPH_TARGET")
    {
      stringstream ss(parameterValue);
      ss >> _currentConstraint.value;
    }else
    {
      LOG_ERROR(parserLogger,"MORPH_VALUE parameter can only be nested in a MORPH_TARGET element");
    }
  }else if (parameterKey == "SHADER_KEY")
  {
    if (_parsedElements.back() == "CONSTRAINT" && _currentConstraint.type == "SHADER")
    {
      _currentConstraint.morphKey = parameterValue;
    }else
    {
      LOG_ERROR(parserLogger,"SHADER_KEY parameter can only be nested in a SHADER element");
    }
  }else if (parameterKey == "SHADER_VALUE")
  {
    if (_parsedElements.back() == "CONSTRAINT" && _currentConstraint.type == "SHADER")
    {
      stringstream ss(parameterValue);
      ss >> _currentConstraint.value;
    }else
    {
      LOG_ERROR(parserLogger,"SHADER_VALUE parameter can only be nested in a SHADER element");
    }
  }else if (parameterKey == "BEHAVIOR_KEY")
  {
    if (_parsedElements.back() == "CONSTRAINT" && _currentConstraint.type == "AUTONOMOUS_BEHAVIOR")
    {
      _currentConstraint.morphKey = parameterValue;
    }else
    {
      LOG_ERROR(parserLogger,"SHADER_KEY parameter can only be nested in a SHADER element");
    }
  }else if (parameterKey == "BEHAVIOR_VALUE")
  {
    if (_parsedElements.back() == "CONSTRAINT" && _currentConstraint.type == "AUTONOMOUS_BEHAVIOR")
    {
      stringstream ss(parameterValue);
      ss >> _currentConstraint.value;
    }else
    {
      LOG_ERROR(parserLogger,"SHADER_VALUE parameter can only be nested in a SHADER element");
    }
  }else if (parameterKey == "FADE_IN")
  {
    if (_parsedElements.back() == "K_POSE_SEQUENCE")
    {
      stringstream ss(parameterValue);
      ss >> _currentGesture.fadeInMS;
      //_currentGesture.motionSequence->setFadeInMs(_currentGesture.fadeInMS);
    }else
    {
      LOG_ERROR(parserLogger,"FADE_IN parameter can only be nested in an K_POSE_SEQUENCE element");
    }
  }else if (parameterKey == "FADE_OUT")
  {
    if (_parsedElements.back() == "K_POSE_SEQUENCE")
    {
      stringstream ss(parameterValue);
      ss >> _currentGesture.fadeOutMS;
      //_currentGesture.motionSequence->setFadeOutMs(_currentGesture.fadeOutMS);
    }else
    {
      LOG_ERROR(parserLogger,"FADE_OUT parameter can only be nested in an K_POSE_SEQUENCE element");
    }
  }else if (parameterKey == "ANIMATION_CLIP")
  {
    if (_parsedElements.back() == "GESTURE_COMPONENT" && _currentComponent.type == "ANIMATION")
    {
      _currentComponent.animationClip = parameterValue;
    }else
    {
      LOG_ERROR(parserLogger,"ANIMATION_CLIP parameter can only be nested in an ANIMATION element");
    }
  }else if (parameterKey == "POSE_KEY")
  {
    if (_parsedElements.back() == "CONSTRAINT" && _currentConstraint.type == "POSE_TARGET")
    {
      _currentConstraint.key = parameterValue;
    }else
    {
      LOG_ERROR(parserLogger,"POSE_KEY parameter can only be nested in an POSE_TARGET CONSTRAINT");
    }
  }else if (parameterKey == "INFLUENCE")
  {
    if (_parsedElements.back() == "CONSTRAINT" && _currentConstraint.type == "POSE_TARGET")
    {
      stringstream ss(parameterValue);
      ss >> _currentConstraint.influence;
    }else
    {
      LOG_ERROR(parserLogger,"INFLUENCE parameter can only be nested in an POSE_TARGET CONSTRAINT");
    }
  }else if (parameterKey == "JOINT")
  {
    if (_parsedElements.back() == "CONSTRAINT")
    {
      _currentConstraint.joint = parameterValue;
    }else
    {
      LOG_ERROR(parserLogger,"JOINT parameter can only be nested in a CONSTRAINT element");
    }
  }else if (parameterKey == "BODY_GROUP")
  {
    if (_parsedElements.back() == "CONSTRAINT")
    {
      _currentConstraint.stencil = parameterValue;
    }else if (_parsedElements.back() == "GESTURE_COMPONENT" && _currentComponent.type == "ANIMATION")
    {
      _currentComponent.stencil = parameterValue;
    }else
    {
      LOG_ERROR(parserLogger,"BODY_GROUP parameter can only be nested in a CONSTRAINT or an ANIMATION element");
    }
  }else if (parameterKey == "OFFSET")
  {
    if (_parsedElements.back() == "CONSTRAINT")
    {
      if (strcspn (parameterValue.c_str(),alpha) != parameterValue.length())
      {
        LOG_ERROR(parserLogger,parameterValue << " : parameter format is invalid");
        //exit(1);
      }
      stringstream ss(parameterValue);
      double posX,posY,posZ;
      posX = tokenize(parameterValue,";");
      posY = tokenize(parameterValue,";");
      posZ = tokenize(parameterValue,";");
      _currentConstraint.offset = SMRVector3(posX,posY,posZ);
    }else
    {
      LOG_ERROR(parserLogger,"OFFSET parameter can only be nested in a CONSTRAINT element");
    }
  }else if (parameterKey == "TARGET")
  {
    if (_parsedElements.back() == "CONSTRAINT")
    {
      if (strcspn (parameterValue.c_str(),alpha) != parameterValue.length())
      {
        LOG_ERROR(parserLogger,parameterValue << " : parameter format is invalid");
        //exit(1);
      }
      stringstream ss(parameterValue);
      double posX,posY,posZ;
      posX = tokenize(parameterValue,";");
      posY = tokenize(parameterValue,";");
      posZ = tokenize(parameterValue,";");
      _currentConstraint.target = SMRVector3(posX,posY,posZ);
    }else
    {
      LOG_ERROR(parserLogger,"TARGET parameter can only be nested in a CONSTRAINT element");
    }

  }else if (parameterKey == "SWIVEL_ANGLE")
  {
    if (_parsedElements.back() == "CONSTRAINT")
    {
      if (strcspn (parameterValue.c_str(),alpha) != parameterValue.length())
      {
        LOG_ERROR(parserLogger,parameterValue << " : parameter format is invalid");
        //exit(1);
      }
      stringstream ss(parameterValue);
      float angle;
      ss >> angle;
      _currentConstraint.angle = angle;
      //_currentConstraint.target = SMRVector3(posX,posY,posZ);
    }else
    {
      LOG_ERROR(parserLogger,"TARGET parameter can only be nested in a CONSTRAINT element");
    }

  }else if (parameterKey == "DIRECTION")
  {
    if (_parsedElements.back() == "CONSTRAINT")
    {
      if (strcspn (parameterValue.c_str(),alpha) != parameterValue.length())
      {
        LOG_ERROR(parserLogger,parameterValue << " : parameter format is invalid");
        //exit(1);
      }
      stringstream ss(parameterValue);
      double posX,posY,posZ;
      posX = tokenize(parameterValue,";");
      posY = tokenize(parameterValue,";");
      posZ = tokenize(parameterValue,";");
      _currentConstraint.direction = SMRVector3(posX,posY,posZ);
    }else
    {
      LOG_ERROR(parserLogger,"TARGET parameter can only be nested in a CONSTRAINT element");
    }
  }else if (parameterKey == "NORMAL")
  {
    if (_parsedElements.back() == "CONSTRAINT" && _currentConstraint.type == "ORIENTATION_CONSTRAINT")
    {
      _currentConstraint.normal= parameterValue;
    }else
    {
      LOG_ERROR(parserLogger,"NORMAL parameter can only be nested in an ORIENTATION CONSTRAINT element");
    }
  }else if (parameterKey == "FEEDBACK_ID")
  {
    if (_parsedElements.back() == "CONSTRAINT")
    {
      _currentConstraint.feedbackID= parameterValue;
    }else
    {
      LOG_ERROR(parserLogger,"FEEDBACK_ID parameter can only be nested in an CONSTRAINT element");
    }
  }else if (parameterKey == "TIME_WARP")
  {
    if (_parsedElements.back() == "K_POSE_SEQUENCE")
    {
      stringstream ss(parameterValue);
      float stipness;
      string symetryType;
      symetryType = stringTokenize(parameterValue,";");
      stipness = tokenize(parameterValue,";");
      _currentGesture.timeWarpStipness = stipness;
      _currentGesture.timeWarpSym = symetryType;
    }else
    {
      LOG_ERROR(parserLogger,"TIME_WARP parameter can only be nested in a K_POSE_SEQUENCE element");
    }
  }else if (parameterKey == "FEEDBACK_MESSAGE")
  {
    ActuatorFactory::getInstance()->setFeedbackMessage(parameterValue);
    LOG_ERROR(parserLogger,"a feedback message has been received: " << parameterValue);
    
  }else if (parameterKey == "SPATIAL_EXT")
  {
    if (_parsedElements.back() == "GESTURE_NUCLEUS")
    { 
      stringstream ss(parameterValue);
      ss >> _currentNucleus.spatialExt;
    }else
    {
      LOG_ERROR(parserLogger,"SPATIAL_EXT parameter can only be nested in a NUCLEUS element");
    }
  }else if (parameterKey == "TEMPORAL_EXT")
  {              
    if (_parsedElements.back() == "GESTURE_NUCLEUS")
    { 
      stringstream ss(parameterValue);
      ss >> _currentNucleus.temporalExt;
    }else
    {
      LOG_ERROR(parserLogger,"TEMPORAL_EXT parameter can only be nested in a NUCLEUS element");
    }
  }else if (parameterKey == "POWER")
  {
    if (_parsedElements.back() == "GESTURE_NUCLEUS")
    { 
      stringstream ss(parameterValue);
      ss >> _currentNucleus.powerValue;
    }else
    {
      LOG_ERROR(parserLogger,"POWER parameter can only be nested in a NUCLEUS element");
    }
  }else if (parameterKey == "FLUIDITY")
  {
    if (_parsedElements.back() == "GESTURE_NUCLEUS")
    {   
      stringstream ss(parameterValue);
      ss >> _currentNucleus.fluidityValue;
    }else
    { 
      LOG_ERROR(parserLogger,"FLUIDITY parameter can only be nested in a NUCLEUS element");
    }
  }else if (parameterKey == "NUCLEUS_ID")
  {
    if (_parsedElements.back() == "GESTURE_NUCLEUS")
    {   
      stringstream ss(parameterValue);
      ss >> _currentNucleus.nucID;
    }else
    { 
      LOG_ERROR(parserLogger,"NUCLEUS_ID parameter can only be nested in a NUCLEUS element and not in "<<_parsedElements.back());
    }
  }else
  {
    LOG_ERROR(parserLogger,parameterKey << " : This parameter is unknown");
  }
}
