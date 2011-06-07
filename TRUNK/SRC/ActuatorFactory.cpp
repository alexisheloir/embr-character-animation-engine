
#include "ActuatorFactory.h"


class XMLNODE
{
public:
    XMLNODE():name(""),type(0),depth(0),hasValue(false),value(""){};
   ~XMLNODE(){};

public:
  string  name;
  int     type;  
  int     depth; 
  bool    hasValue;
  string  value;
  map<string,string> attributes;
};

/**
 * \brief process a node
 */
static void
processNode(xmlTextReaderPtr reader) {
    const xmlChar *name, *value;

    name = xmlTextReaderConstName(reader);
    if (name == NULL)
	    name = BAD_CAST "--";

    value = xmlTextReaderConstValue(reader);

    printf("%d %d %s %d %d", 
	    xmlTextReaderDepth(reader),
	    xmlTextReaderNodeType(reader),
	    name,
	    xmlTextReaderIsEmptyElement(reader),
	    xmlTextReaderHasValue(reader));
    if (value == NULL)
	    printf("\n");
    else {
      if (xmlStrlen(value) > 40)
        printf(" %.40s...\n", value);
      else
	      printf(" %s\n", value);
    }
}

// Convert from degrees to radians
inline double degToRad( double d ) 
{
  return d * (3.1415926 / 180.0);
}

inline double radToDeg( double d ) 
{
  return d * (180.0 / 3.1415926);
}

// Singleton initialization.  At first, there is no object created.
ActuatorFactory *ActuatorFactory::m_singleton = NULL;

void ActuatorFactory::addPoseToAnimation(string _characterName, string _animationName, SMRSkeleton _pose)
{

  Character* currentCharacter = getCharacter(_characterName);

  map<string,SMRMotion>::iterator animationIterator;
  animationIterator = currentCharacter->m_animations.find(_animationName);
  if (animationIterator == currentCharacter->m_animations.end())
  {
    LOG_ERROR(logger, "animation "<< _animationName << "has not been registered");
  }else
  {
    SMRMotion *currentMotion = &(animationIterator->second);
    currentMotion->insertSkeleton(_pose); // TODO: should handle the last pose
  }
  
}

string getAttrValue(string _name, xmlNode *_node)
{
  string returnstring="";
  xmlAttr *currentAttr = _node->properties;
  for (currentAttr = _node->properties; currentAttr; currentAttr=currentAttr->next)
  {
    if(!xmlStrcmp(currentAttr->name,(const xmlChar *)_name.c_str()))
    {
      returnstring = reinterpret_cast<char*>(currentAttr->children->content);
    }
  }
  return returnstring;
}

void findNode(string _nameToFind,xmlNode *_nodeToSearch, xmlNode **result)
{
    *result = NULL;
    xmlNode *cur_node = NULL;
    for (cur_node = _nodeToSearch; cur_node; cur_node = cur_node->next) 
    {
      if (!xmlStrcmp(cur_node->name,(const xmlChar*)_nameToFind.c_str() )) 
      {
        *result = cur_node;
        return;
      }
      if (cur_node->children) findNode(_nameToFind,cur_node->children,result);
    if (*result != NULL) return;
    }
}

void ActuatorFactory::init()
{

/*
 * this initialize the library and check potential API mismatches
 * between the version it was compiled for and the actual shared
 * library used.
 */
  LIBXML_TEST_VERSION

/*
 * Pass some special parsing options to activate DTD attribute defaulting,
 * entities substitution and DTD validation
 */
  //int ret=0;

  char* filename("./characterConfiguration.xml");
  std::vector<string> characters;
  xmlDoc *doc = xmlReadFile(filename,NULL,XML_PARSE_DTDVALID |  /* validate with the DTD */
                                         XML_PARSE_DTDATTR ); /* default DTD attributes */
  
  
  if(doc != NULL)
  {
    const SMRSkeleton *posture;
    Character *currentCharacter;
    //retrieve the group name
    xmlNode *root_element = NULL;

    // get the root element of the xml document
    root_element = xmlDocGetRootElement(doc);

    xmlNode *groupsNode = NULL;
    xmlNode *autonomousParametersNode = NULL;
    xmlNode *characterNode = NULL;
    xmlNode *autonomousParameterNode = NULL;
    xmlNode *periodicBehaviorNode = NULL;
    xmlNode *actuatorMorphAnimationNode = NULL;
    xmlNode *actuatorSkelAnimationNode = NULL;
    xmlNode *morphKeyNode = NULL;
    xmlNode *shaderParametersNode = NULL;
    xmlNode *shaderParameterNode = NULL;
    xmlNode *animationsNode = NULL;
    xmlNode *animationNode = NULL;
    xmlNode *correlationMapNode = NULL;
    
     
    Scheduler *newScheduler;
    findNode("character", root_element, &characterNode);
    string characterName;
    do
    {
      if (characterNode == NULL)
      {
        LOG_ERROR(xmlLogger,"unable to find \"character\" node");
      }else
      {
        //retrieve the character's name
        characterName = getAttrValue("name",characterNode);
        characters.push_back(characterName);
        //Character* newCharacter = new Character();
        //m_characters[characterName] = newCharacter;
        currentCharacter = getCharacter(characterName);
        posture = &(currentCharacter->m_TPose);

        newScheduler = new Scheduler(characterName, ActuatorFactory::getInstance()->getCharacterAnimationPosture(characterName));
        newScheduler->setCurrentPose(ActuatorFactory::getInstance()->getCharacterTPosture(characterName));
        m_schedulerManager->addScheduler(characterName, newScheduler);
      }

      findNode("jointGroups",characterNode->children,&groupsNode);
      if (groupsNode == NULL)
      {
        LOG_ERROR(xmlLogger,"unable to find \"jointGroups\" node");
      }else
      {
        xmlNode *currentGroup = groupsNode;
        do
        {
          findNode("jointGroup",currentGroup,&currentGroup);
          if (currentGroup == NULL)
          {
            LOG_INFO(xmlLogger,"unable to find \"jointGroup\" node.");
          }else
          {
            //instanciate a new skeleton that will store the joints declared in the group
            string groupName = getAttrValue("id",currentGroup);
            //create a skeleton (will store the joints)
            SMRSkeleton currentGroupSkeleton = SMRSkeleton(posture->getMode(),posture->getRotationOrder(),groupName);
            //create a kinematic chain (IK purpose)
            SMRKinematicChain currentIKSkeleton(currentGroupSkeleton);

            //add the joints
            xmlNode *jointsNode = NULL;

            findNode("joints",currentGroup,&jointsNode);
            if (jointsNode == NULL)
            {
              LOG_ERROR(xmlLogger,"unable to find \"joints\" node.");
            }else
            {
              xmlNode *currentJointNode = jointsNode;
              do 
              {
                findNode("joint",currentJointNode,&currentJointNode);
                if ( currentJointNode == NULL )
                {
                  LOG_ERROR(xmlLogger,"unable to find \"joint\" node.");
                }else
                { 
                  //create new Joint
                  string jointName = getAttrValue("id",currentJointNode);
                  SMRJoint *retrievedJoint =  posture->getJointByName(jointName);
                  SMRJoint *currentJoint = NULL;
                  if (retrievedJoint == NULL) 
                    LOG_FATAL(xmlLogger,"the joint " + jointName + " defined in the character configuration file doesn't exist in the panda resource file" );
                  else
                    currentJoint = new SMRJoint(*retrievedJoint);
                  currentGroupSkeleton.insertJoint(currentJoint);
    
                  //the same, IK version...
                  SMRKinematicJoint *currentIKJoint = new SMRKinematicJoint(*posture->getJointByName(jointName));
                  currentIKSkeleton.insertJoint(currentIKJoint);

                  // add the DOFs to the newly created joint
                  xmlNode *dofsNode = NULL;
                  findNode("DOFS", currentJointNode,&dofsNode);
                  if ( dofsNode == NULL )
                  {
                    LOG_INFO(xmlLogger,"this joint doesn't contain any \"DOFS\" node.");
                  }else
                  {
                    xmlNode *currentDOFNode = dofsNode;
                    do
                    {
                      findNode("DOF",currentDOFNode,&currentDOFNode);
                      if (currentDOFNode == NULL)
                      {
                        LOG_ERROR(xmlLogger, "unable to find joint \" DOF \" node.");
                      }else
                      {
                        SMRVector3 currentAxis(0.0,0.0,0.0);
                        double upperBound = 0.0;
                        double lowerBound = 0.0;
                        double gain = 1.0;
                        //find the axis
                        istringstream upperBoundStream(getAttrValue("upperLimit",currentDOFNode));
                        upperBoundStream >> upperBound;
                        istringstream lowerBoundStream(getAttrValue("lowerLimit",currentDOFNode));
                        lowerBoundStream >> lowerBound;
                        istringstream gainStream(getAttrValue("gain",currentDOFNode));
                        gainStream >> gain;
                        xmlNode *axisNode = NULL;
                        findNode("DOFAxis",currentDOFNode,&axisNode);
                        string axis = getAttrValue("axis",currentDOFNode);
                        if (axis == "Xaxis")
                        {
                          SMRDOF *currentDof = currentIKJoint->getDOF(1);
                          currentDof->setLowerBound(currentDof->getRotationAngle() + degToRad(lowerBound));
                          currentDof->setUpperBound(currentDof->getRotationAngle() + degToRad(upperBound));
                          currentDof->setGain(gain);
                        }else if (axis == "Yaxis")
                        {
                          SMRDOF *currentDof = currentIKJoint->getDOF(4);
                          currentDof->setLowerBound(currentDof->getRotationAngle() + degToRad(lowerBound));
                          currentDof->setUpperBound(currentDof->getRotationAngle() + degToRad(upperBound));
                          currentDof->setGain(gain);
                        }else if (axis == "Zaxis")
                        {
                          SMRDOF *currentDof = currentIKJoint->getDOF(3);
                          currentDof->setLowerBound(currentDof->getRotationAngle() + degToRad(lowerBound));
                          currentDof->setUpperBound(currentDof->getRotationAngle() + degToRad(upperBound));
                          currentDof->setGain(gain);
                        }
                      }
                      findNode("DOF",currentDOFNode->next,&currentDOFNode);
                      //if (currentDOFNode) currentDOFNode = currentDOFNode->next;
                    }while(currentDOFNode);
                  }
                }
                findNode("joint",currentJointNode->next,&currentJointNode);
                //if (currentJointNode) currentJointNode = currentJointNode->next;
              }while(currentJointNode);
            }
            currentCharacter->m_jointGroups[groupName] = currentGroupSkeleton;
            SMRKinematicJoint* currentJoint = currentIKSkeleton.getJoint(0);
            string startJointName = currentJoint->getName();
            while(currentJoint->getParentName() != "")
            {
              string currentParentName = currentJoint->getParentName();
              //currentJoint = new SMRKinematicJoint(*posture->getJointByName(currentParentName));
              SMRJoint* parentJoint = posture->getJointByName(currentParentName);
              currentJoint = static_cast<SMRKinematicJoint*>(parentJoint);
              currentIKSkeleton.insertJoint(0,currentJoint);
            }
            currentIKSkeleton.setStartJointIndex(startJointName);
            currentCharacter->m_IKChains[groupName] = currentIKSkeleton;
        }
        if (currentGroup) currentGroup = currentGroup->next;
        }while(currentGroup);
      }
      findNode("correlationMap", characterNode->children, &correlationMapNode);
      while(correlationMapNode)
      {
        if(correlationMapNode == NULL)
        {
          LOG_INFO(xmlLogger,"unable to find \"correlationMap\" node");
        }else
        {
          xmlNode *targetJoint; 
          xmlNode *sourceJoint;
          findNode("targetJoint",correlationMapNode,&targetJoint);
          findNode("sourceJoint",correlationMapNode,&sourceJoint);
          if(targetJoint == NULL || sourceJoint == NULL)
          {
                LOG_INFO(xmlLogger,"unable to find \"targetJoint element or sourceJoint element\" ");
          }else
          {
            string sourceJointName = getAttrValue("name", sourceJoint);
            string sourceDOF = getAttrValue("type", sourceJoint);
            float sourceMinValue;
            istringstream minValueStream(getAttrValue("min",sourceJoint));
            minValueStream >> sourceMinValue;
            float sourceMaxValue;
            istringstream maxValueStream(getAttrValue("max",sourceJoint));
            maxValueStream >> sourceMaxValue;

            string targetJointName = getAttrValue("name", targetJoint);
            string targetDOF = getAttrValue("type", targetJoint);
            float targetMinValue;
            istringstream targetMinValueStream(getAttrValue("min",targetJoint));
            targetMinValueStream >> targetMinValue;
            float targetMaxValue;
            istringstream targetMaxValueStream(getAttrValue("max",targetJoint));
            targetMaxValueStream >> targetMaxValue;

            ComplexMotionSegment *currentMotionSegment = new SelfPostingMotionSegment(currentCharacter, "",40);
            
            currentMotionSegment->m_type = ANIMATION; //TODO:should be CORRELATIONMAP
            {
              ActuatorCorrelationMap *newActuator = new ActuatorCorrelationMap(currentCharacter, sourceJointName, sourceMinValue, sourceMaxValue, targetJointName, targetMinValue, targetMaxValue, newScheduler->getCurrentPose());
              currentMotionSegment->addActuator(newActuator);
              currentMotionSegment->setAbsoluteStartTime(0);
              currentMotionSegment->setAbsoluteStopTime(0);
              currentMotionSegment->setHoldDuration(0);
              newScheduler->enqueueMotionSegment(currentMotionSegment);
            }
          }
        findNode("correlationMap", correlationMapNode->next, &correlationMapNode);
        }
      }
      // New part, parse animation clips from the xml file
      findNode("animations", characterNode->children, &animationsNode);
      if(animationsNode == NULL)
      {
        LOG_INFO(xmlLogger,"unable to find \"animations\" node");
      }else
      {
        findNode("animation",animationsNode,&animationNode);
        if(animationNode == NULL)
        {
        }else
        {  
        // read animation nodes to import .bvh files
          do
          {
            xmlNode *bvhmotionNode;
            xmlNode *ressourceNode;
            findNode("bvhmotion",animationNode,&bvhmotionNode);
            findNode("ressource",animationNode,&ressourceNode);
          
            if(bvhmotionNode == NULL || ressourceNode  == NULL)
            {
              LOG_INFO(xmlLogger,"unable to find \"bvhmotionNode or ressourceNode\" node");
            }else
            {
              string parameterName = getAttrValue("name", bvhmotionNode);
              string parameterFilepath = getAttrValue("filepath", ressourceNode);
              // option to define the transformation order of the imported motion
              SMRTransformationOrderType transformationOrder = TRANSLATIONFIRST;        
              SMRMotion bvhMotion = loadMotionFromBVH(parameterFilepath,transformationOrder);
              Character* currentCharacter = getCharacter(characterName);
              //transform the motion from the bvh skeleton def to the panda def and add it to the motion dictionary
              LOG_INFO(xmlLogger,"Adding Animation "<<parameterName<<" for character "<<characterName);
              //this->transformToPandaSkeleton(bvhMotion, currentCharacter->m_TPose);
              currentCharacter->m_animations[parameterName] = bvhMotion;
              LOG_INFO(logger,"bvhmotion added has "<<bvhMotion.getNumFrames()<<" frames");
            }
            findNode("animation",animationNode->next,&animationNode);                  
          }while (animationNode);
        }
      }
      //end new part
      findNode("shaderParameters", characterNode->children, &shaderParametersNode);
      if(shaderParametersNode == NULL)
      {
        LOG_INFO(xmlLogger,"unable to find \"shaders\" node");
      }else
      {
        findNode("shaderParameter",shaderParametersNode,&shaderParameterNode);
        if(shaderParameterNode == NULL)
        {
        }else
        {
          do
          {
            string parameterName = getAttrValue("name", shaderParameterNode);
            stringstream parameterValue(getAttrValue("initialValue", shaderParameterNode));
            currentCharacter->addShaderParameter(parameterName);
            float value = 0.0f;
            parameterValue >> value;
            currentCharacter->setShaderParameterWeight(parameterName,value);
            
            //if (autonomousParameterNode) autonomousParameterNode = autonomousParameterNode->next;
            findNode("shaderParameter",shaderParameterNode->next,&shaderParameterNode);
          }while(autonomousParameterNode);
        }
      }
      findNode("autonomousParameters", characterNode->children, &autonomousParametersNode);
      if (autonomousParametersNode == NULL)
      {
        LOG_INFO(xmlLogger,"unable to find \"autonomousParameters\" node");
      }else
      {
        findNode("autonomousParameter",autonomousParametersNode,&autonomousParameterNode);
        if(autonomousParameterNode == NULL)
        {
        }else
        {
          do
          {
            string parameterName = getAttrValue("name", autonomousParameterNode);
            stringstream parameterValue(getAttrValue("initialValue", autonomousParameterNode));
            currentCharacter->addAutonomousParameter(parameterName);
            float value = 0.0f;
            parameterValue >> value;
            currentCharacter->setAutonomousParameterValue(parameterName,value);
            
            //if (autonomousParameterNode) autonomousParameterNode = autonomousParameterNode->next;
            findNode("autonomousParameter",autonomousParameterNode->next,&autonomousParameterNode);
          }while(autonomousParameterNode);
        }
      }
      findNode("autonomousBehavior", characterNode->children, &periodicBehaviorNode);
      if (periodicBehaviorNode == NULL)
      {
        LOG_INFO(xmlLogger,"unable to find \"periodicBehavior\" node");
      }else
      {
        do
        {
          string parameterName = getAttrValue("name", periodicBehaviorNode);
          string parameterFrequency = getAttrValue("frequency", periodicBehaviorNode);
          string behaviorType = getAttrValue("type", periodicBehaviorNode);

          ComplexMotionSegment *currentMotionSegment;

          if (behaviorType == "periodic")
          {
            currentMotionSegment = new SelfPostingMotionSegment(currentCharacter, parameterFrequency,40);
            newScheduler->enqueueMotionSegment(currentMotionSegment);
          }else
          {
            currentMotionSegment = new RandomPostingMotionSegment(currentCharacter, parameterFrequency,40);
            newScheduler->enqueueMotionSegment(currentMotionSegment);
          }

          currentMotionSegment->setAbsoluteStartTime(0);
          currentMotionSegment->setAbsoluteStopTime(0);

          findNode("actuatorMorphAnimation", periodicBehaviorNode, &actuatorMorphAnimationNode);
          if (actuatorMorphAnimationNode == NULL)
          {
            LOG_INFO(xmlLogger,"unable to find \"actuatorMorphAnimationNode\" node");
          }else
          {
            do
            {
              string value = getAttrValue("value", actuatorMorphAnimationNode);
              ActuatorMorphAnimation *currentMorphMotionActuator = new ActuatorMorphAnimation(currentCharacter, value);
              currentMotionSegment->addActuator(currentMorphMotionActuator);

              findNode("morphKey", actuatorMorphAnimationNode, &morphKeyNode);
              if(morphKeyNode == NULL)
              {
                LOG_INFO(xmlLogger,"unable to find \"morphKey\" node");
              }else
              {
                do
                {
                  string morphKey = getAttrValue("name", morphKeyNode);
                  stringstream cursorStringStream(getAttrValue("relativeKey", morphKeyNode));
                  float cursor = 0.0f;
                  cursorStringStream >> cursor;
                  stringstream valueStringStream(getAttrValue("value", morphKeyNode));
                  float value = 0.0f;
                  valueStringStream >> value;

                  currentMorphMotionActuator->addMorphKey(morphKey, cursor, value);
                  findNode("morphKey", morphKeyNode->next, &morphKeyNode);
                }while(morphKeyNode);
                  //if(actuatorMorphAnimationNode) actuatorMorphAnimationNode = actuatorMorphAnimationNode->next;
              }
              findNode("actuatorMorphAnimation", actuatorMorphAnimationNode->next, &actuatorMorphAnimationNode);
            }while(actuatorMorphAnimationNode);
          }
          findNode("actuatorSkelAnimation", periodicBehaviorNode, &actuatorSkelAnimationNode);
          if (actuatorSkelAnimationNode == NULL)
          {
            LOG_INFO(xmlLogger,"unable to find \"actuatorSkelAnimationNode\" node");
          }else
          {
            do
            {
              string animation = getAttrValue("animation", actuatorSkelAnimationNode);
              string jointGroup = getAttrValue("jointGroup", actuatorSkelAnimationNode);
              string useVelocity = getAttrValue("useVelocity", actuatorSkelAnimationNode);
              string value = getAttrValue("value", actuatorSkelAnimationNode);
              
              if (useVelocity == "True")
              {
                map<string,SMRMotion>::iterator animationIterator;
                animationIterator = currentCharacter->m_animations.find(animation);
                SMRMotion *currentMotion;
                if (animationIterator == currentCharacter->m_animations.end())
                {
                  LOG_ERROR(logger, "animation " << animation << "has not been registered");
                }else
                {
                  currentMotion = &(animationIterator->second);
                  cout << "derivate animation" << endl;
                  currentCharacter->m_animations[animation] = currentMotion->derivate();
                }

                ActuatorAnimation *currentSkelMotionActuator = new ActuatorAnimation(currentCharacter, &currentCharacter->m_animations[animation],currentCharacter->m_jointGroups[jointGroup],currentCharacter->m_animationPose,"derivate",value);
                currentMotionSegment->addActuator(currentSkelMotionActuator);
              }else
              {
                ActuatorAnimation *currentSkelMotionActuator = new ActuatorAnimation(currentCharacter, &currentCharacter->m_animations[animation],currentCharacter->m_jointGroups[jointGroup],currentCharacter->m_animationPose,"direct",value);
                currentMotionSegment->addActuator(currentSkelMotionActuator);
              }
              findNode("actuatorSkelAnimation", actuatorSkelAnimationNode->next, &actuatorSkelAnimationNode);
            }while(actuatorSkelAnimationNode);
          }
          findNode("autonomousBehavior", periodicBehaviorNode->next, &periodicBehaviorNode);
        }while(periodicBehaviorNode);
          LOG_ERROR(xmlLogger,"Exporting motion ");
         // exportMotionToBVH("hands_fist.bvh", currentCharacter->m_animations["hands_fist2"]);
      }
      findNode("character", characterNode->next, &characterNode);
    }while(characterNode);
    
  } else 
  {
    LOG_ERROR(xmlLogger,"Unable to open " << filename);
  }
  

  
  xmlFreeDoc(doc);
  xmlCleanupParser();
  

}

Character* ActuatorFactory::getCharacter(const string &_characterName)
{
  map<string, Character*>::iterator characterIterator;
  characterIterator = (m_characters.find(_characterName));
  if(characterIterator == m_characters.end())
  {
    LOG_ERROR(logger," Character " << _characterName << " is not registered");
    exit(1);
  }else
    return characterIterator->second;
}

void ActuatorFactory::setSchedulerManager(SchedulerManager *_schedulerManager)
{
  m_schedulerManager = _schedulerManager;
}

void ActuatorFactory::addCharacter(const string &_characterName, SMRSkeleton* const _referencePose)
{
  //create a new character and pile it
  Character* newCharacter = new Character(_referencePose);
  m_characters[_characterName] = newCharacter;
}

ActuatorFactory::ActuatorFactory():m_schedulerManager(NULL) 
{
  m_previousVector = SMRVector3(0.0,0.0,0.0);
}

ActuatorFactory::~ActuatorFactory() 
{
}

void ActuatorFactory::kill( void )
{
  if( m_singleton ) {
    delete m_singleton;
    m_singleton = NULL;
  }
}

ActuatorFactory *ActuatorFactory::getInstance()
{
  if( m_singleton== NULL ) {
    m_singleton = new ActuatorFactory;
  }
  return m_singleton;
}

void ActuatorFactory::sendMotionSegments(Scheduler *_scheduler)
{
  std::vector<MotionSegment*>::iterator currentSegmentIterator;
  
  for (currentSegmentIterator = m_segmentToSend.begin(); currentSegmentIterator < m_segmentToSend.end(); currentSegmentIterator++ )
  {
    //    new papo
    //     decide if it is IK segment or not
    if((*currentSegmentIterator)->m_type == KINEMATIC_POSE)
    { 
      m_segmentToCombine.push_back(*currentSegmentIterator);   
    }else
    {
      _scheduler->enqueueMotionSegment(*currentSegmentIterator);
    }
  }
  m_segmentToSend.clear();
} 

void ActuatorFactory::combinePoseSequence(string _characterName, float fadeIn, float fadeOut, string _timeWarpSym, float _timeWarpStipness)
{

  Character* currentCharacter = getCharacter(_characterName);

  Scheduler *_scheduler = m_schedulerManager->getScheduler(_characterName);
  std::vector<MotionSegment*>::iterator currentSegmentIterator;
  map<string, ComplexMotionSegment*>::iterator mapIterator;
  map<string, ComplexMotionSegment*> CMSmaps;

  

  //first, gathers segments to combine into complex motion segments
  for (currentSegmentIterator = m_segmentToCombine.begin(); currentSegmentIterator < m_segmentToCombine.end(); currentSegmentIterator++ )
  {
    LOG_INFO(parserLogger,"combining pose sequence");
    mapIterator = CMSmaps.find((*currentSegmentIterator)->m_stencil);
    ComplexMotionSegment* complexSegment;
    if(mapIterator == CMSmaps.end())
    {
      //no CMS for the current body group in the map 
      complexSegment = new ComplexMotionSegment(currentCharacter, fadeIn, fadeOut);
      if (_timeWarpSym == "TAN")
      {
        complexSegment->setTempWarpSpline(-1.0 * _timeWarpStipness, 0.0, 1.0, 1.0 + _timeWarpStipness, (2.0/_timeWarpStipness)/6.0);
      }else if (_timeWarpSym == "EXP" && _timeWarpStipness != 0)
      {
        complexSegment->setTempWarpSpline(-1.0 * _timeWarpStipness, 0.0, 1.0, 1.0 + 1.0/_timeWarpStipness,0.0);
      }else
      {
        complexSegment->setTempWarpSpline(-1.0,0.0,1.0,2.0,0.0);
      }
      CMSmaps.insert(std::pair<string,ComplexMotionSegment*>((*currentSegmentIterator)->m_stencil, complexSegment));
      complexSegment->m_type = (*currentSegmentIterator)->m_type;
    }else complexSegment = mapIterator->second;
    complexSegment->addMotionSegment((*currentSegmentIterator));
  }

  //second, prepare the motion segments
  for(mapIterator = CMSmaps.begin(); mapIterator != CMSmaps.end(); mapIterator++)
  { 
    (mapIterator->second)->combine();
    _scheduler->enqueueMotionSegment((mapIterator->second));
  }
  m_segmentToCombine.clear();
}

void ActuatorFactory::setupAndPostAnimationSegment(Scheduler * _scheduler, int _startTime, int _stopTime, int _holdDuration, string _stencil, string _animationKey, float relativeFadeIn, float relativeFadeOut)
{
  string currentCharacterName = _scheduler->getRelatedCharacter();
  Character *currentCharacter = getCharacter(currentCharacterName);
  if (_startTime == 0)
  {
    _startTime = _scheduler->getCurrentTimeMs();
    _stopTime = _stopTime + _scheduler->getCurrentTimeMs();
  }
  AnimationMotionSegment* animationSegment = new AnimationMotionSegment(currentCharacter, relativeFadeIn,relativeFadeOut);

  animationSegment->m_type = ANIMATION;
  animationSegment->m_stencil = _stencil;
  
  map<string,SMRMotion>::iterator animationIterator;
  map<string,SMRSkeleton>::iterator bodyGroupIterator;
  animationIterator = currentCharacter->m_animations.find(_animationKey);
  if(animationIterator == currentCharacter->m_animations.end())
  {
    LOG_ERROR(logger,"animation "<< _animationKey << "has not been registered");
  }else
  {
    bodyGroupIterator = currentCharacter->m_jointGroups.find(_stencil);
    if (bodyGroupIterator == currentCharacter->m_jointGroups.end())
    {
      LOG_ERROR(logger,"body group "<< _stencil << "has not been registered");
    }else
    {
      ActuatorAnimation *newActuator = new ActuatorAnimation(currentCharacter, &(animationIterator->second), bodyGroupIterator->second, _scheduler->getCurrentPose());
      newActuator->setEaseIn(0.0f);
      animationSegment->addActuator(newActuator);
      animationSegment->setAbsoluteStartTime(_startTime);
      animationSegment->setAbsoluteStopTime(_stopTime);
      animationSegment->setHoldDuration(_holdDuration);
      _scheduler->enqueueMotionSegment(animationSegment);
    }
  }
}

void ActuatorFactory::setupAndPostStoredPoseSegment(Scheduler * _scheduler, int _startTime, int _stopTime, int _holdDuration, string _stencil, string _animationKey, float _attenuationFactor, float relativeFadeIn, float relativeFadeOut)
{

  string currentCharacterName = _scheduler->getRelatedCharacter();
  Character *currentCharacter = getCharacter(currentCharacterName);

    if (_startTime == 0)
    {
      _startTime = _scheduler->getCurrentTimeMs();
      _stopTime = _stopTime + _scheduler->getCurrentTimeMs();
    }
    MotionSegment* animationSegment = new MotionSegment(currentCharacter, relativeFadeIn,relativeFadeOut);

    animationSegment->m_type = ANIMATION;
    animationSegment->m_stencil = _stencil;
    
    map<string,SMRMotion>::iterator animationIterator;
    map<string,SMRSkeleton>::iterator bodyGroupIterator;
    animationIterator = currentCharacter->m_animations.find(_animationKey);
    if(animationIterator == currentCharacter->m_animations.end())
    {
      LOG_ERROR(logger,"animation "<< _animationKey << "has not been registered");
    }else
    {
      bodyGroupIterator = currentCharacter->m_jointGroups.find(_stencil);
      if (bodyGroupIterator == currentCharacter->m_jointGroups.end())
      {
        LOG_ERROR(logger,"body group "<< _stencil << "has not been registered");
      }else
      {
        ActuatorAnimation *newActuator = new ActuatorAnimation(currentCharacter, &(animationIterator->second), bodyGroupIterator->second, _scheduler->getCurrentPose());
        newActuator->setEaseIn(1.0f);
        newActuator->setAlpha(_attenuationFactor);
        animationSegment->addActuator(newActuator);
        animationSegment->setAbsoluteStartTime(_startTime);
        animationSegment->setAbsoluteStopTime(_stopTime+_holdDuration);
        animationSegment->setHoldDuration(_holdDuration);
        
        m_segmentToSend.push_back(animationSegment);
      }
    }
}



void ActuatorFactory::setupAndPostMorphTargetSegment(Scheduler * _scheduler, int _startTime, int _stopTime, int _holdDuration, string _morphKey, float _morphWeight, float relativeFadeIn, float relativeFadeOut)
{
    Character* currentCharacter = getCharacter(_scheduler->getRelatedCharacter());

    if (_startTime == 0)
    {
      _startTime = _scheduler->getCurrentTimeMs();
      _stopTime = _stopTime + _scheduler->getCurrentTimeMs();
    }
    MotionSegment* morphTargetSegment = new MotionSegment(currentCharacter, relativeFadeIn,relativeFadeOut);

    morphTargetSegment->m_type = MORPH_TARGET;
    morphTargetSegment->m_stencil = _morphKey;

    map<string,float>::iterator morphTargetIterator;
    morphTargetIterator = currentCharacter->m_morphTargets.find(_morphKey);
    if(morphTargetIterator == currentCharacter->m_morphTargets.end())
    {
      LOG_ERROR(logger,"Morph target "<< _morphKey << "has not been registered");
    }else
    {
      ActuatorMorphTarget *newActuator = new ActuatorMorphTarget(currentCharacter,_morphKey, _morphWeight);
      morphTargetSegment->addActuator(newActuator);
      morphTargetSegment->setAbsoluteStartTime(_startTime);
      morphTargetSegment->setAbsoluteStopTime(_stopTime+_holdDuration);
      morphTargetSegment->setHoldDuration(_holdDuration);
      m_segmentToSend.push_back(morphTargetSegment);
    }
}

void ActuatorFactory::setupAndPostAutonomousBehaviorParameterSegment(Scheduler * _scheduler, int _startTime, int _stopTime, string _behaviorParameterKey, float _behaviorParameterValue, float relativeFadeIn, float relativeFadeOut)
{

    Character* currentCharacter = getCharacter(_scheduler->getRelatedCharacter());

    if (_startTime == 0)
    {
      _startTime = _scheduler->getCurrentTimeMs();
      _stopTime = _stopTime + _scheduler->getCurrentTimeMs();
    }
    MotionSegment* autonomousBehaviorSegment = new MotionSegment(currentCharacter, relativeFadeIn,relativeFadeOut);

    autonomousBehaviorSegment->m_type = ANIMATION;
    autonomousBehaviorSegment->m_stencil = _behaviorParameterKey;

    map<string,float>::iterator autonomousBehaviorIterator;
    autonomousBehaviorIterator = currentCharacter->m_autonomousParameters.find(_behaviorParameterKey);
    if(autonomousBehaviorIterator == currentCharacter->m_autonomousParameters.end())
    {
      LOG_ERROR(logger,"Autonomous behavior parameter "<< _behaviorParameterKey << "has not been registered");
    }else
    {
      ActuatorAutonomousBehaviorParameter *newActuator = new ActuatorAutonomousBehaviorParameter(currentCharacter, _behaviorParameterKey, _behaviorParameterValue);
      autonomousBehaviorSegment->addActuator(newActuator);
      autonomousBehaviorSegment->setAbsoluteStartTime(_startTime);
      autonomousBehaviorSegment->setAbsoluteStopTime(_stopTime);
      m_segmentToSend.push_back(autonomousBehaviorSegment);
    }
}

void ActuatorFactory::setupAndPostShaderParameterSegment(Scheduler * _scheduler, int _startTime, int _stopTime, int _holdDuration, string _shaderKey, float _shaderValue, float relativeFadeIn, float relativeFadeOut)
{

    Character* currentCharacter = getCharacter(_scheduler->getRelatedCharacter());

    if (_startTime == 0)
    {
      _startTime = _scheduler->getCurrentTimeMs();
      _stopTime = _stopTime + _scheduler->getCurrentTimeMs();
    }
    MotionSegment* shaderParameterTargetSegment = new MotionSegment(currentCharacter, relativeFadeIn,relativeFadeOut);

    shaderParameterTargetSegment->m_type = SHADER_TARGET;
    shaderParameterTargetSegment->m_stencil = _shaderKey;

    map<string,float>::iterator shaderParametersIterator;
    shaderParametersIterator = currentCharacter->m_shaderParameters.find(_shaderKey);
    if(shaderParametersIterator == currentCharacter->m_shaderParameters.end())
    {
      LOG_ERROR(logger,"Shader parameter "<< _shaderKey << " has not been registered");
    }else
    {
      ActuatorShaderParameter *newActuator = new ActuatorShaderParameter(currentCharacter, _shaderKey, _shaderValue);
      shaderParameterTargetSegment->addActuator(newActuator);
      shaderParameterTargetSegment->setAbsoluteStartTime(_startTime);
      shaderParameterTargetSegment->setAbsoluteStopTime(_stopTime+_holdDuration);
      shaderParameterTargetSegment->setHoldDuration(_holdDuration);
      m_segmentToSend.push_back(shaderParameterTargetSegment);
      LOG_TRACE(logger, "posting shader segment, tart time= " << _startTime << " stop time= " << _stopTime << " hold = " << _holdDuration << endl);
    }
}


void ActuatorFactory::setupAndPostMotionSegment(Scheduler * _scheduler, int _startTime, int _stopTime, int _holdDuration, string _actuatorType,string _stencil,string _firstActuatorParam,string _secondActuatorParam,string _thirdActuatorParam, SMRSmartPtr<GestureModifier> *_gestMod, bool _feedbackRequired, string messageID)
{
  string currentCharacterName = _scheduler->getRelatedCharacter();
  Character *currentCharacter = getCharacter(currentCharacterName);
  if (_actuatorType == "positionConstraint")
  {
    MotionSegment* slerpIKSegment = new MotionSegment(currentCharacter);

    slerpIKSegment->m_type = KINEMATIC_POSE;
    slerpIKSegment->m_stencil = _stencil;

    slerpIKSegment->addActuator(new ActuatorHybridIKSlerp(currentCharacter));

    slerpIKSegment->setAbsoluteStartTime(_startTime);
    slerpIKSegment->setAbsoluteStopTime(_stopTime+_holdDuration);
    slerpIKSegment->setHoldDuration(_holdDuration);
    slerpIKSegment->setGestureModifier(_gestMod);
    //ActuatorIKSlerp* actuator;
    ActuatorHybridIKSlerp* actuator;
    //actuator = static_cast<ActuatorIKSlerp*>(slerpIKSegment->getActuator(0));
    actuator = static_cast<ActuatorHybridIKSlerp*>(slerpIKSegment->getActuator(0));
    actuator->setKinematicChain(currentCharacter->m_IKChains[_stencil],currentCharacter->m_jointGroups[_stencil],_scheduler->getCurrentPose());
    actuator->m_feedbackRequired = _feedbackRequired; // tell the actuator to register a feedback message
    if (_feedbackRequired)
    {
      actuator->m_feedbackMessage.id = messageID;
    }

    //actuator->setGestureModifier(gestMod);
    stringstream token(_firstActuatorParam);
    float target_x,target_y,target_z;
    token >> target_x;
    token >> target_y;
    token >> target_z;

    stringstream token1(_thirdActuatorParam);
    float offset_x,offset_y,offset_z;
    token1 >> offset_x;
    token1 >> offset_y;
    token1 >> offset_z;

    actuator->flushConstraintPtrs(); // when using existing instances from factory
    actuator->setTarget(SMRVector3(target_x,target_y,target_z),_secondActuatorParam.c_str(),SMRVector3(offset_x,offset_y,offset_z));
    m_segmentToSend.push_back(slerpIKSegment);
  }
  else if (_actuatorType == "swivelConstraint")
  {
    ActuatorHybridIKSlerp *actuator = NULL;

    // first, check if there is a pre existing positionIK MotionSegment
    std::vector<MotionSegment*>::iterator motionSegmentIt;
    for (motionSegmentIt = m_segmentToSend.begin(); motionSegmentIt < m_segmentToSend.end(); motionSegmentIt++)
    {
      MotionSegment *motionSegment = *motionSegmentIt;
      if ( _stencil == motionSegment->m_stencil && motionSegment->m_type == KINEMATIC_POSE)
      {
        //this is it, get the actuator
        actuator = static_cast<ActuatorHybridIKSlerp *>(motionSegment->getActuator(0));
      }
    }
    if (actuator) // actuator exists, there must be a constraint for the joint
    {
      stringstream ss(_firstActuatorParam);
      float angle;
      ss >> angle;
      actuator->setAngle(angle);
    }
  }
  else if (_actuatorType == "orientationConstraint")
  { 
    ActuatorHybridIKSlerp *actuator = NULL;

    // first, check if there is a pre existing positionIK MotionSegment
    std::vector<MotionSegment*>::iterator motionSegmentIt;
    for (motionSegmentIt = m_segmentToSend.begin(); motionSegmentIt < m_segmentToSend.end(); motionSegmentIt++)
    {
      MotionSegment *motionSegment = *motionSegmentIt;
      if ( _stencil == motionSegment->m_stencil && ( motionSegment->m_type == KINEMATIC_POSE))
      {
        //this is it, get the actuator
        actuator = static_cast<ActuatorHybridIKSlerp *>(motionSegment->getActuator(0));
      }
    }
    if (!actuator) // actuator doesn't exists, create it (inside a new motion segment)
    {
      MotionSegment* slerpIKSegment = new MotionSegment(currentCharacter);

      slerpIKSegment->m_type = KINEMATIC_POSE;
      slerpIKSegment->m_stencil = _stencil;

      slerpIKSegment->addActuator(new ActuatorHybridIKSlerp(currentCharacter));

      slerpIKSegment->setAbsoluteStartTime(_startTime);
      slerpIKSegment->setAbsoluteStopTime(_stopTime+_holdDuration);
      slerpIKSegment->setHoldDuration(_holdDuration);
      slerpIKSegment->setGestureModifier(_gestMod);
      actuator = static_cast<ActuatorHybridIKSlerp*>(slerpIKSegment->getActuator(0));

      actuator->setKinematicChain(currentCharacter->m_IKChains[_stencil],currentCharacter->m_jointGroups[_stencil],_scheduler->getCurrentPose());

      actuator->flushConstraintPtrs(); // when using existing instances from factory
      m_segmentToSend.push_back(slerpIKSegment);

    }

    stringstream token(_firstActuatorParam);
    float target_x,target_y,target_z;
    token >> target_x;
    token >> target_y;
    token >> target_z;
    SMRVector3 orientationOffset = SMRVector3(target_x, target_y, target_z);
    orientationOffset.normalize();
    orientationOffset = orientationOffset * 0.06; // unit is meter

    SMRVector3 direction;
    if (_thirdActuatorParam == "Xaxis")
    {
      direction = SMRVector3(0.06,0.0,0.0);
    }else if (_thirdActuatorParam == "Yaxis")
    {
      direction = SMRVector3(0.0,0.06,0.0);
    }else if (_thirdActuatorParam == "Zaxis")
    {
      direction = SMRVector3(0.0,0.0,0.06);
    }else if (_thirdActuatorParam == "")
    {
      direction = SMRVector3(0.0,0.0,-0.06);
    }else
    {
      LOG_ERROR(logger,"unknown normal value: " << _thirdActuatorParam);
    }
    actuator->setTarget(_secondActuatorParam.c_str(), _secondActuatorParam.c_str(), direction, orientationOffset);


  }
  else if (_actuatorType == "lookAtConstraint")
  {
    if (_stencil != "eyes")
    {
      MotionSegment* slerpIKSegment = new MotionSegment(currentCharacter);

      slerpIKSegment->m_type = KINEMATIC_POSE;
      slerpIKSegment->m_stencil = _stencil;

      slerpIKSegment->addActuator(new ActuatorHybridIKSlerp(currentCharacter));

      slerpIKSegment->setAbsoluteStartTime(_startTime);
      slerpIKSegment->setAbsoluteStopTime(_stopTime+_holdDuration);
      slerpIKSegment->setHoldDuration(_holdDuration);
      slerpIKSegment->setGestureModifier(_gestMod);
      ActuatorHybridIKSlerp* actuator;
      actuator = static_cast<ActuatorHybridIKSlerp*>(slerpIKSegment->getActuator(0));
      actuator->m_feedbackRequired = _feedbackRequired; // tell the actuator to register a feedback message
      if (_feedbackRequired)
      {
        actuator->m_feedbackMessage.id = messageID;
      }
      actuator->setKinematicChain(currentCharacter->m_IKChains[_stencil],currentCharacter->m_jointGroups[_stencil],_scheduler->getCurrentPose());

      stringstream token(_firstActuatorParam);
      float target_x,target_y,target_z;
      token >> target_x;
      token >> target_y;
      token >> target_z;

      actuator->flushConstraintPtrs(); // when using existing instances from factory (in that case, a previously instanciated actuto may still have constraints)

      SMRSkeleton tmpSkel = *(_scheduler->getCurrentPose());
      tmpSkel.setMode(ABSOLUTEMODE);

      SMRVector3 headEndPos = tmpSkel.getJointByName("headEnd")->getPosition();
      SMRVector3 headEndOrientationTarget = SMRVector3(target_x,target_y,target_z) - headEndPos;
      headEndOrientationTarget.normalize();
      headEndOrientationTarget *= 0.06;
      actuator->setTarget("headEnd","headEnd",SMRVector3(0.0,0.06,0.0),headEndOrientationTarget);

      m_segmentToSend.push_back(slerpIKSegment);
    }
    {
      // create a motion segment for each eye.
      MotionSegment* slerpLEyeIKSegment = new MotionSegment(currentCharacter);
      slerpLEyeIKSegment->m_type = KINEMATIC_POSE;
      slerpLEyeIKSegment->m_stencil = "leye";

      MotionSegment* slerpREyeIKSegment = new MotionSegment(currentCharacter);
      slerpREyeIKSegment->m_type = KINEMATIC_POSE;
      slerpREyeIKSegment->m_stencil = "reye";

      slerpLEyeIKSegment->addActuator(new ActuatorIKSlerp(currentCharacter));
      slerpREyeIKSegment->addActuator(new ActuatorIKSlerp(currentCharacter));

      slerpLEyeIKSegment->setAbsoluteStartTime(_startTime);
      slerpREyeIKSegment->setAbsoluteStartTime(_startTime);

      slerpLEyeIKSegment->setHoldDuration(_holdDuration);
      slerpREyeIKSegment->setHoldDuration(_holdDuration);

      slerpLEyeIKSegment->setAbsoluteStopTime(_stopTime+_holdDuration);
      slerpREyeIKSegment->setAbsoluteStopTime(_stopTime+_holdDuration);

      slerpLEyeIKSegment->setGestureModifier(_gestMod);
      slerpREyeIKSegment->setGestureModifier(_gestMod);

      ActuatorIKSlerp *lEyeActuator,*rEyeActuator;
      lEyeActuator = static_cast<ActuatorIKSlerp*>(slerpLEyeIKSegment->getActuator(0));
      rEyeActuator = static_cast<ActuatorIKSlerp*>(slerpREyeIKSegment->getActuator(0));

      lEyeActuator->setKinematicChain(currentCharacter->m_IKChains["leye"],currentCharacter->m_jointGroups["leye"],_scheduler->getCurrentPose());
      rEyeActuator->setKinematicChain(currentCharacter->m_IKChains["reye"],currentCharacter->m_jointGroups["reye"],_scheduler->getCurrentPose());

      stringstream token(_firstActuatorParam);
      float target_x,target_y,target_z;
      token >> target_x;
      token >> target_y;
      token >> target_z;

      lEyeActuator->flushConstraintPtrs(); // when using existing instances from factory (in that case, a previously instanciated actuator may still have constraints)
      rEyeActuator->flushConstraintPtrs(); // when using existing instances from factory (in that case, a previously instanciated actuator may still have constraints)

      SMRSkeleton tmpSkel = *(_scheduler->getCurrentPose());
      tmpSkel.setMode(ABSOLUTEMODE);

      SMRVector3 lEyePos = tmpSkel.getJointByName("leyeEnd")->getPosition();
      SMRVector3 rEyePos = tmpSkel.getJointByName("reyeEnd")->getPosition();

      SMRVector3 lEyeOrientationTarget = SMRVector3(target_x,target_y,target_z) - lEyePos;
      SMRVector3 rEyeOrientationTarget = SMRVector3(target_x,target_y,target_z) - rEyePos;

      lEyeOrientationTarget.normalize();
      rEyeOrientationTarget.normalize();

      lEyeActuator->setTarget("leyeEnd","leyeEnd",SMRVector3(0.0,1.0,0.0),lEyeOrientationTarget);
      rEyeActuator->setTarget("reyeEnd","reyeEnd",SMRVector3(0.0,1.0,0.0),rEyeOrientationTarget);

      m_segmentToSend.push_back(slerpREyeIKSegment);
      m_segmentToSend.push_back(slerpLEyeIKSegment);
    }
  }
}

void ActuatorFactory::setupAndPostCorrelationMapMotionSegment(Scheduler * _scheduler)
{
  string currentCharacterName = _scheduler->getRelatedCharacter();
  Character *currentCharacter = getCharacter(currentCharacterName);
  //int startTime = 0;
  //int stopTime = INT_MAX;
  //
  AnimationMotionSegment *animationSegment = new AnimationMotionSegment(currentCharacter, 0.0f,0.0f);

  //animationSegment->m_type = ANIMATION;
  animationSegment->m_type = ANIMATION; //TODO:should be CORRELATIONMAP
  {
    //ActuatorCorrelationMap *newActuator = new ActuatorCorrelationMap(currentCharacter, "lhand" , "spine4", _scheduler->getCurrentPose());
    //animationSegment->addActuator(newActuator);
    //animationSegment->setAbsoluteStartTime(startTime);
    //animationSegment->setAbsoluteStopTime(stopTime);
    //animationSegment->setHoldDuration(0);
    //_scheduler->enqueueMotionSegment(animationSegment);
  }
}


const SMRSkeleton* ActuatorFactory::getCharacterTPosture(const string &_characterName)
{
  return &((m_characters[_characterName])->m_TPose);
}

SMRSkeleton* const ActuatorFactory::getCharacterAnimationPosture(const string &_characterName)
{
  Character* currentCharacter = getCharacter(_characterName);
  return currentCharacter->m_animationPose;
}


void ActuatorFactory::addAnimation(string _characterName, string _animationKey)
{
  Character* currentCharacter = getCharacter(_characterName);
  currentCharacter->m_animations[_animationKey] = SMRMotion();
}

void ActuatorFactory::setFeedbackMessage(const string &message)
{
  m_feedbackMessage = message;
}

string ActuatorFactory::getFeedbackMessage(void)
{
  return m_feedbackMessage;
}

void ActuatorFactory::transformToPandaSkeleton(SMRMotion & _bvhMotion,SMRSkeleton _pandaSkeleton)
{
   LOG_TRACE(logger,"bvhSkeleton: " <<_bvhMotion.getSkeleton(0));
   LOG_TRACE(logger,"_pandaSkeleton: " <<_pandaSkeleton);    
  for (unsigned int i = 0; i < _bvhMotion.getNumFrames(); i++)
  {  
    SMRJoint *BVHRootJoint = _bvhMotion.getSkeleton(i).getJointByName("root");
    SMRQuaternion rootOrientation = BVHRootJoint->getOrientation();
    rootOrientation.normalize();
    BVHRootJoint->setOrientation(rootOrientation);
    SMRSkeleton frameSkel = _bvhMotion.getSkeleton(i);

    for( unsigned int j = 0; j < _pandaSkeleton.getNumJoints(); ++j)
    {
      SMRJoint *currentBVHJoint   = _bvhMotion.getSkeleton(i).getJoint(j);
      SMRJoint *currentPandaJoint = _pandaSkeleton.getJoint(j);
            
      SMRQuaternion pandaOrientation = currentPandaJoint->getOrientation();
      pandaOrientation.normalize();
      SMRQuaternion inversePO = Inverse(pandaOrientation);
      inversePO.normalize();
      SMRQuaternion bvhOrientation = currentBVHJoint->getOrientation();
      bvhOrientation.normalize();
      SMRQuaternion inverseBVH = Inverse(bvhOrientation);
      //Correct bone orientation
      bvhOrientation = pandaOrientation * bvhOrientation; 
      bvhOrientation.normalize();
      currentBVHJoint->setOrientation(bvhOrientation);
      // correct initial bone orientation (initial bone orientation is set to identity in BVH files)
      SMRVector3 bvhBonePosition = currentBVHJoint->getPosition();
      //pandaOrientation.rotate(bvhBonePosition);
      inversePO.rotate(bvhBonePosition);
      currentBVHJoint->setPosition(bvhBonePosition);
    } 
  }
}