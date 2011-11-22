#ifndef _CORRELATIONMAP_H
#define _CORRELATIONMAP_H

#include "../ActuatorFactory.h"

class CorrelationMap: public ComplexMotionSegment {
public:
  typedef enum ActuatorTypeEnum{AUTONOMOUSPARAMETER, JOINTPOSITION, CORRELATIONMAP} InputType;

protected:
  const SMRSkeleton *m_referenceSkeleton;
  SMRJoint* m_sourceJoint;                      /** this joint will drive the correlation map */
  SMRKinematicChain m_kinematicJoint;           /** a kinematic chain which only contains one joint */
  SMRSkeleton m_referenceChain;
  InputType m_inputType;
  string m_autonomousParameterKey;
  CorrelationMap *m_sourceCorrelationMap;
  float m_value;

public:
  CorrelationMap(Character *_relatedCharacter, float _relative_fade_in=0.0f, float _relative_fade_out=1.0f);
  void process(const unsigned int _absoluteTimeMs, bool _timeForFeedback = true);
  void setAutonomousParameterKey(string _autonomousParameterKey);
  void setReferenceSkeleton(SMRSkeleton *_referenceSkeleton);
  void setSourceJoint(string _sourceJointName);
  void setInputCorrelationMap(CorrelationMap *_inputCorrelationMap);
  float getValue();
};

#endif
