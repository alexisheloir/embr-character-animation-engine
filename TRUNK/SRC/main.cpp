/*
*
*  Test class for DFKI realizer
*
*/

#include "Realizer.h"


int main (int argc, char * argv[])
{

Realizer myRealizer;

//Parser::getInstance();

//SMRMotion motion = loadMotionFromBVH("exportedPose.bvh");
//SMRSkeleton posture = motion.getSkeleton(0);

//ActuatorFactory::getInstance()->addCharacter("Alfonse");
//Character* currentCharacter = ActuatorFactory::getInstance()->getCharacter("Alfonse");
//currentCharacter->addPose(&posture);
//ActuatorFactory::getInstance()->init();

myRealizer.run();

ActuatorFactory::getInstance()->kill();
//Parser::getInstance()->kill();
//SocketListener::getInstance()->kill();

exit(0);
}


