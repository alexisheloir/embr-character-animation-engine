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

#ifndef _GESTUREMODIFIER_H
#define _GESTUREMODIFIER_H

#define _USE_MATH_DEFINES

#include <cmath>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <deque>

#include "SmrVector3.h"
#include "SmrQuaternion.h"

#include "SmrSkeleton.h"
#include "SmrIKConstraint.h"
#include "SmrJoint.h"
#include "ActuatorHybridIKSlerp.h"

#include "logger.h"
#include "EMBRLoggers.h"
#include "MotionSegment.h"

//class MotionSegment;

class GestureModifier {

private:
    float m_v_s;   //spatial extent
    float m_v_t;   // temporal extent
    float m_v_p;  // power value 
    float m_v_f;  // fluidity value
    
    //spline interpolation parameters
    float m_ten;
    float m_con;
    float m_bia;
    
    SMRSkeleton lastPosture;
   
 // deque<pair<int,SMRVector3>> leftGesture;
 // deque<pair<int,SMRVector3>> rightGesture;


public:
    

    GestureModifier(float init_v_s,float init_v_t,float init_v_p,float init_v_f);
    
    ~GestureModifier();

    void setSpatialExtent(float _v_s);
    
    inline float getSpatialExtent() {return m_v_s;}

    //inline void addleftPosture(float _x, float _y, float _z, int _time) 
    //{ 
    //  SMRVector3 newPos(_x,_y,_z);
    //  leftGesture.push_back(pair<int,SMRVector3>(_time,newPos));
    //}

    //inline void addrightPosture(float _x, float _y, float _z, int _time) 
    //{ 
    //  SMRVector3 newPos(_x,_y,_z);
    //  rightGesture.push_back(pair<int,SMRVector3>(_time,newPos));
    //}
    
    inline float getContinuityParameter() {return m_con;}
    
    inline float getBiasParameter() {return m_bia;}
    
    inline float getTensionParameter() {return m_ten;}
    
    inline float getPower() {return m_v_p;}
    
    inline void getInterpolationParameters(SMRVector3 * _tensBiasCont) 
    { 
      _tensBiasCont->m_x = m_ten;
      _tensBiasCont->m_y = m_bia;
      _tensBiasCont->m_z = m_con;
    }
      
  //  int calcPreGestureRight(SMRVector3 &_prePos);
  //  int calcPreGestureLeft(SMRVector3 &_prePos);

    void setTemporalExtent(float _v_t);
    
    void setPower(float _v_p);

    void setFluidity(float _v_f);

    bool modifyGesture();
    
    int temporalExtent(int t0, int t1);

    void spatialMod(SMRVector3 &origin, const SMRVector3 & _shoulder, const char & _bodyPart);

    //int calcDelay(const SMRVector3 &start,const SMRVector3 &end,int _duration);
    
    void spatialExtent(SMRVector3 &origin);

    void spatialExtent(const SMRVector3 &  _a,SMRVector3 &  _b,const SMRVector3  _c);
    
    void spatialExtent(vector<MotionSegment *> & _motionSegments);
    
    void spatialExtent(vector<MotionSegment *> & _motionSegments, const SMRVector3 & _lowerBoxBound, const SMRVector3 & _higherBoxBound);
    
    void computeBoundingBox(vector<MotionSegment *> & _motionSegments, SMRVector3 & _lowerBoxBound, SMRVector3 & _higherBoxBound);
    
    void applyPower(vector<MotionSegment *> & _motionSegments, unsigned int _nucleusStart);
    
    void applyTemporalExtent(vector<MotionSegment *> & _motionSegments, unsigned int & _delay, unsigned int _nucleusStart, unsigned int _nucleusEnd);
    
    void interpolateBias(float & _bias, unsigned int _position, unsigned int _steps);
  };
#endif


