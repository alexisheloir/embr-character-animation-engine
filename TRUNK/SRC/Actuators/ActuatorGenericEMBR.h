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


#ifndef _ACTUATORGENERIC_H
#define _ACTUATORGENERIC_H

#include <string>
#include "UDPFeedbackSender.h"

#include "../EMBRLoggers.h"
#include "../Character.h"

#include "SMRUtils.h"

typedef struct feedbackMessageStruct{
  typedef enum feedbackMessageStatusEnum{SUCCES,FAILURE,FATAL} feedbackMessageStatus;
  string id;
  unsigned int time;
  feedbackMessageStatus status; 
  string feedback;
} FeedbackMessage;

class ActuatorGenericEMBR
{

public:
  bool m_feedbackRequired; /** does this segment should send a feedback message? */
  bool m_timeForFeedback;
  unsigned int m_absoluteTimeMs;
  FeedbackMessage m_feedbackMessage; /** feedback message to be enqueed in the socket listener */
  
  typedef enum ActuatorTypeEnum{ANIMATION, KINEMATIC, REAL_VALUED, CORRELATIONMAP} ActuatorType;
  ActuatorGenericEMBR(Character* _relatedCharacter);
  virtual ~ActuatorGenericEMBR();
  virtual void process(float relativeTime) = 0;
  ActuatorType m_type;

  string feedbackMessageToString()
  {
    stringstream ss;
    ss << std::endl << std::endl << m_feedbackMessage.id << std::endl << "time: " << m_absoluteTimeMs << std::endl << m_feedbackMessage.feedback;
    string feedbackMessage = ss.str();
    return feedbackMessage;
  }

protected:
  Character *m_relatedCharacter;
  

};

#endif

