#include "GestureModifier.h"

GestureModifier::GestureModifier(float init_v_s=0.0f,float init_v_t=0.0f,float init_v_p=0.0f,float init_v_f=0.0f)
    :m_v_s(init_v_s), m_v_t(init_v_t),m_v_p(init_v_p),m_v_f(init_v_f)
  {
    if (m_v_t< 0.0) m_v_t*= 0.6f;
    m_ten = init_v_p;
    m_con = init_v_f;
    m_bia = init_v_p;  
  }
    


GestureModifier::~GestureModifier()
  {
  }

void GestureModifier::setSpatialExtent(float _v_s)
{
  if( _v_s < -1.0f)
  {
    m_v_s = -1.0f;
  }else
  { 
    if (_v_s > 1.0f)
    {
      m_v_s = 1.0f;
    }else 
    {
      m_v_s = _v_s;
    }
  }
}


void GestureModifier::setTemporalExtent(float _v_t)
{
  if ( _v_t < -1.0f)
  {
    m_v_t = -1.0f;
  }else
  { 
    if (_v_t > 1.0f)
    {
      m_v_t = 1.0f;
    }else 
    {
      m_v_t = _v_t;
    }
  }
}

void GestureModifier::setPower(float _v_p)
{
  if ( _v_p < -1.0f)
  {
    m_v_p = -1.0f;
  }else
  { 
    if (_v_p > 1.0f)
    {
      m_v_p = 1.0f;
    }else
    {
      m_v_p = _v_p;
    }
  }
}

void GestureModifier::setFluidity(float _v_f)
{
  if ( _v_f < -1.0f)
  {
    m_v_f = -1.0f;
  }else
  { 
    if (_v_f > 1.0f)
    {
      m_v_f = 1.0f;
    }else
    {
      m_v_f = _v_f;
    }
  }
}

int GestureModifier::temporalExtent(int t0, int t1)
{
  int timeSpan =t1 - t0;
  // most movements from k-pose to k-pose will be shorter then 5 seconds
    
  double h =  std::log((  ((double)timeSpan) /1000.0)+ 1.0) *1000.0;
    
  int delay = (int) (m_v_t * 1.5* h) ;
    
  timeSpan = timeSpan + delay;// +(m_v_t * ((float) std::log(((timeSpan/1000) *2.0 )+ 1)) * 1000);   // 
  // TODO: this should be dependent on distance!
  // t0 += (int) timeSpan; // duration should not be lower than 100ms    
  return delay;
}
 
//int GestureModifier::calcPreGestureLeft(SMRVector3 &_prePos)
//{ 
//  int additionalTimePoint = 0;
//  if( m_v_p >0)
//  {
//    deque<pair<int,SMRVector3>>::iterator queIt;
//    if (!leftGesture.empty()) 
//    {
//      queIt = leftGesture.begin();
//      _prePos = (*queIt).second;
//      additionalTimePoint = queIt->first;
//      queIt++;
//      if (queIt !=  leftGesture.end())
//      {
//        _prePos -= (*queIt).second;
//        additionalTimePoint -= ((int)m_v_p) * (queIt->first);
//        _prePos.normalize();
//        _prePos = (leftGesture.front()).second+ ( _prePos * m_v_p);
//
//        LOG_WARN(modifierLogger,"additional PrePosleft is: "<<_prePos.X()<<";"<<_prePos.Y()<<";"<<_prePos.Z()<<"at "<<additionalTimePoint<<endl);
//       
//      }else
//      {
//        LOG_DEBUG(modifierLogger,"Only one pose contained in the nucleus, not able to compute a path!");
//      }
//    }
//  }
//  return additionalTimePoint;
//}
//  
//
//int GestureModifier::calcPreGestureRight(SMRVector3 &_prePos)
//{
//  int additionalTimePoint = 0;
//  if(m_v_p >0)
//  {
//    deque<pair<int,SMRVector3>>::iterator queIt;
//    if (!rightGesture.empty()) 
//    {
//      queIt = rightGesture.begin();
//      _prePos = (*queIt).second;
//      additionalTimePoint = queIt->first;
//      queIt++;
//      if (queIt !=  rightGesture.end())
//      {
//        _prePos -= (*queIt).second;
//        additionalTimePoint -= ((int) m_v_p) * (queIt->first);
//        _prePos.normalize();
//        _prePos = (rightGesture.front()).second+(_prePos * m_v_p);
//        LOG_WARN(modifierLogger,"additional PrePosright is: "<<_prePos.X()<<";"<<_prePos.Y()<<";"<<_prePos.Z()<<"at "<<additionalTimePoint<<endl);
//      }else
//      {
//        LOG_INFO(modifierLogger,"Only one pose contained in the nucleus, not able to compute a path!");
//      }
//    }
//  }
//  return additionalTimePoint;
//}


/* spatial modification function, changes gesture volume according to v_s [-1,1] small to big
*   _shoulder : coordinates of the humerus joint
*   _bodyPart : l or r, horizontal offsets must be added or substracted to the old position
*/

void GestureModifier::spatialMod(SMRVector3 &origin, const SMRVector3 & _shoulder, const char & _bodyPart){
 
  if (m_v_s != 0.0)
  {
    LOG_INFO(modifierLogger,"old TARGET: "<<origin.X()<<";"<<origin.Y()<<";"<<origin.Z());
    double l = 1.0;
    if (_bodyPart == 'r' ) l = -1.0;
    double twoPi = 2.0 * M_PI;
    double piHalf = M_PI / 2.0;
    

    const double xDiff = _shoulder.X()-origin.X();
    const double yDiff = _shoulder.Y()-origin.Y();
    const double zDiff = _shoulder.Z()-origin.Z();

    const double axDiff = abs(xDiff);
    const double ayDiff = abs(yDiff);
    const double azDiff = abs(zDiff);
    LOG_TRACE(modifierLogger,"bodyPart is :"<<_bodyPart);
    LOG_TRACE(modifierLogger,"shoulderPos: "<<_shoulder.X()<<";"<<_shoulder.Y()<<";"<<_shoulder.Z());
    LOG_TRACE(modifierLogger,"diff: "<<xDiff<<";"<<yDiff<<";"<<zDiff<<endl);
    
    const double angleDiff = m_v_s * (M_PI /8.0);

    //horizontal angle
    double alpha = std::atan2(2.5 * ayDiff ,axDiff);
    
    
    //vertical angle
    double beta = std::atan2(2.5 * axDiff,azDiff);


    //sagital angle
    double gamma = std::atan2(2.5 * azDiff,ayDiff);


   LOG_TRACE(modifierLogger,"alpha: "<<alpha<<"beta: "<<beta<<"gamma: "<<gamma<<endl);
   if (xDiff<0.0) 
   {
     if (yDiff >0.0)alpha = M_PI - alpha;
     else alpha +=M_PI;
   }else if (yDiff <0.0) alpha = twoPi -alpha;

    if (zDiff<0.0) 
      {
        if (xDiff <0.0)beta = M_PI - beta;
        else beta +=M_PI;
      }
    else if (xDiff >0.0) beta = twoPi -beta;

    if (yDiff <0.0)
    {
      if (zDiff<0.0) gamma =M_PI -gamma;
      else gamma += M_PI;
   }else if(zDiff>0.0) gamma = twoPi-gamma;

    LOG_TRACE(modifierLogger,"corrected alpha: "<<alpha);
    LOG_TRACE(modifierLogger,"corrected beta: "<<beta);
    LOG_TRACE(modifierLogger,"corrected gamma: "<<gamma);
  
    double alphaRad =  (xDiff  / (std::cos(alpha)));
    double betaRad  = (zDiff  / (std::cos(beta )));
    double gammaRad = (yDiff  / (std::cos(gamma)));

    LOG_TRACE(modifierLogger,"rads: "<<alphaRad<<" "<<betaRad<<" "<<gammaRad);
    //open or close arms
    if (m_v_s < 0.0)
    {
      if (alpha < M_PI) 
      {
        alpha = max((alpha +l * angleDiff) , 0.0);
     }else
      {
        alpha = min((alpha - l* angleDiff) , twoPi);
       }
      if (beta < M_PI)
      {
        if (beta < piHalf)
        {
          beta = min((beta -  angleDiff),piHalf);
       }else
        {
          beta = max((beta + angleDiff),piHalf);
        }
     }else
      {
        if (beta < (3.0* piHalf))
        {
         beta = min((beta - angleDiff),3.0* piHalf);
       }else
          {
          beta = max((beta +angleDiff),3.0* piHalf);
         }
      }
      if (gamma < M_PI)
      {
        if (gamma < piHalf) 
          { 
             gamma = max((gamma + 0.3* angleDiff),0.0);
          }
        else
          {
           gamma = min((gamma- 0.3*angleDiff),M_PI);
          }
     }else
      {
        if (gamma <(3.0*piHalf)) 
          {
           gamma = max((gamma + 0.3*angleDiff), M_PI);
          }
        else 
          {
          gamma = min((gamma -  0.3*angleDiff),twoPi);
          }
       }
  }else
    {
      if (alpha < M_PI)
      {
        alpha = min((alpha + l*angleDiff) , M_PI);
     }else
      {
        alpha = max((alpha - l* angleDiff) , M_PI);
       }
      if (beta < M_PI)
      {
        if (beta < piHalf)
        {
          beta = max((beta - angleDiff),0.0);
       }else
        {
          beta = min((beta + angleDiff),M_PI);
        }
     }else
      {
        if (beta < (3.0* piHalf))
        {
         beta = max((beta - angleDiff),M_PI);
       }else
          {
          beta = min((beta + angleDiff),twoPi);
         }
      }
      if (gamma < M_PI)
      {
        if (gamma < piHalf) 
        { 
           gamma = min((gamma +  0.3*angleDiff),piHalf);
       }else
          {
           gamma = max((gamma -  0.3*angleDiff),piHalf);
          }
     }else
      {
        if (gamma <(3*piHalf)) 
        {
           gamma = min((gamma + 0.3* angleDiff), 3.0*piHalf);
        }
        else 
          {
          gamma = max((gamma - 0.3*angleDiff),3.0*piHalf);
          }
       }
     }
    
    LOG_TRACE(modifierLogger,"new alpha: "<<alpha<<"new beta: "<<beta<<"new gamma: "<<gamma<<endl);
 
    

    double hx =   xDiff -(alphaRad *(std::cos(alpha)));
    
    double hy =   yDiff - alphaRad * (std::sin(alpha)/ 2.5);

    double vz =  zDiff - (betaRad *(std::cos(beta)));

    double vx =  xDiff + betaRad *(std::sin(beta)/2.5);

    double sy =  yDiff - (gammaRad * std::cos(gamma));
    
    double sz = zDiff + gammaRad *(std::sin(gamma)/2.5);
    
    //for the right arm, offsets must be inverted


    LOG_TRACE(modifierLogger,"hx: "<<hx<<" hy: "<<hy);
    LOG_TRACE(modifierLogger,"vz: "<<vz<<" vx: "<<vx);
    LOG_TRACE(modifierLogger,"sy: "<<sy<<" sz: "<<sz<<endl);

    
    origin.m_z += vz + sz;
    origin.m_x += hx + vx;
    origin.m_y -= sy + hy;

    //correct targets to avoid collision with hand or body
    if (origin.m_z < 0.65 && (origin.X() * l) <0.2) origin.m_y = min (origin.Y(), -0.15); 
    
    
    if ((origin.X()* l) <0.05 && m_v_s < 0.0) origin.m_x = l * 0.05;
    
    LOG_INFO(modifierLogger,"new TARGET: "<<origin.X()<<";"<<origin.Y()<<";"<<origin.Z()<<endl);
  }
}
   
 /** Scaling function around 0.0f, -0.22f, 0.28f
 **   gets original vector and returns new calculated values
 **/

void GestureModifier::spatialExtent(SMRVector3 &origin)
{
  if ( m_v_s != 0.0f)
  {
    const SMRVector3 cOGS = SMRVector3(0.0f, -0.22f, 0.28f);
    LOG_DEBUG(modifierLogger,origin.m_x<<";"<<origin.m_y<<";"<<origin.m_z<<"is the old target");
    float scaleFactor = 1 + (m_v_s * 0.35f); 
    origin = cOGS + ((origin - cOGS).operator *(scaleFactor));
    LOG_DEBUG(modifierLogger,origin.m_x<<";"<<origin.m_y<<";"<<origin.m_z<<"is the NEW target");
  }
}

/* translates _b along orthogonal vector to vector through _a and _c
 * @param _a point x_n-1
 * @param _b modified point x_n
 * @param _c point x_n+1
 */
void GestureModifier::spatialExtent(const SMRVector3 &  _a,SMRVector3 &  _b, const SMRVector3  _c)
{
  if (m_v_s == 0.0) 
    return;   // no spatial extent to apply
  
  if ( _a == _c)
  {
    // move b on connection line between a/c and b
    _b = (_b - _a) * m_v_s + _b;
  }
  else
  {
    SMRVector3 AC, HB;
    AC = _c - _a;
    //compute orthogonal vector
    SMRVector3 HA  = DotProduct((_b - _a),AC )* AC;
    HB = (_b - _a) + HA; 
    LOG_INFO(modifierLogger,"shift target in direction: "<<HB.X()<<";"<<HB.Y()<<";"<<HB.Z());
    //move keypoint in direction of this vector
    _b = HB * static_cast<float> ((m_v_s)) + _b;
  }  
}


/* Runs over a vector of MotionSegments, updating position constraint according to spatial extent settings 
 * decides what method is used, scaleMod for unique nucleus positions, spatialExt otherwise 
 */
void GestureModifier::spatialExtent(vector<MotionSegment *> & _motionSegments) //, GestureModifier* _gestureMod)
{
  SMRVector3 xnm1, xn,xn_old, xnp1;
  //find first pose of the nucleus
  vector<MotionSegment *>::iterator motionSegIt = _motionSegments.begin();
  while ( motionSegIt < _motionSegments.end() && (*motionSegIt)->getGestureModifier() == NULL )
  {
    motionSegIt++;
  }
  if ( motionSegIt == _motionSegments.end())
    return; //no nucleus assigned to this gesture,REMARK:set a "global" flag and avoid calling this function!!
 
  //first key pose:
  ActuatorHybridIKSlerp * actuator_xnm1 = static_cast<ActuatorHybridIKSlerp*>((*motionSegIt)->getActuator(0));
  SMRIKConstraint* constraintPtr_xnm1 = actuator_xnm1->getConstraintPtr(0);
  xnm1 = constraintPtr_xnm1->getPosition();
  MotionSegment * lastMoSeg = (*motionSegIt);
  motionSegIt++;
  
  if ( motionSegIt == _motionSegments.end()  || (*motionSegIt)->getGestureModifier() == NULL )
  {
    //only one pose in the nucleus
    (lastMoSeg->getGestureModifier())->getPointee()->spatialExtent(xnm1);
    constraintPtr_xnm1->setPosition(xnm1);
    return;
  } 
  ActuatorHybridIKSlerp *actuator_xn, *actuator_xnp1;
  actuator_xn = static_cast<ActuatorHybridIKSlerp*>((*motionSegIt)->getActuator(0));
  SMRIKConstraint * constraintPtr_xn,*constraintPtr_xnp1;
  constraintPtr_xn = actuator_xn->getConstraintPtr((unsigned int)0);
  xn = constraintPtr_xn->getPosition();
  xn_old = xn;
  lastMoSeg = (*motionSegIt);
  motionSegIt++; 
  if ( motionSegIt == _motionSegments.end() || (*motionSegIt)->getGestureModifier() == NULL)
  {
    //only two poses in the nucleus, move second point along vector xnm1->xn
    constraintPtr_xn->setPosition(xn +( (0.5 *(lastMoSeg->getGestureModifier())->getPointee()->getSpatialExtent()) * (xn-xnm1)));
    return;
  }  
  //main loop applying spatial extent to the nucleus 
  while (motionSegIt != _motionSegments.end() && (*motionSegIt)->getGestureModifier() !=NULL)//lastMoSeg->getGestureModifier() !=NULL )
  {  
    actuator_xnp1 = static_cast<ActuatorHybridIKSlerp*>((*motionSegIt)->getActuator(0));
    constraintPtr_xnp1 = actuator_xnp1->getConstraintPtr(0);
    xnp1 = constraintPtr_xnp1->getPosition();
    LOG_INFO(modifierLogger,"old target: "<<xn.X()<<","<<xn.Y()<<";"<<xn.Z());
    lastMoSeg->getGestureModifier()->getPointee()->spatialExtent(xnm1,xn,xnp1);
    LOG_INFO(modifierLogger,"new target: "<<xn.X()<<","<<xn.Y()<<";"<<xn.Z()<<endl);
    constraintPtr_xn->setPosition(xn);
    //prepare next round
    xnm1 = xn_old;
    xn = xnp1;
    xn_old = xn;
    constraintPtr_xnm1 = constraintPtr_xn;
    constraintPtr_xn = constraintPtr_xnp1;
    lastMoSeg = (*motionSegIt);
    motionSegIt++;
  }
  //modify last key pose of the nucleus
  if (lastMoSeg->getGestureModifier() !=NULL)
  constraintPtr_xn->setPosition(xn + ( (xn-xnm1) * static_cast<float>(((lastMoSeg->getGestureModifier())->getPointee()->getSpatialExtent()))));
  return;
}

/* extents spatial dimension by scaling the motion bounding box
*/
void GestureModifier::spatialExtent(vector<MotionSegment *> & _motionSegments, const SMRVector3 & _lowerBoxBound, const SMRVector3 & _higherBoxBound)
{
  if (m_v_s == 0.0) return; // no spatial extent to apply, return
  vector<MotionSegment *>::iterator motionSegIt = _motionSegments.begin();
  while ( motionSegIt < _motionSegments.end() && (*motionSegIt)->getGestureModifier() == NULL )
  {
    motionSegIt++;
  }
  if ( motionSegIt == _motionSegments.end())
    return; //no nucleus assigned to this gesture
 
  //first key pose:
  SMRIKConstraint* constraintPtr = (static_cast<ActuatorHybridIKSlerp*>((*motionSegIt)->getActuator(0)))->getConstraintPtr(0);  
  SMRVector3 currentKeyPoint = constraintPtr->getPosition();
  motionSegIt++;
  if ( motionSegIt == _motionSegments.end()  || (*motionSegIt)->getGestureModifier() == NULL )
  {
    //only one pose in the nucleus
    ((*motionSegIt)->getGestureModifier())->getPointee()->spatialExtent(currentKeyPoint);
    constraintPtr->setPosition(currentKeyPoint);
    return;
  }
  SMRVector3 boundingBoxCenter =  (_lowerBoxBound + _higherBoxBound) * 0.5f;
  const float scaleFactor = m_v_s * 0.8f;
  constraintPtr->setPosition(currentKeyPoint + ((currentKeyPoint-boundingBoxCenter)* scaleFactor));
  constraintPtr = (static_cast<ActuatorHybridIKSlerp*>((*motionSegIt)->getActuator(0)))->getConstraintPtr(0); 
  currentKeyPoint = constraintPtr->getPosition();
  constraintPtr->setPosition(currentKeyPoint + ((currentKeyPoint-boundingBoxCenter)* scaleFactor));
  motionSegIt++;
  while ( motionSegIt < _motionSegments.end() && (*motionSegIt)->getGestureModifier() != NULL )
  {
    constraintPtr = (static_cast<ActuatorHybridIKSlerp*>((*motionSegIt)->getActuator(0)))->getConstraintPtr(0); 
    currentKeyPoint = constraintPtr->getPosition();
    constraintPtr->setPosition(currentKeyPoint + ((currentKeyPoint-boundingBoxCenter)* scaleFactor));
    motionSegIt++;  
  }
}  

void GestureModifier::computeBoundingBox(vector<MotionSegment *> & _motionSegments, SMRVector3 & _lowerBoxBound, SMRVector3 & _higherBoxBound)
{
  vector<MotionSegment *>::iterator motionSegIt = _motionSegments.begin();
  while ( motionSegIt < _motionSegments.end() && (*motionSegIt)->getGestureModifier() == NULL )
  {
    motionSegIt++;
  }
  if ( motionSegIt == _motionSegments.end())
    return; //no nucleus assigned to this gesture, just for security reasons, I think this function is not called in that case
  
  //first key pose:
  _higherBoxBound = (static_cast<ActuatorHybridIKSlerp*>((*motionSegIt)->getActuator(0)))->getConstraintPtr(0)->getPosition();
  _lowerBoxBound = _higherBoxBound;
  motionSegIt++;
  SMRVector3 currentKeyPoint;
  while (motionSegIt != _motionSegments.end() && (*motionSegIt)->getGestureModifier() !=NULL)
  {
    currentKeyPoint = (static_cast<ActuatorHybridIKSlerp*>((*motionSegIt)->getActuator(0)))->getConstraintPtr(0)->getPosition();
    _lowerBoxBound.m_x = min(_lowerBoxBound.X(),currentKeyPoint.X());
    _lowerBoxBound.m_y = min(_lowerBoxBound.Y(),currentKeyPoint.Y());
    _lowerBoxBound.m_z = min(_lowerBoxBound.Z(),currentKeyPoint.Z());
    _higherBoxBound.m_x = max(_higherBoxBound.X(),currentKeyPoint.X());
    _higherBoxBound.m_y = max(_higherBoxBound.Y(),currentKeyPoint.Y());
    _higherBoxBound.m_z = max(_higherBoxBound.Z(),currentKeyPoint.Z());
    motionSegIt++;
  }
  LOG_INFO(modifierLogger,"BoundingBox is: "<<_lowerBoxBound.X()<<";"<<_lowerBoxBound.Y()<<";"<<_lowerBoxBound.Z()\
                                            <<" - "<<_higherBoxBound.X()<<";"<<_higherBoxBound.Y()<<";"<<_higherBoxBound.Z());
}

/** modifies gesture appearancy according to POWER settings, move first key pose, changes hold durations
 *param _motionSegments key poses of the sequence 
 *param _nucleusStart first pose of the nucleus at this position
 **/
void GestureModifier::applyPower(vector<MotionSegment *> & _motionSegments, unsigned int _nucleusStart)
{
  float power = _motionSegments[_nucleusStart]->getGestureModifier()->getPointee()->getPower();
  if (power == 0) return;
  if (power > 0 )
  { 
    unsigned int holdTime = static_cast<unsigned int> ( 300 * floor(power));
    unsigned int stopTime = _motionSegments[_nucleusStart]->getAbsoluteStopTime();
    _motionSegments[_nucleusStart]->setHoldDuration(_motionSegments[_nucleusStart]->getHoldDuration() + holdTime);
    if (_motionSegments.size() > (_nucleusStart +1) ) 
    {
      unsigned int nextStopTime = _motionSegments[_nucleusStart + 1]->getAbsoluteStopTime();
      unsigned int nextHoldTime = _motionSegments[_nucleusStart + 1]->getHoldDuration();
      
      if ((nextStopTime-nextHoldTime) < (stopTime + holdTime))    
      {
        if (nextHoldTime >0 )
        {  
          nextHoldTime -= holdTime;
          if (nextHoldTime >= 0)
          {
            _motionSegments[_nucleusStart + 1]->setHoldDuration(nextHoldTime);
            _motionSegments[_nucleusStart]->setAbsoluteStopTime(stopTime + holdTime);
            
          }else
          {
            _motionSegments[_nucleusStart + 1]->setAbsoluteStopTime(nextStopTime - nextHoldTime +100);
            _motionSegments[_nucleusStart + 1]->setHoldDuration(0);
          }
        }else
        {
           _motionSegments[_nucleusStart + 1]->setAbsoluteStopTime(stopTime + holdTime);
           _motionSegments[_nucleusStart]->setAbsoluteStopTime(nextStopTime);
        }
      }else
        _motionSegments[_nucleusStart]->setAbsoluteStopTime(stopTime + holdTime);
    }else 
      _motionSegments[_nucleusStart]->setAbsoluteStopTime(stopTime + holdTime);   
    SMRIKConstraint* constraintPtr = static_cast<ActuatorHybridIKSlerp*>(((_motionSegments[_nucleusStart]->getActuator(0))))->getConstraintPtr(0);
    SMRVector3 currentKeyPoint = constraintPtr->getPosition();
    SMRVector3 nextKeyPoint = (static_cast<ActuatorHybridIKSlerp*>((_motionSegments[_nucleusStart+1])->getActuator(0)))->getConstraintPtr(0)->getPosition();
    constraintPtr->setPosition(currentKeyPoint +((currentKeyPoint - nextKeyPoint).normalize() * (0.1f * power )));
  }else
  {
    unsigned int holdTime = _motionSegments[_nucleusStart]->getHoldDuration();
    if (holdTime > 0 ) 
    {
      unsigned int newHoldTime = static_cast<unsigned int> (max(0.0f,holdTime + (power * holdTime)));
      _motionSegments[_nucleusStart]->setHoldDuration(newHoldTime);
      _motionSegments[_nucleusStart]->setAbsoluteStopTime(_motionSegments[_nucleusStart]->getAbsoluteStopTime() - (holdTime -newHoldTime ));
    }
  }
}

void GestureModifier::applyTemporalExtent(vector<MotionSegment *> & _motionSegments,unsigned int & _delay, unsigned int _nucleusStart, unsigned int _nucleusEnd)
{
  if (m_v_t == 0.0) return;
  int delay = 0;
  _nucleusStart++;
  while ( _nucleusStart <= _nucleusEnd )
  {
    //update timing and calculate new delay
    LOG_INFO(modifierLogger,"---------------------------------");
    LOG_INFO(modifierLogger,"old timing:"<<_motionSegments[_nucleusStart]->getAbsoluteStartTime()<<"-"<<_motionSegments[_nucleusStart]->getAbsoluteStopTime()<<"-"<<_motionSegments[_nucleusStart]->getHoldDuration());
    int start = _motionSegments[_nucleusStart]->getAbsoluteStartTime();
    int stop  = _motionSegments[_nucleusStart]->getAbsoluteStopTime();
    _motionSegments[_nucleusStart]->setAbsoluteStartTime(start+delay);
    delay += this->temporalExtent(start,stop);
    
    _motionSegments[_nucleusStart]->setAbsoluteStopTime(stop+delay);
    
    int hold = _motionSegments[_nucleusStart]->getHoldDuration();
    double h =  std::log((  ((double)hold) /1000.0)+ 1.0) *1000.0;
    hold += (int) (m_v_t * 1.5* h) ;
    
    _motionSegments[_nucleusStart]->setHoldDuration(hold);
    LOG_INFO(modifierLogger,"new timing:"<<_motionSegments[_nucleusStart]->getAbsoluteStartTime()<<"-"<<_motionSegments[_nucleusStart]->getAbsoluteStopTime()<<"-"<<_motionSegments[_nucleusStart]->getHoldDuration());
  
    _nucleusStart++; 
  }
  while (_nucleusStart < _motionSegments.size())
  {
    //update timing for post nucleus poses
    _motionSegments[_nucleusStart]->setAbsoluteStartTime(_motionSegments[_nucleusStart]->getAbsoluteStartTime()+delay);
    _motionSegments[_nucleusStart]->setAbsoluteStopTime(_motionSegments[_nucleusStart]->getAbsoluteStopTime()+delay);
    _nucleusStart++;    
  }
  _delay = delay; 
}
  
void GestureModifier::interpolateBias(float & _bias, unsigned int _position, unsigned int _steps)
{
  _bias = _bias + ((-_bias + _bias)/ (_steps)) * ( _position / _steps);   
}

