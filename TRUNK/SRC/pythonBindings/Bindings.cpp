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


/**
 *
 * This file is the python wrapper for SMR/EMBR
 *
 */
//#define BOOST_DEBUG_PYTHON
//#define BOOST_DEBUG_PYTHON
//#include <boost/python/detail/wrap_python.hpp>
#include <boost/python.hpp>  // include the python binding declarations

//#include "SMR.h" // include the standard SMR declarations
//#include "SmrIO.h"
//#include "SMRInvJacSolver.h"
//#include "SMRGSMMSolver.h"
#include "../Realizer.h"

using namespace boost::python;

/**
 * Wrapper for boost.python, as type polymorphism is not supported, instances of generic classes
 * must be explicitely specified
 * class SMRVector3
 */
class SMRVector3Wrap : public SMRVector3
{
public:
  SMRVector3Wrap(): SMRVector3()
  {
  }
  SMRVector3Wrap(double x, double y, double z): SMRVector3(x,y,z)
  {
  }
  SMRVector3Wrap(const SMRVector3 &_vector): SMRVector3(_vector)
  {
  }
  SMRVector3Wrap default_normalize(void)
  {
    this->normalize();
    return *this;
  }
};

/**
 * Wrapper for boost.python, as type polymorphism is not supported, instances of generic classes
 * must be explicitely specified
 * class SMRQuaternion
 */
class SMRQuaternionWrap : public SMRQuaternion
{
public:
  SMRQuaternionWrap():SMRQuaternion()
  {
  }
  SMRQuaternionWrap( const SMRQuaternion _quat ):SMRQuaternion( _quat )
  {
  }
  SMRQuaternionWrap( const SMRVector3Wrap &_axis, double _angle ):SMRQuaternion( _axis, _angle )
  {
  }
  SMRQuaternionWrap( double _rotX, double _rotY, double _rotZ ):SMRQuaternion( _rotX, _rotY, _rotZ )
  {
  }
  SMRQuaternionWrap( double _w, double _x, double _y, double _z ):SMRQuaternion( _w, _x, _y, _z )
  {
  }
  SMRQuaternionWrap default_normalize(void)
  {
    this->normalize();
    return *this;
  }
  SMRVector3Wrap default_getRotationAxis(void)
  {
    SMRVector3Wrap axis(this->getRotationAxis());
    return axis;
  }
  SMRQuaternionWrap default_multiply(SMRQuaternionWrap _quaternion)
  {
    return (*this * _quaternion) ;
  }
  double getX()
  {
    return m_x;
  }
  double getY()
  {
    return m_y;
  }
  double getZ()
  {
    return m_z;
  }
  double getW()
  {
    return m_w;
  }
  SMRVector3Wrap default_toEulerAngles()
  {
    double x,y,z;
    this->toEulerAngles(x,y,z);

    return SMRVector3Wrap(x,y,z);
  }

};

SMRQuaternionWrap InverseWrap(const SMRQuaternionWrap &_quaternion)
{
 return Inverse(_quaternion);
}

class SMRDOFWrap : public SMRDOF
{
public:
  SMRDOFWrap(PyObject* self): SMRDOF(), self(self) 
  {
  }

  PyObject* self;

  void default_setRotationAxis(SMRVector3Wrap _vector3)
  {
    this->setRotationAxis(_vector3);
  }

};

class SMRKinematicJointWrap : public SMRKinematicJoint
{
public:
  SMRKinematicJointWrap(PyObject* self, bool _endJoint) : SMRKinematicJoint(_endJoint), self(self)
  {
  }

  PyObject* self;

  SMRKinematicJointWrap( const SMRKinematicJointWrap &_joint ) : SMRKinematicJoint(_joint)
  {
  }

  SMRKinematicJointWrap( const SMRKinematicJoint &_joint ) : SMRKinematicJoint(_joint)
  {
  }

  SMRQuaternionWrap default_getOrientation()
  {
    SMRQuaternionWrap returnQuat(this->getOrientation());
    return returnQuat;
  }

  handle<> default_getDOF(const int _index)
  {
    SMRDOFWrap* DOFPtr = static_cast<SMRDOFWrap*>(this->getDOF(_index));
    assert(DOFPtr != 0);
    return handle<>(borrowed(DOFPtr->self));
  }

  void default_addDOF(SMRDOFWrap DOF)
  {
    m_dofVector.push_back(DOF);;
  }


  void default_createDOF(SMRVector3Wrap _axis, double _lowerBound, double _upperBound, double _value, double gain)
  {
    this->addDOF(_axis, _lowerBound, _upperBound, _value, gain);
  }

  void setPos_3Doubles(double x, double y, double z)
  {
    setPosition(x,y,z);
  }

  void setRot_3Doubles(double x, double y, double z)
  {
    setOrientation(x,y,z);
  }
  void setRot_Quaternion(double w, double x, double y, double z)
  {
    setOrientation(SMRQuaternion(w,x,y,z));
  }
  void setEndPos_3Doubles(double x, double y, double z)
  {
    setEndLength(x,y,z);
  }
};

class SMRJointWrap : public SMRJoint
{
public:
  SMRJointWrap(PyObject* self, bool _endJoint) : SMRJoint(_endJoint), self(self)
  {
  }

  PyObject* self;

  SMRJointWrap( const SMRJointWrap &_joint ) : SMRJoint(_joint)
  {
  }

  SMRJointWrap( const SMRJoint &_joint ) : SMRJoint(_joint)
  {
  }

  SMRQuaternionWrap default_getOrient()
  {
    SMRQuaternionWrap returnQuat(this->getOrientation());
    return returnQuat;
  }

  void setPos_3Doubles(double x, double y, double z)
  {
    setPosition(x,y,z);
  }

  SMRVector3Wrap default_getPos(void)
  {
    return SMRVector3Wrap(getPosition());
  }

  void setRot_3Doubles(double x, double y, double z)
  {
    setOrientation(x,y,z);
  }

  void setRot_Quaternion(double w, double x, double y, double z)
  {
    SMRQuaternion newQuaternion(w,x,y,z);
    //newQuaternion.normalize();
    setOrientation(newQuaternion);
  }
  void setEndPos_3Doubles(double x, double y, double z)
  {
    setEndLength(x,y,z);
  }
};

/**
 * Wrapper for boost.python, as type polymorphism is not supported, instances of generic classes
 * must be explicitely specified
 * class SMRSkeletonWrap
 */
class SMRSkeletonWrap : public SMRSkeleton
{
public:
  SMRSkeletonWrap(PyObject* self, bool mode, bool rotOrder, string name=""): SMRSkeleton(mode,rotOrder,name), self(self)
  {
  }

  PyObject* self;

  handle<> default_getJoint(int jointNumber)
  {
    boost::mutex::scoped_lock lockSkeletonModifier(skeletonMutex);
    SMRJointWrap* jointPtr = static_cast<SMRJointWrap*>(this->getJoint(jointNumber));
    assert(jointPtr != 0);
    return handle<>(borrowed(jointPtr->self));    
  }

  handle<> default_getJointByName(string jointName)
  {
    SMRJointWrap* jointPtr = static_cast<SMRJointWrap*>(this->getJointByName(jointName));
    assert(jointPtr != 0);
    return handle<>(borrowed(jointPtr->self));
  }

  void default_insertJoint(SMRJointWrap* _joint)
  {
    return this->insertJoint(static_cast<SMRJoint*>(_joint));
  }

  bool isRelative()
  {
    return this->m_mode;
  }
};
/**
 * Wrapper for boost.python, as type polymorphism is not supported, instances of generic classes
 * must be explicitely specified
 * class SMRKinematicChainWrap
 */
class SMRMotionWrap : public SMRMotion
{
public:
  SMRMotionWrap():SMRMotion()
  {
  }
  void insertSkeletonToMotion(const SMRSkeletonWrap & _skeleton)
  {
    this->insertSkeleton(_skeleton);
  }
  void setTimeStepWrap(double dt)
  {
    this->setTimeStep(dt);
  }  
  /*void insertSkeleton(const SMRSkeleton & skeleton, int pos)    */
};



/**
 * Wrapper for boost.python, as type polymorphism is not supported, instances of generic classes
 * must be explicitely specified
 * class SMRKinematicChainWrap
 */
class SMRKinematicChainWrap : public SMRKinematicChain
{
public:
  SMRKinematicChainWrap(PyObject* self, bool mode, bool rotOrder, string name="") :  SMRKinematicChain(mode,rotOrder,name), self(self)
  {
  }

  PyObject* self;

  handle<> default_getJoint(int jointNumber)
  {
    SMRKinematicJointWrap* jointPtr = static_cast<SMRKinematicJointWrap*>(this->getJoint(jointNumber));
    assert(jointPtr != 0);
    return handle<>(borrowed(jointPtr->self));
  }

  handle<> default_getJointByName(string jointName)
  {
    SMRKinematicJointWrap* jointPtr = static_cast<SMRKinematicJointWrap*>(this->getJointByName(jointName));
    assert(jointPtr != 0);
    return handle<>(borrowed(jointPtr->self));
  }

  void default_insertJoint(SMRKinematicJointWrap* _joint)
  {
    return this->insertJoint(static_cast<SMRKinematicJoint*>(_joint));
  }

};

class SMRIKConstraintWrap : public SMRIKConstraint 
{
public:
  SMRIKConstraintWrap()
  {
  }
  void default_setRelatedJointName(char* _jointName)
  {
    return this->setRelatedJointName(_jointName);
  }
  void default_setPosition(SMRVector3Wrap _position)
  {
    this->setPosition(_position);
  }
  void default_setOffset(SMRVector3Wrap _offset)
  {
    this->setOffset(_offset);
  }
};

class SMRGSMMSolverWrap : public SMRGSMMSolver
{
public:
  SMRGSMMSolverWrap(SMRKinematicChainWrap *kinematicChain):SMRGSMMSolver(kinematicChain)
  {
  }
  void default_addConstraintPtr(SMRIKConstraintWrap * _constraintPtr)
  {
    //cout << _constraintPtr->getOffset() << endl;
    this->addConstraintPtr(_constraintPtr);
  }
  void default_process()
  {
    this->process(0.0f);
  }
  handle<> default_getSkeleton(float attenuationFactor)
  {
    SMRSkeletonWrap* skeletonPtr = static_cast<SMRSkeletonWrap*>(this->getSkeleton(attenuationFactor));
    return handle<>(borrowed(skeletonPtr->self));
  }

};

void exportPoseToBvhWrap(string _filename, SMRSkeletonWrap &_bindPose)
{
  exportPoseToBVH(_filename,_bindPose);
}

void exportMotionToBvhWrap(string _filename, SMRMotionWrap &_bindMotion)
{
   //call to void exportMotionToBVH(string _filename, SMRMotion &_motion, SMRSkeleton &_bindPose, RootRotationOrderType _rootRotOrder)
   exportMotionToBVH(_filename,_bindMotion);
}

void exportMotionToBvhWithSkeletonWrap(string _filename, SMRMotionWrap &_bindMotion, SMRSkeletonWrap &_bindPose)
{
   //call to void exportMotionToBVH(string _filename, SMRMotion &_motion, SMRSkeleton &_bindPose, RootRotationOrderType _rootRotOrder)
   exportMotionToBVH(_filename,_bindMotion,_bindPose);
}

class RealizerWrap : public Realizer
{
public:
  RealizerWrap():Realizer()
  {
  }

  void addCharacter(const string &_characterName, SMRSkeletonWrap* const _skeleton)
  {
    ActuatorFactory::getInstance()->addCharacter(_characterName, _skeleton);
  }

  handle<> default_getCharacterPosture(const string &_character)
  {
    boost::mutex::scoped_lock lockSkeletonModifier(skeletonMutex);
    SMRSkeletonWrap* skeletonPtr = static_cast<SMRSkeletonWrap*>(this->getCharacterPose(_character));
    return handle<>(borrowed(skeletonPtr->self));
  }

  void default_addMorphTarget(const string &_characterName, const string &_morphTargetName)
  {
    Character* currentCharacter = ActuatorFactory::getInstance()->getCharacter(_characterName);
    currentCharacter->addMorphTarget(_morphTargetName);
  }

  void default_setMorphTargetWeight(const string &_characterName, const string &_morphTargetName, float _value)
  {
    Character* currentCharacter = ActuatorFactory::getInstance()->getCharacter(_characterName);
    currentCharacter->setMorphTargetWeight(_morphTargetName, _value);
  }

  float default_getMorphTargetWeight(const string &_characterName, const string &_morphTargetName)
  {
    Character* currentCharacter = ActuatorFactory::getInstance()->getCharacter(_characterName);
    float weight=currentCharacter->getMorphTargetWeight(_morphTargetName);
    assert(!isNaN(weight));
    return weight;
  }

  boost::python::list default_getModifiedMorphTargets(const string &_characterName)
  {
      boost::python::list myList; // find a way to register the modified morph targets here (in scheduler?)
      Character* currentCharacter = ActuatorFactory::getInstance()->getCharacter(_characterName);
      std::vector<string>::iterator morphTargetIterator = currentCharacter->m_modifiedMorphTargets.begin();
      while (morphTargetIterator != currentCharacter->m_modifiedMorphTargets.end())
      {
          myList.append(*morphTargetIterator);
          morphTargetIterator = currentCharacter->m_modifiedMorphTargets.erase(morphTargetIterator);
      }
      return myList;
  }
    
  void default_addShaderParameter(const string &_characterName, const string &_shaderParameterName)
  {
    Character* currentCharacter = ActuatorFactory::getInstance()->getCharacter(_characterName);
    currentCharacter->addShaderParameter(_shaderParameterName);
  }

  void default_setShaderParameterWeight(const string &_characterName, const string &_shaderParameterName, float _value)
  {
    Character* currentCharacter = ActuatorFactory::getInstance()->getCharacter(_characterName);
    currentCharacter->setShaderParameterWeight(_shaderParameterName, _value);
  }

  float default_getShaderParameterWeight(const string &_characterName, const string &_shaderParameterName)
  {
    Character* currentCharacter = ActuatorFactory::getInstance()->getCharacter(_characterName);
    return currentCharacter->getShaderParameterWeight(_shaderParameterName);
  }

  void default_addAnimation(string _characterName, string _animationKey)
  {
    ActuatorFactory::getInstance()->addAnimation(_characterName, _animationKey);
  }

  void default_addPoseToAnimation(string _characterName, string _animationKey, SMRSkeletonWrap *_pose)
  {
    SMRSkeleton pose = *_pose;
    pose.setName(_animationKey);
    ActuatorFactory::getInstance()->addPoseToAnimation(_characterName, _animationKey, pose);
  }

  void init()
  {
    ActuatorFactory::getInstance()->init();
  }

  //void lock(string agentName)
  //{
  //  ActuatorFactory::getInstance()->lock(agentName);
  //}

  //void unLock(string agentName)
  //{
  //  ActuatorFactory::getInstance()->unLock(agentName);
  //}

  void default_run()
  {
    Py_BEGIN_ALLOW_THREADS
      run();
    Py_END_ALLOW_THREADS
  }

};

/**
 * Binding declaration
 */

BOOST_PYTHON_MODULE(SMRPy)
{
/**
 * Common mathematical classes
 */
  class_<SMRVector3Wrap>("SMRVector3")
    .def(init<double,double,double>())
    .def("init", &SMRVector3Wrap::init)
    .def("normalize", &SMRVector3Wrap::default_normalize)
    .def("norm", &SMRVector3Wrap::norm)
    .def("X", &SMRVector3Wrap::X)
    .def("Y", &SMRVector3Wrap::Y)
    .def("Z", &SMRVector3Wrap::Z)
    ;

  class_<SMRQuaternionWrap>("SMRQuaternion")
    .def(init<SMRVector3Wrap,double>())
    .def(init<double,double,double,double>())
    .def("getRotationAxis", &SMRQuaternionWrap::default_getRotationAxis)
    .def("getRotationAngle", &SMRQuaternionWrap::getRotationAngle)
    .def("multiply", &SMRQuaternionWrap::default_multiply)
    .def("normalize", &SMRQuaternionWrap::default_normalize)
    .def("getX", &SMRQuaternionWrap::getX)
    .def("getY", &SMRQuaternionWrap::getY)
    .def("getZ", &SMRQuaternionWrap::getZ)
    .def("getW", &SMRQuaternionWrap::getW)
    .def("toEulerAngles", &SMRQuaternionWrap::default_toEulerAngles)
  ;

/**
 * Common SMRSkeleton declarations
 */

  class_<SMRKinematicChain, SMRKinematicChainWrap, boost::noncopyable>("SMRKinematicChain", init<bool, bool, std::string>())
    .def("getName",       &SMRKinematicChainWrap::getName)
    .def("getNumjoints",  &SMRKinematicChainWrap::getNumJoints)
    .def("getJoint",      &SMRKinematicChainWrap::default_getJoint)
    .def("getJointByName",&SMRKinematicChainWrap::default_getJointByName)
    .def("insertJoint",   &SMRKinematicChainWrap::default_insertJoint)
    .def("setStartJointIndex",   &SMRKinematicChainWrap::setStartJointIndex)
  ;

  class_<SMRSkeleton, SMRSkeletonWrap, boost::noncopyable>("SMRSkeleton", init<bool, bool, std::string>())
    .def("getName",        &SMRSkeletonWrap::getName)
    .def("getNumjoints",   &SMRSkeletonWrap::getNumJoints)
    .def("getJoint",       &SMRSkeletonWrap::default_getJoint)
    .def("getJointByName", &SMRSkeletonWrap::default_getJointByName)
    .def("insertJoint",    &SMRSkeletonWrap::default_insertJoint)
    .def("isRelative",     &SMRSkeletonWrap::isRelative)
    //.def("getJointByName", &SMRSkeleton::getJointByName)
  ;

  class_<SMRDOF, SMRDOFWrap, boost::noncopyable>("SMRDOF")
    .def("setRotationAxis",  &SMRDOFWrap::default_setRotationAxis)
    .def("setRotationAngle", &SMRDOFWrap::setRotationAngle)
    .def("getRotationAngle", &SMRDOFWrap::getRotationAngle)
    .def("setUpperBound",   &SMRDOFWrap::setUpperBound)
    .def("setLowerBound",   &SMRDOFWrap::setLowerBound)
    .def("setGain",          &SMRDOFWrap::setGain)
  ;

  class_<SMRJoint, SMRJointWrap, boost::noncopyable>("SMRJoint", init<bool>())
    .def("getName", &SMRJointWrap::getName)
    .def("setName", &SMRJointWrap::setName)
    .def("setPos", &SMRJointWrap::setPos_3Doubles)
    .def("getPos", &SMRJointWrap::default_getPos)
    .def("setRot", &SMRJointWrap::setRot_3Doubles)
    .def("setRotQuat", &SMRJointWrap::setRot_Quaternion)
    .def("setEndVect", &SMRJointWrap::setEndPos_3Doubles)
    .def("getRot", &SMRJointWrap::default_getOrient)
    .def("setParentName", &SMRJointWrap::setParentName)
    .def("getParentName", &SMRJointWrap::getParentName)
    .def("setName", &SMRJointWrap::setName)
  ;
  //void (SMRKinematicJointWrap::*addDOF_Vector3)(const SMRVector3 customAxis, const double lowerBound, const double _upperBound, const double _value) = &SMRKinematicJointWrap::addDOF;

  class_<SMRKinematicJoint, SMRKinematicJointWrap, boost::noncopyable>("SMRKinematicJoint", init<bool>())
    .def("getName", &SMRKinematicJointWrap::getName)
    .def("setName", &SMRKinematicJointWrap::setName)
    .def("updateRotation", &SMRKinematicJointWrap::updateRotation)
    .def("addDOF", &SMRKinematicJointWrap::default_addDOF)
    .def("createDOF", &SMRKinematicJointWrap::default_createDOF)
    .def("getDOF", &SMRKinematicJointWrap::default_getDOF)
    .def("setPos", &SMRKinematicJointWrap::setPos_3Doubles)
    .def("setRot", &SMRKinematicJointWrap::setRot_3Doubles)
    .def("setRotQuat", &SMRKinematicJointWrap::setRot_Quaternion)
    .def("setEndVect", &SMRKinematicJointWrap::setEndPos_3Doubles)
    .def("getRot", &SMRKinematicJointWrap::default_getOrientation)
    .def("setParentName", &SMRKinematicJointWrap::setParentName)
    .def("getParentName", &SMRKinematicJointWrap::getParentName)
    .def("setName", &SMRKinematicJointWrap::setName)
  ;

/**
 *    Inverse Kinematics
 */
  class_<SMRIKConstraintWrap>("SMRIKConstraint")
    .def("setRelatedJointName", &SMRIKConstraintWrap::default_setRelatedJointName)
    .def("setOffset", &SMRIKConstraintWrap::default_setOffset)
    .def("setPosition", &SMRIKConstraintWrap::default_setPosition)
  ;

  class_<SMRGSMMSolverWrap>("SMRGSMMSolver", init<SMRKinematicChainWrap*>())
    .def("addConstraintPtr", &SMRGSMMSolverWrap::default_addConstraintPtr)
    .def("process",          &SMRGSMMSolverWrap::default_process)
    .def("getSkeleton",      &SMRGSMMSolverWrap::default_getSkeleton)
  ;

  class_<RealizerWrap>("Realizer")
    .def("addCharacter",                  &RealizerWrap::addCharacter)
    .def("run",                       &RealizerWrap::default_run)
    //.def(getSkeleton, &RealizerWrap::getSkeleton)
    //.def("addCharacterSkeleton",      &RealizerWrap::addCharacterSkeleton)
    .def("getCharacterPosture",       &RealizerWrap::default_getCharacterPosture)
    .def("addMorphTarget",            &RealizerWrap::default_addMorphTarget)
    .def("setMorphTargetWeight",      &RealizerWrap::default_setMorphTargetWeight)
    .def("getMorphTargetWeight",      &RealizerWrap::default_getMorphTargetWeight)
    .def("getModifiedMorphTargets",   &RealizerWrap::default_getModifiedMorphTargets)
    .def("addShaderParameter",        &RealizerWrap::default_addShaderParameter)
    .def("setShaderParameterWeight",  &RealizerWrap::default_setShaderParameterWeight)
    .def("getShaderParameterValue",   &RealizerWrap::default_getShaderParameterWeight)
    .def("addPoseToAnimation",        &RealizerWrap::default_addPoseToAnimation)
    .def("addAnimation",              &RealizerWrap::default_addAnimation)
    .def("init",                      &RealizerWrap::init)
    .def("configureLogger",           &RealizerWrap::configureLogger)
    .def("skeletonRequested",         &RealizerWrap::skeletonRequested)
    .def("getFeedbackMessage",        &RealizerWrap::getFeedbackMessage)
    //.def("lock",                      &RealizerWrap::lock)
    //.def("unLock",                    &RealizerWrap::unLock)
;

/**
 *   SMRMotion Class 
 */
 class_<SMRMotionWrap>("SMRMotion")
    .def("insertSkeleton",          &SMRMotionWrap::insertSkeletonToMotion)
    .def("setTimeStep",             &SMRMotionWrap::setTimeStepWrap)
 ;

/**
 *    export methods exposure
 */
  def("exportSkeletonToBvh", exportPoseToBvhWrap);

  def("InverseQuaternion", InverseWrap);
  
  def("exportMotionToBvh", exportMotionToBvhWrap);
  
  def("exportMotionToBvhWithSkeleton", exportMotionToBvhWithSkeletonWrap);
}



BOOST_PYTHON_MODULE(EMBOTS_REALIZER)
{

}

