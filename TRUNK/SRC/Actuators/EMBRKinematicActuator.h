
#ifndef _EMBRKINEMATICACTUATOR_H
#define _EMBRKINEMATICACTUATOR_H

#include "ActuatorGenericEMBR.h"

class EMBRKinematicActuator:public ActuatorGenericEMBR
{
public:
  EMBRKinematicActuator(Character* _relatedCharacter):ActuatorGenericEMBR(_relatedCharacter)
  {
    m_type = ActuatorGenericEMBR::KINEMATIC;
  };
  ~EMBRKinematicActuator(){};
  virtual SMRSkeleton * getSkeleton(float attenuationFactor=1.0f) = 0;
};

#endif
