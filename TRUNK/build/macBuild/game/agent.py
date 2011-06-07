import direct.directbase.DirectStart
import SMRPy
from pandac.PandaModules import AmbientLight,DirectionalLight
from pandac.PandaModules import TextNode,NodePath,LightAttrib,AntialiasAttrib
from pandac.PandaModules import Vec3,Vec4
from pandac.PandaModules import Quat,Mat4
from direct.actor.Actor import Actor
from direct.task.Task import Task
from direct.gui.OnscreenText import OnscreenText
from direct.showbase.DirectObject import DirectObject
from pandac.PandaModules import Lens
from pandac.PandaModules import Texture,TextureStage
from pandac.PandaModules import PStatClient
from pandac.PandaModules import *
import thread
import sys
import math
import copy
import getopt

import shaders
import shadowmap

#debug
import time
import os
from pandac.PandaModules import Filename
import re

from pandac.PandaModules import TransparencyAttrib

class Agent:
    def __init__( self, _name ):
        print "Creating agent " + _name
        self.name = _name
        self.loadLightingFromConfig = 0
        self.createShadowMap = 0
     
    def setDataPath( self, _dataPath ):
        self.dataPath = _dataPath;
        
    def setActor( self, _modelFileName, _animationFileNames, _morphTargetsFileName ):
        self.modelFileName = _modelFileName;
        self.animationFileNames = _animationFileNames;
        self.morphTargetsFileName = _morphTargetsFileName;
    
    def setRealizer( self, _realizer ):
        self.realizer = _realizer;
        
    def setCameraMask( self, _mask ):
        self.cameraMask = _mask
        
    def setShadowMapParameters( self, _index, _distance ):
        self.createShadowMap = 1
        self.shadowMapIndex = _index
        self.shadowMapDistance = _distance
        
    def setTransform( self, x, y, z, rx, ry, rz, scale ):
        self.agent.setPos( x, y, z )
        self.agent.setScale( scale )
        self.agent.setHpr( rx, ry, rz )
        self.positionX = x
        self.positionY = y
        self.positionZ = z
        
    def setLightingConfiguration( self, _config, _name ):
        self.lightingConfig = _config
        self.lightingConfigName = _name
        self.loadLightingFromConfig = 1
        
    def init( self ):
        #load the agent and parent it to the world
        #The joints of this agent will reference Panda NodePaths, it will be possible to play animations on it
        self.animationAgent = Actor( self.modelFileName, self.animationFileNames )
        
        self.agent = Actor( self.modelFileName, self.morphTargetsFileName )
        
        self.targets = {#'Basis':[],                
                        'ExpSmileClosed':[],
                        'ExpAnger':[],
                        'ExpDisgust':[],
                        'ExpFear':[],
                        'ExpSad':[],
                        'ExpSurprise':[],
                        'ExpSmileOpen':[],
                        'ModBlinkLeft':[],
                        'ModBlinkRight':[],
                        'ModBrowDownLeft':[],
                        'ModBrowDownRight':[],
                        'ModBlinkRight':[],
                        'ModBrowInRight':[],
                        'ModBrowInLeft':[],
                        'ModBrowUpLeft':[],
                        'ModBrowUpRight':[],
                        'ModEarsOut':[],
                        'ModEyeSquintLeft':[],
                        'ModEyeSquintRight':[],
                        'ModLookDown':[],
                        'ModLookLeft':[],
                        'ModLookRight':[],
                        'ModLookUp':[],
                        'ModBlinkLeft':[],
                        'Phonaah':[],
                        'PhonB,M,P':[],
                        'Phonbigaah':[],
                        'Phonch,J,sh':[],
                        'PhonD,S,T':[],
                        'Phonee':[],
                        'Phoneh':[],
                        'PhonF,V':[],
                        'Phoni':[],
                        'PhonK':[],
                        'PhonN':[],
                        'Phonoh':[],
                        'Phonooh,Q':[],
                        'PhonR':[],
                        'Phonth':[],
                        'PhonW':[]}
        
        iter = self.targets.iteritems()
        while True :
            try: morphsItem = iter.next()
            except StopIteration:break
            for i in range(2,7):
                #print (str(i)+'_'+morphsItem[0])
                blendShape = self.agent.controlJoint(None, 'modelRoot', str(i)+'_'+morphsItem[0])
                if(blendShape):
                    morphsItem[1].append(blendShape)

        self.targets['inspire'] = [self.agent.controlJoint(None, 'modelRoot', 'inspire')]
        
         #instanciate a list in order to keep track of kinematic joints joints 
        #in python runtime
        #if nothing points towards those joints, they get flushed by
        #python's garbage collector
        self.jointList = []
        self.jointFKList = []
        self.agentControlJoints = []
        self.agentNodePaths = []

        self.agentSMRSkel = SMRPy.SMRSkeleton(True,True,'agent')
        self.createSkel( self.agent, self.agentSMRSkel, 'root', '' )
        #SMRPy.exportSkeletonToBvh('exportedPose.bvh',self.agentSMRSkel);
        self.newSkeleton = SMRPy.SMRSkeleton(True,True,'pose')
        self.createFKSkel(self.animationAgent, self.newSkeleton, "root", '')    

        self.realizer.addCharacter( self.name, self.agentSMRSkel );
        
        for key in self.targets.keys():
            self.realizer.addMorphTarget( self.name, key )
        
        self.realizer.addShaderParameter( self.name, 'blushing' )
                
        self.addAnimation(self.name,'breathing')
        self.addAnimation(self.name,'hands_claw')
        self.addAnimation(self.name,'hands_fist')
        self.addAnimation(self.name,'hands_index')
        self.addAnimation(self.name,'hands_open-relaxed')
        self.addAnimation(self.name,'hands_open-spread')
        self.addAnimation(self.name,'hands_open-straight')
        self.addAnimation(self.name,'hands_purse')
        self.addAnimation(self.name,'hands_ring')
        self.addAnimation(self.name,'poseNeutral')

        self.addAnimation(self.name,'hands_DGS_A')
        self.addAnimation(self.name,'hands_DGS_B')
        self.addAnimation(self.name,'hands_DGS_C')
        self.addAnimation(self.name,'hands_DGS_D')
        self.addAnimation(self.name,'hands_DGS_E')
        self.addAnimation(self.name,'hands_DGS_F')
        self.addAnimation(self.name,'hands_DGS_G')
        self.addAnimation(self.name,'hands_DGS_H')
        self.addAnimation(self.name,'hands_DGS_I')
        self.addAnimation(self.name,'hands_DGS_J')
        self.addAnimation(self.name,'hands_DGS_K')
        self.addAnimation(self.name,'hands_DGS_L')
        self.addAnimation(self.name,'hands_DGS_M')
        self.addAnimation(self.name,'hands_DGS_N')
        self.addAnimation(self.name,'hands_DGS_O')
        self.addAnimation(self.name,'hands_DGS_P')
        self.addAnimation(self.name,'hands_DGS_Q')
        self.addAnimation(self.name,'hands_DGS_R')
        self.addAnimation(self.name,'hands_DGS_T')
        self.addAnimation(self.name,'hands_DGS_U')
        self.addAnimation(self.name,'hands_DGS_V')
        self.addAnimation(self.name,'hands_DGS_W')
        self.addAnimation(self.name,'hands_DGS_X')
        self.addAnimation(self.name,'hands_DGS_Y')
        self.addAnimation(self.name,'hands_DGS_Z')
        self.addAnimation(self.name,'hands_DGS_SCH')

        self.addAnimation(self.name,'hands_ASL_A')
        self.addAnimation(self.name,'hands_ASL_B')
        self.addAnimation(self.name,'hands_ASL_C')
        self.addAnimation(self.name,'hands_ASL_D')
        self.addAnimation(self.name,'hands_ASL_E')
        self.addAnimation(self.name,'hands_ASL_F')
        self.addAnimation(self.name,'hands_ASL_G')
        self.addAnimation(self.name,'hands_ASL_H')
        self.addAnimation(self.name,'hands_ASL_I')
        self.addAnimation(self.name,'hands_ASL_J')
        self.addAnimation(self.name,'hands_ASL_K')
        self.addAnimation(self.name,'hands_ASL_L')
        self.addAnimation(self.name,'hands_ASL_M')
        self.addAnimation(self.name,'hands_ASL_N')
        self.addAnimation(self.name,'hands_ASL_O')
        self.addAnimation(self.name,'hands_ASL_P')
        self.addAnimation(self.name,'hands_ASL_Q')
        self.addAnimation(self.name,'hands_ASL_R')
        self.addAnimation(self.name,'hands_ASL_T')
        self.addAnimation(self.name,'hands_ASL_U')
        self.addAnimation(self.name,'hands_ASL_V')
        self.addAnimation(self.name,'hands_ASL_W')
        self.addAnimation(self.name,'hands_ASL_X')
        self.addAnimation(self.name,'hands_ASL_Y')
        self.addAnimation(self.name,'hands_ASL_Z')

        self.addAnimation(self.name,'hands_ASL_1CL')
        self.addAnimation(self.name,'hands_ASL_2CL')
        self.addAnimation(self.name,'hands_ASL_3CL')
        self.addAnimation(self.name,'hands_ASL_4CL')
        self.addAnimation(self.name,'hands_ASL_5aCL')
        self.addAnimation(self.name,'hands_ASL_5bCL')
        self.addAnimation(self.name,'hands_ASL_ACL')
        self.addAnimation(self.name,'hands_ASL_BCL')
        self.addAnimation(self.name,'hands_ASL_CCL')
        self.addAnimation(self.name,'hands_ASL_FCL')
        self.addAnimation(self.name,'hands_ASL_ICL')
        self.addAnimation(self.name,'hands_ASL_ILYCL')
        self.addAnimation(self.name,'hands_ASL_M')
        self.addAnimation(self.name,'hands_ASL_N')
        self.addAnimation(self.name,'hands_ASL_SCL')
        self.addAnimation(self.name,'hands_ASL_T')
        self.addAnimation(self.name,'hands_ASL_VaCL')
        self.addAnimation(self.name,'hands_ASL_VbCL')
        self.addAnimation(self.name,'hands_ASL_YCL')

        #self.addAnimation(self.name,'endian')
        
        if 'default' in self.animationFileNames:
            self.addAnimation(self.name,'default')
        
        #self.realizer.addCharacterSkeleton( self.name,  );
        
        self.agent.reparentTo( render )
        self.agent.hide( BitMask32.bit( self.cameraMask ) )
        
        #set lighting
        ambientColor = Vec4(0.0,0.0,0.0,0.0)
        self.lighting = shaders.Lighting( ambientColor )
        if self.loadLightingFromConfig == 1:
            self.lighting.loadFromConfig( self.lightingConfig, self.lightingConfigName )
        
        #shadow map
        if self.createShadowMap == 1:
            self.shadowMap = shadowmap.ShadowMap( self.shadowMapIndex, self.dataPath )
        
    def setShaders( self, _shaders ):
        self.shaders = _shaders
        
    def update( self ):
        self.realizer.skeletonRequested()
        self.updatePandaSkeleton(self.agentControlJoints, self.agentSMRSkel)
        
        for key in self.targets.keys():
            #print(key, "\n")
            weight = self.realizer.getMorphTargetWeight( self.name, key )
            morphTargets = self.targets[key]
            for morphTarget in morphTargets :
                #print(weight, "\n")
                morphTarget.setX(weight)
                
        blushingValue = self.realizer.getShaderParameterValue( self.name, 'blushing' )
        self.shaders.headShader.blushing.set( blushingValue )
        
        self.lighting.update()
        if self.createShadowMap == 1:
            self.shadowMap.setLighting( self.lighting, Vec3( self.positionX, self.positionY, self.positionZ ), self.shadowMapDistance )
            self.shaders.updateWithShadows( self.shadowMap )
        else:
            self.shaders.update()
        
    def addAnimation(self,_actorName,_animationName):
        self.animationAgent.reparentTo(render)
        self.animationAgent.setScale(10)
        self.realizer.addAnimation(_actorName, _animationName)
        for i in range (self.animationAgent.getNumFrames(_animationName)):
            self.animationAgent.pose(_animationName,i)
            base.graphicsEngine.renderFrame()
            self.updateSMRSkeleton(self.agentNodePaths, self.newSkeleton)
            self.realizer.addPoseToAnimation(_actorName, _animationName, self.newSkeleton)
        self.animationAgent.detachNode()
        print "animation",_animationName,"added"
     
    def createSkel(self, _pandaAgent, _smrSkel, _initialJointName, _parentName):
        #get the agent's currentJoint
        currentPandaJoint = _pandaAgent.getJoints(None,_initialJointName)

        currentPandaCJoint = _pandaAgent.controlJoint(None, 'modelRoot', _initialJointName)
        self.agentControlJoints.append(currentPandaCJoint)
        #get the first joint's position
        position = currentPandaCJoint.getPos()

        if (currentPandaJoint[0].getNumChildren() == 0):
            newJoint = SMRPy.SMRJoint(True)
            newJoint.setEndVect(position.getX(),position.getY(),position.getZ());
        else:  
            newJoint = SMRPy.SMRJoint(False)

        rotZ = (currentPandaCJoint.getH()/180.0)*3.14159;
        rotX = (currentPandaCJoint.getP()/180.0)*3.14159;
        rotY = (currentPandaCJoint.getR()/180.0)*3.14159;

        quatZ = SMRPy.SMRQuaternion(SMRPy.SMRVector3(0.0,0.0,1.0),rotZ)
        quatX = SMRPy.SMRQuaternion(SMRPy.SMRVector3(1.0,0.0,0.0),rotX)
        quatY = SMRPy.SMRQuaternion(SMRPy.SMRVector3(0.0,1.0,0.0),rotY)

        quatRot = quatZ.multiply(quatX)
        quatRot = quatRot.multiply(quatY)
        quatRot.normalize();

        newJoint.setPos(position.getX(),position.getY(),position.getZ())
        newJoint.setRotQuat(quatRot.getW(),quatRot.getX(),quatRot.getY(),quatRot.getZ())

        newJoint.setParentName(_parentName)
        newJoint.setName(_initialJointName)

        #print _initialJointName, 'numchildren : ', currentPandaJoint[0].getNumChildren()

        self.jointList.append(newJoint)
        _smrSkel.insertJoint(newJoint)

        for i in range(currentPandaJoint[0].getNumChildren()):
            childJoint = currentPandaJoint[0].getChild(i)
            childName = childJoint.getName()
            #print(childName)
            self.createSkel(_pandaAgent, _smrSkel, childName, _initialJointName)
        
    def createFKSkel(self, _pandaAgent, _smrSkel, _initialJointName, _parentName):
        #get the agent's currentJoint
        currentPandaJoint = _pandaAgent.getJoints(None, _initialJointName)

        currentPandaJointPath = _pandaAgent.exposeJoint(None, 'modelRoot', _initialJointName, "lodRoot", True)
        
        #currentPandaCJoint = _pandaAgent.controlJoint(None, 'modelRoot', _initialJointName)
        #if (_parentName == 'root' or _parentName == '' or _parentName == 'Bone.001'):
        #    currentPandaJointPath = _pandaAgent.exposeJoint(None, 'modelRoot', _initialJointName)
        #else:
        #    print(_parentName)
        #    currentPandaJointPath = _pandaAgent.exposeJoint(None, _parentName, _initialJointName)
            
        #if (_initialJointName == "lhand"):
        #    self.rhand = currentPandaJointPath

        self.agentNodePaths.append(currentPandaJointPath)

        #get the first joint's position
        position = currentPandaJointPath.getPos()

        if (currentPandaJoint[0].getNumChildren() == 0):
            newJoint = SMRPy.SMRJoint(True)
            newJoint.setEndVect(position.getX(),position.getY(),position.getZ());
        else:  
            newJoint = SMRPy.SMRJoint(False)

        quatRot = currentPandaJointPath.getQuat()

        newJoint.setPos(position.getX(),position.getY(),position.getZ())
        newJoint.setRotQuat(quatRot.getR(),quatRot.getI(),quatRot.getJ(),quatRot.getK())

        newJoint.setParentName(_parentName)
        newJoint.setName(_initialJointName)

        #print _initialJointName, 'numchildren : ', currentPandaJoint[0].getNumChildren()

        self.jointFKList.append(newJoint)
        _smrSkel.insertJoint(newJoint)

        for i in range(currentPandaJoint[0].getNumChildren()):
            childJoint = currentPandaJoint[0].getChild(i)
            childName = childJoint.getName()
            #print(childName)
            self.createFKSkel(_pandaAgent, _smrSkel, childName, _initialJointName)

    def updatePandaSkeleton(self, agentControlJoints, _smrSkeleton):

        #synchronize root joint
        SMRJoint = _smrSkeleton.getJoint(0)
        PANDAJoint = agentControlJoints[0]
        position = SMRJoint.getPos() 
        PANDAJoint.setPos(position.X(),position.Y(),position.Z());

        for i in range(_smrSkeleton.getNumjoints()):
          SMRJoint = _smrSkeleton.getJoint(i)
          PANDAJoint = agentControlJoints[i]
          self.synchronize(PANDAJoint,SMRJoint)

    def updateSMRSkeleton(self, agentNodePaths, _smrSkeleton):
        #synchronize root joint
        SMRJoint = _smrSkeleton.getJoint(0)
        PANDAJoint = agentNodePaths[0]
        position = PANDAJoint.getPos() 
        SMRJoint.setPos(position.getX(),position.getY(),position.getZ());

        for i in range(_smrSkeleton.getNumjoints()):
          SMRJoint = _smrSkeleton.getJoint(i)
          PANDAJoint = agentNodePaths[i]
          self.synchronizePandaToSMR(SMRJoint,PANDAJoint)

    def synchronizePandaToSMR(self, _SMRJoint, _PANDAJoint):
        pandaQuaternion = _PANDAJoint.getQuat()
        x = pandaQuaternion.getI()
        y = pandaQuaternion.getJ()
        z = pandaQuaternion.getK()
        w = pandaQuaternion.getR()
        _SMRJoint.setRotQuat(w,x,y,z)

    def synchronize(self, _pandaCJoint, _smrJoint):
        smrQuaternion = _smrJoint.getRot()
        pandaQuaternion = Quat()
        pandaQuaternion.setI(smrQuaternion.getX())
        pandaQuaternion.setJ(smrQuaternion.getY())
        pandaQuaternion.setK(smrQuaternion.getZ())
        pandaQuaternion.setR(smrQuaternion.getW())
        if not(pandaQuaternion.isNan()):
            _pandaCJoint.setQuat(pandaQuaternion)
