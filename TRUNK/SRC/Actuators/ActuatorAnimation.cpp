
#include "ActuatorAnimation.h"
#include "Scheduler.h"
// ? why  #include "Windows.h"

ActuatorAnimation::ActuatorAnimation(Character *_relatedCharacter, SMRMotion* motion,const SMRSkeleton &_stencil,const SMRSkeleton *_referencePose, bool _isDerivative, string _weightKey):SMRMotionPlayer(motion), EMBRKinematicActuator(_relatedCharacter), m_fade_in(0),m_fade_out(0),m_alpha(1.0f), m_startPose(_stencil), m_neutralPose(_stencil),  m_firstTimeProcess(false), m_weightKey(_weightKey), m_lastRelativeTime(0.0f),m_isDerivative(_isDerivative)
{
  m_type = ActuatorGenericEMBR::ANIMATION;
  m_referenceSkeleton = _referencePose;
  if (m_isDerivative)
  {
    for(unsigned int i=0; i< m_neutralPose.getNumJoints(); i++)
    {
      SMRQuaternion id;
      id.identity();
      m_neutralPose.getJoint(i)->setOrientation(id);
    }
    m_returnPose =  (m_neutralPose);
    m_cumuledDrift = (m_neutralPose);
  }else
  {
    m_returnPose = (m_referenceSkeleton)->getSubSkeleton(m_startPose);
  }

  SMRMotionPlayer::setInterpolationMode(true);
  //SMRMotionPlayer::m_motion->derivate();
}
ActuatorAnimation::~ActuatorAnimation()
{
}

void ActuatorAnimation::process(float _relativeTime)
{
  LOG_TRACE(logger,"processing motion player with relative time: " << _relativeTime << " fade_in is " << m_fade_in);
  if (!m_firstTimeProcess && !m_isDerivative) 
  {
    m_startPose = m_referenceSkeleton->getSubSkeleton(m_startPose);
    m_firstTimeProcess = true;
  }
  if (_relativeTime < m_fade_in) // progressive appearance of first frame
  {
    float alpha = _relativeTime / m_fade_in;
    LOG_TRACE(logger,"animation processing relativetime (fade in phase)" << _relativeTime);
    SMRMotionPlayer::process(_relativeTime);
    m_returnPose = interpolateSkeletons(m_startPose,(SMRMotionPlayer::getSkeleton())->getSubSkeleton(m_startPose),alpha);
  }else
  {
    LOG_TRACE(logger, "animation processing relativetime" << _relativeTime);
    SMRMotionPlayer::process(_relativeTime);
    m_returnPose = (SMRMotionPlayer::getSkeleton())->getSubSkeleton(m_startPose);
    if ( m_isDerivative )
    {
      if (m_lastRelativeTime > _relativeTime)//animation has looped
      {
        m_cumuledDrift.inverse();
        m_returnPose = (m_cumuledDrift)*=(m_returnPose);
        m_cumuledDrift=(SMRMotionPlayer::getSkeleton())->getSubSkeleton(m_startPose);
        if (m_weightKey != "") m_alpha = m_relatedCharacter->getAutonomousParameterValue(m_weightKey);
      }else
      {
        m_cumuledDrift*=m_returnPose;
      }
    }else if (m_weightKey != "")
    {
      m_alpha = m_relatedCharacter->getAutonomousParameterValue(m_weightKey);
    }
    m_lastRelativeTime = _relativeTime;
    //cout << (SMRMotionPlayer::getSkeleton())->getJoint(0)->getPosition();

    //for(unsigned int i=0; i<m_returnPose.getNumJoints(); i++ )
    //{
    //  cout << m_returnPose.getJoint(i)->getOrientation() << endl;
    //}
  }
  //if(!m_returnPose.getJoint(0)->hasParent())//first joint is root joint
  //{
  //  SMRVector3 relativePosition = m_returnPose.getJoint(0)->getPosition();
  //  SMRVector3 currentPosition = m_referenceSkeleton->getJoint(0)->getPosition();
  //  m_returnPose.getJoint(0)->setPosition(currentPosition+relativePosition);
  //  //cout << currentPosition+relativePosition << endl;
  //}
}


SMRSkeleton * ActuatorAnimation::getSkeleton(float attenuationFactor)
{
  //float alpha = 1.0;
  //if (m_weightKey != "")
  //{
  //  alpha = ActuatorFactory::getInstance()->getAutonomousParameterValue(m_weightKey);
  //  //if ( alpha == -1.0f)
  //  //  alpha = 1.0f;
  //}
  interpolateSkeletons(m_neutralPose,m_returnPose,&m_returnPose,m_alpha*attenuationFactor);
  //for(unsigned int i=0; i<m_returnPose.getNumJoints(); i++ )
  //{
  //  cout << alpha << endl;
  //  cout << m_returnPose.getJoint(i)->getOrientation() << endl;
  //}
  return &m_returnPose;
}

void ActuatorAnimation::setEaseIn(float _ease_in)
{
  m_fade_in = _ease_in;
}

void ActuatorAnimation::setAlpha(float _alpha)
{
  m_alpha = _alpha;
}
