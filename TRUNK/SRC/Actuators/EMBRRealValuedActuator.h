
#ifndef _EMBRREALVALUEDACTUATOR_H
#define _EMBRREALVALUEDACTUATOR_H

#include "ActuatorGenericEMBR.h"

#include <string>
#include "../Character.h"

class EMBRRealValuedActuator:public ActuatorGenericEMBR
{
public:
  EMBRRealValuedActuator(Character* _relatedCharacter):ActuatorGenericEMBR(_relatedCharacter)
  {
    m_type = ActuatorGenericEMBR::REAL_VALUED;
  };
  ~EMBRRealValuedActuator(){};
  virtual void process(float _relativeTime)=0;
  const RealValuedResultStruct& getRealValuedStruct() const {return m_result;}
protected:
  RealValuedResultStruct m_result;
};

#endif