
#include "ActuatorIKSlerp.h"
#include "Scheduler.h"
// why? pp   #include "Windows.h"

ActuatorIKSlerp::ActuatorIKSlerp(Character* _relatedCharacter) : EMBRKinematicActuator(_relatedCharacter), SMRGSMMSolver(),m_currentError(0.0f),m_lastError(1000.0f),m_currentGain(1.0f),m_slowStart(true),m_iterations(0.0f), m_ikAlreadyPerformed(false)
{
  m_type = ActuatorGenericEMBR::KINEMATIC;
} 
ActuatorIKSlerp::~ActuatorIKSlerp()
{
  for_each(m_constraints.begin(), m_constraints.end(), DeleteObjectPtr());
}
void ActuatorIKSlerp::computeTargetPose()
{
  unsigned int i = 0; 
  //float dError = 0.0f;

  SMRGSMMSolver::setGlobalGain(20);
  while(i < 1)
  {
    SMRGSMMSolver::process();
    i++;
    //m_currentError = (float)SMRIKSolver::getErrorNorm();
    //m_iterations += 1.0f;
    //m_lastError = m_currentError;
  }
  LOG_TRACE(ikLogger, " error for this actuator (IKSlerp) " << m_skeleton->getName() << ": " << (float)SMRIKSolver::getErrorNorm());
  m_targetPose = reinterpret_cast<SMRSkeleton*>(m_skeleton);
}

void ActuatorIKSlerp::process(float relativeTime)
{
  //if(m_lastError > 0.002f)
  //{ 
    //LARGE_INTEGER secondShot;
    //LARGE_INTEGER firstShot;
    //LARGE_INTEGER frequency;

    //if (!m_ikAlreadyPerformed)
    //{
    //  m_startPose = m_referenceSkeleton->getSubSkeleton(m_startPose);
    //}


    //QueryPerformanceCounter( &firstShot );

    //refreshSkeleton();

    //m_ikAlreadyPerformed = true;
    computeTargetPose();

    //QueryPerformanceCounter( &secondShot );
    //QueryPerformanceFrequency( &frequency );
    //unsigned long firstShotL = firstShot.LowPart;
    //unsigned long secondShotL = secondShot.LowPart;
    //secondShotL = secondShotL - firstShotL;
    //unsigned long frequencyL = frequency.LowPart;
    //double timeMS = (double)secondShotL/(double)frequencyL;
    //cout << "It took " << 1000.0*timeMS << " to the IK to complete\n";

  //}
  //m_returnPose = interpolateSkeletons(m_startPose,*m_targetPose,relativeTime);
  m_returnPose = m_targetPose->getSubSkeleton(m_startPose);
}

SMRSkeleton * ActuatorIKSlerp::getSkeleton(float attenuationFactor)
{
  return &m_returnPose;
}

void ActuatorIKSlerp::setKinematicChain(SMRKinematicChain &_chain, SMRSkeleton _stencil, SMRSkeleton *_refPose)
{
  m_skeleton = &_chain;
  m_referenceSkeleton = _refPose;
  m_startPose = _stencil;
  m_numDOFs = m_skeleton->getNumDOFs(); 
  m_numConstraints = 0;
}

void ActuatorIKSlerp::setTarget(const SMRVector3 &target, const char* _jointName, const SMRVector3 &_offset)
{
  SMRIKConstraint *constraint = new SMRIKConstraint();  // use a factory later on
  constraint->setPosition(target);
  constraint->setOffset(_offset);
  constraint->setRelatedJointName(_jointName);
  m_constraints.push_back(constraint);
  addConstraintPtr(constraint);
}

void ActuatorIKSlerp::setTarget(const char* _jointTarget, const char* _jointName, const SMRVector3 &_offset, const SMRVector3 &_rotationOffset)
{
  SMRIKConstraint *constraint = new SMRIKConstraint();  // use a factory later on
  constraint->setJointTarget(_jointTarget, _rotationOffset);
  constraint->setOffset(_offset);
  constraint->setRelatedJointName(_jointName);
  m_constraints.push_back(constraint);
  addConstraintPtr(constraint);
}
