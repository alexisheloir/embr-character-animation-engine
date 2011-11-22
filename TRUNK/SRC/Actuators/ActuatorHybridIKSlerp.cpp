
#include "ActuatorHybridIKSlerp.h"
#include "Scheduler.h"
#if(WIN32)
#include "Windows.h"
#endif

ActuatorHybridIKSlerp::ActuatorHybridIKSlerp(Character* _relatedCharacter): EMBRKinematicActuator(_relatedCharacter), m_currentError(0.0f),m_lastError(1000.0f),m_iterations(0.0f),m_slowStart(true),m_ikAlreadyPerformed(false),m_currentGain(1.0f),m_mean(0.0f),m_adjustSwivel(false),m_perfomAnalyticalIK(false),m_angle(0.0f)
{
  m_type = ActuatorGenericEMBR::KINEMATIC;
  LOG_DEBUG(logger,"constructor of ActuatorHybridIKSlerp");
}

ActuatorHybridIKSlerp::~ActuatorHybridIKSlerp()
{
  delete(m_kinematicChain);
  for_each( m_constraints.begin(), m_constraints.end(), DeleteObjectPtr());
}

void ActuatorHybridIKSlerp::setKinematicChain(const SMRKinematicChain &_chain, SMRSkeleton _stencil, SMRSkeleton *_refPose)
{
  LOG_DEBUG(ikLogger,"setkinematicChain of ActuatorHybridIKSlerp");

  m_kinematicChain = new SMRKinematicChain(_chain);


  m_iterativeIKSolver.setKinematicChain(*m_kinematicChain);
  m_analyticalIKSolver.setKinematicChain(*m_kinematicChain);
  if (m_kinematicChain->getName() == "rarm")
  {
   m_swivelActuator.setKinematicChain(*m_kinematicChain,"rhumerus","rhand","rradius");
   m_swivelActuator.setUpDirection("spine1","spine4");
  }
  else if (m_kinematicChain->getName() == "larm")
  {
   m_swivelActuator.setKinematicChain(*m_kinematicChain,"lhumerus","lhand","lradius");
   m_swivelActuator.setUpDirection("spine1","spine4");
  }

  m_referenceSkeleton = _refPose;
  m_startPose = _stencil;

  m_returnPose = (m_referenceSkeleton)->getSubSkeleton(m_startPose);

}

void ActuatorHybridIKSlerp::process(float relativeTime)
{
  //m_returnPose = m_startPose;

  if (!m_ikAlreadyPerformed)
  {
    m_startPose = m_referenceSkeleton->getSubSkeleton(m_startPose); 
    if (m_perfomAnalyticalIK)
    {
      m_analyticalIKSolver.process();
    }
    if (m_adjustSwivel)
    {
      if (m_kinematicChain->getName() == "rarm")
      {
        m_swivelActuator.setAngle(-m_angle);
      }else if( m_kinematicChain->getName() == "larm" )
      {
        m_swivelActuator.setAngle(m_angle);
      }
      if (m_kinematicChain->getName() == "rarm" || m_kinematicChain->getName() == "larm" )
        m_swivelActuator.process();
    }
	  m_ikAlreadyPerformed = true;
    m_iterativeIKSolver.computeErrorVector();
    m_currentError = (float)m_iterativeIKSolver.getErrorNorm();
  }
  unsigned int i = 0;

  if (m_mean > 8.0E-4f || m_mean == 0.0f)
  {
    while(i < 20 && m_currentError > 0.005f )
    {
      m_iterativeIKSolver.setGlobalGain(m_currentGain);
      m_iterativeIKSolver.process(); // TODO: DEBUG THIS FOR ORIENTATION CONSTRAINTS
      i++;
      m_currentError = (float)m_iterativeIKSolver.getErrorNorm();
      m_mean = (m_mean + abs(m_lastError - m_currentError))/2;
      float dError = m_lastError - m_currentError;
      if (dError < m_mean)
      {
        m_currentGain += (1.0f);
      }else
      {
        m_currentGain /= (1.2f);
      }
      m_lastError = m_currentError;
    }
  }

  //if (m_currentError > 0.001f && m_mean > 8.0E-4f) LOG_DEBUG(ikLogger, " error for this actuator (" << m_kinematicChain->getName() << "): " << m_iterativeIKSolver.getErrorNorm());
  LOG_DEBUG(ikLogger, " error for this actuator (HybridIKSlerp) (" << m_kinematicChain->getName() << "): " << m_iterativeIKSolver.getErrorNorm());
  if (m_feedbackRequired && relativeTime > 0.99 && m_timeForFeedback)
  {
     stringstream ss;
     ss << "error value: " << m_currentError;
     m_feedbackMessage.feedback = ss.str();
     UDPFeedbackSender::getInstance()->sendFeedbackMessage(feedbackMessageToString());
     m_feedbackRequired = false;
  }

  m_targetPose = reinterpret_cast<SMRSkeleton*>(m_kinematicChain);

  m_returnPose = interpolateSkeletons(m_startPose,*m_targetPose,relativeTime); 
}

SMRSkeleton * ActuatorHybridIKSlerp::getSkeleton(float attenuationFactor)
{
  return &m_returnPose;
}

void ActuatorHybridIKSlerp::setTarget(const SMRVector3 &target, const char* _jointName, const SMRVector3 &_offset)
{
  SMRIKConstraint *constraint = new SMRIKConstraint();  // use a factory later on
  constraint->setPosition(target);
  constraint->setOffset(_offset);
  constraint->setRelatedJointName(_jointName);
  m_constraints.push_back(constraint);

  if (string(_jointName) == (m_kinematicChain->getJoint(m_kinematicChain->getNumJoints()-1))->getName() )
  {
    m_analyticalIKSolver.addConstraintPtr(constraint);
    m_perfomAnalyticalIK = true;
  }
  m_iterativeIKSolver.addConstraintPtr(constraint);

  //computeTargetPose();
}

void ActuatorHybridIKSlerp::setTarget(const char* _jointTarget, const char* _jointName, const SMRVector3 &_offset, const SMRVector3 &_rotationOffset)
{
  SMRIKConstraint *constraint = new SMRIKConstraint();  // use a factory later on
  constraint->setJointTarget(_jointTarget, _rotationOffset);
  constraint->setOffset(_offset);
  constraint->setRelatedJointName(_jointName);
  m_constraints.push_back(constraint);
  //if related joint is the last one, enable analitical solver
  if (string(_jointName) == (m_kinematicChain->getJoint(m_kinematicChain->getNumJoints()-1))->getName() && m_kinematicChain->getName().find("arm") != string::npos  ) // hack to limit IK to arms
  {
    m_analyticalIKSolver.addConstraintPtr(constraint);
    m_perfomAnalyticalIK = true;
  }
  m_iterativeIKSolver.addConstraintPtr(constraint);

  //computeTargetPose();
}

void ActuatorHybridIKSlerp::flushConstraintPtrs()
{
  m_analyticalIKSolver.flushConstraintPtrs();
  m_iterativeIKSolver.flushConstraintPtrs();
}

SMRIKConstraint* ActuatorHybridIKSlerp::getConstraintPtr(const unsigned int index) 
{
  return m_constraints.at(index);
}