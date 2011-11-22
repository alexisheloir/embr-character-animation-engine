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

import time
import os
from pandac.PandaModules import Filename
import re

from pandac.PandaModules import TransparencyAttrib

class Agent:
    def __init__( self, _name ):
        print "Creating agent " + _name
        self.name = _name
     
    def setDataPath( self, _dataPath ):
        self.dataPath = _dataPath;
        
    def setActor( self, _modelFileName, _animationFileNames, _morphTargetsFileName ):
        self.modelFileName = _modelFileName;
        self.animationFileNames = _animationFileNames;
        self.morphTargetsFileName = _morphTargetsFileName;
    
    def setRealizer( self, _realizer ):
        self.realizer = _realizer;
        
    def setTransform( self, x, y, z, rx, ry, rz, scale ):
        self.agent.setPos( x, y, z )
        self.agent.setScale( scale )
        self.agent.setHpr( rx, ry, rz )
        self.positionX = x
        self.positionY = y
        self.positionZ = z

    def getPosition( self ):
        return self.agent.getPos()
        
    def init( self ):
        #load the agent and parent it to the world
        #The joints of this agent will reference Panda NodePaths, it will be possible to play animations on it
        self.animationAgent = Actor( self.modelFileName, self.animationFileNames )
        
        self.agent = Actor( self.modelFileName, self.morphTargetsFileName )

        maxMorphTargets={'MT_Jaw_Open':self.agent.controlJoint(None, 'modelRoot', 'MT_Jaw_Open'),
                         'MT_Jaw_L':self.agent.controlJoint(None, 'modelRoot', 'MT_Jaw_L'),
                         'MT_Jaw_R':self.agent.controlJoint(None, 'modelRoot', 'MT_Jaw_R'),
                         'MT_Jaw_Fwd':self.agent.controlJoint(None, 'modelRoot', 'MT_Jaw_Fwd'),
                         'MT_WideL':self.agent.controlJoint(None, 'modelRoot', 'MT_WideL'),
                         'MT_WideR':self.agent.controlJoint(None, 'modelRoot', 'MT_WideR'),
                         'MT_NarrowL':self.agent.controlJoint(None, 'modelRoot', 'MT_NarrowL'),
                         'MT_NarrowR':self.agent.controlJoint(None, 'modelRoot', 'MT_NarrowR'),
                         'MT_FrownL':self.agent.controlJoint(None, 'modelRoot', 'MT_FrownL'),
                         'MT_FrownR':self.agent.controlJoint(None, 'modelRoot', 'MT_FrownR'),
                         'MT_SneerL':self.agent.controlJoint(None, 'modelRoot', 'MT_SneerL'),
                         'MT_SneerR':self.agent.controlJoint(None, 'modelRoot', 'MT_SneerR'),
                         'MT_SquintL':self.agent.controlJoint(None, 'modelRoot', 'MT_SquintL'),
                         'MT_SquintR':self.agent.controlJoint(None, 'modelRoot', 'MT_SquintR'),
                         'MT_BrowUpL':self.agent.controlJoint(None, 'modelRoot', 'MT_BrowUpL'),
                         'MT_BrowUpR':self.agent.controlJoint(None, 'modelRoot', 'MT_BrowUpR'),
                         'MT_BrowDnL':self.agent.controlJoint(None, 'modelRoot', 'MT_BrowDnL'),
                         'MT_BrowDnR':self.agent.controlJoint(None, 'modelRoot', 'MT_BrowDnR'),
                         'MT_MBrowUp':self.agent.controlJoint(None, 'modelRoot', 'MT_MBrowUp'),
                         'MT_BrowDnR':self.agent.controlJoint(None, 'modelRoot', 'MT_BrowDnR'),
                         'MT_BrowDnL':self.agent.controlJoint(None, 'modelRoot', 'MT_BrowDnL'),
                         'MT_MBrowDn':self.agent.controlJoint(None, 'modelRoot', 'MT_MBrowDn'),
                         'MT_BrowSqueeze':self.agent.controlJoint(None, 'modelRoot', 'MT_BrowSqueeze'),
                         'MT_MouthL':self.agent.controlJoint(None, 'modelRoot', 'MT_MouthL'),
                         'MT_MouthR':self.agent.controlJoint(None, 'modelRoot', 'MT_MouthR'),
                         'MT_UprLipUpL':self.agent.controlJoint(None, 'modelRoot', 'MT_UprLipUpL'),
                         'MT_UprLipUpR':self.agent.controlJoint(None, 'modelRoot', 'MT_UprLipUpR'),
                         'MT_UprLipDnL':self.agent.controlJoint(None, 'modelRoot', 'MT_UprLipDnL'),
                         'MT_UprLipDnR':self.agent.controlJoint(None, 'modelRoot', 'MT_UprLipDnR'),
                         'MT_LwrLipUpL':self.agent.controlJoint(None, 'modelRoot', 'MT_LwrLipUpL'),
                         'MT_LwrLipUpR':self.agent.controlJoint(None, 'modelRoot', 'MT_LwrLipUpR'),
                         'MT_LwrLipDnL':self.agent.controlJoint(None, 'modelRoot', 'MT_LwrLipDnL'),
                         'MT_LwrLipDnR':self.agent.controlJoint(None, 'modelRoot', 'MT_LwrLipDnR'),
                         'MT_BlowCheeksL':self.agent.controlJoint(None, 'modelRoot', 'MT_BlowCheeksL'),
                         'MT_BlowCheeksR':self.agent.controlJoint(None, 'modelRoot', 'MT_BlowCheeksR'),
                         'MT_TongueOut':self.agent.controlJoint(None, 'modelRoot', 'MT_TongueOut'),
                         'MT_TongueUp':self.agent.controlJoint(None, 'modelRoot', 'MT_TongueUp'),
                         'MT_TongueTipUp':self.agent.controlJoint(None, 'modelRoot', 'MT_TongueTipUp'),
                         'MT_TongueL':self.agent.controlJoint(None, 'modelRoot', 'MT_TongueL'),
                         'MT_TongueR':self.agent.controlJoint(None, 'modelRoot', 'MT_TongueR'),
                         'MT_Blink_L':self.agent.controlJoint(None, 'modelRoot', 'MT_Blink_L'),
                         'MT_Blink_R':self.agent.controlJoint(None, 'modelRoot', 'MT_Blink_R')}
                         

        self.targets = {#'Basis':[],                
                        'ExpSmileClosed':{maxMorphTargets['MT_WideR']:1.0,maxMorphTargets['MT_WideL']:1.0},
                        'ExpAnger':{maxMorphTargets['MT_Jaw_Open']:-0.07,maxMorphTargets['MT_Jaw_Fwd']:0.25,maxMorphTargets['MT_NarrowL']:0.30,maxMorphTargets['MT_NarrowR']:0.30,maxMorphTargets['MT_SquintL']:0.60,maxMorphTargets['MT_SquintR']:0.60,maxMorphTargets['MT_MBrowDn']:1.0,maxMorphTargets['MT_BrowDnL']:0.7,maxMorphTargets['MT_BrowDnR']:0.7,maxMorphTargets['MT_SneerL']:0.8,maxMorphTargets['MT_SneerR']:0.8,maxMorphTargets['MT_FrownL']:0.2,maxMorphTargets['MT_FrownR']:0.2,maxMorphTargets['MT_UprLipDnL']:0.45,maxMorphTargets['MT_UprLipDnR']:0.45,maxMorphTargets['MT_LwrLipUpL']:1.0,maxMorphTargets['MT_LwrLipUpR']:1.0},
                        'ExpDisgust':{maxMorphTargets['MT_WideL']:0.25,maxMorphTargets['MT_WideR']:0.15,maxMorphTargets['MT_SquintL']:0.40,maxMorphTargets['MT_SquintR']:0.40,maxMorphTargets['MT_MBrowDn']:0.25,maxMorphTargets['MT_BrowSqueeze']:0.45,maxMorphTargets['MT_WideL']:0.25,maxMorphTargets['MT_BrowDnL']:0.50,maxMorphTargets['MT_BrowDnR']:0.50,maxMorphTargets['MT_SneerL']:2.0,maxMorphTargets['MT_SneerR']:2.0,maxMorphTargets['MT_FrownL']:0.25,maxMorphTargets['MT_FrownR']:0.25,maxMorphTargets['MT_UprLipUpL']:0.66,maxMorphTargets['MT_UprLipUpR']:0.33,maxMorphTargets['MT_LwrLipUpL']:0.40,maxMorphTargets['MT_LwrLipUpR']:0.40 },
                        'ExpFear':{maxMorphTargets['MT_Jaw_Open']:0.15,maxMorphTargets['MT_Jaw_Fwd']:-0.3,maxMorphTargets['MT_NarrowL']:0.24,maxMorphTargets['MT_NarrowR']:0.24,maxMorphTargets['MT_SquintL']:-0.4,maxMorphTargets['MT_SquintR']:-0.4,maxMorphTargets['MT_BrowUpL']:0.36,maxMorphTargets['MT_BrowUpR']:0.36,maxMorphTargets['MT_MBrowUp']:1.30,maxMorphTargets['MT_BrowSqueeze']:0.40,maxMorphTargets['MT_FrownL']:0.35,maxMorphTargets['MT_FrownR']:0.35,maxMorphTargets['MT_UprLipDnL']:0.25,maxMorphTargets['MT_UprLipDnR']:0.25,maxMorphTargets['MT_LwrLipUpL']:0.35,maxMorphTargets['MT_LwrLipUpR']:0.35},
                        'ExpSad':{maxMorphTargets['MT_NarrowL']:0.20,maxMorphTargets['MT_NarrowR']:0.20,maxMorphTargets['MT_SquintL']:0.20,maxMorphTargets['MT_SquintR']:0.20,maxMorphTargets['MT_MBrowUp']:0.40,maxMorphTargets['MT_BrowSqueeze']:0.66,maxMorphTargets['MT_BrowDnL']:0.66,maxMorphTargets['MT_BrowDnR']:0.66,maxMorphTargets['MT_BlowCheeksL']:-0.25,maxMorphTargets['MT_BlowCheeksR']:-0.25,maxMorphTargets['MT_FrownL']:0.66,maxMorphTargets['MT_FrownR']:0.66,maxMorphTargets['MT_UprLipDnL']:0.50,maxMorphTargets['MT_UprLipDnR']:0.50,maxMorphTargets['MT_LwrLipUpL']:0.66,maxMorphTargets['MT_LwrLipUpR']:0.66},
                        'ExpSurprise':{maxMorphTargets['MT_Jaw_Open']:0.25,maxMorphTargets['MT_NarrowL']:0.24,maxMorphTargets['MT_NarrowR']:0.24,maxMorphTargets['MT_SquintL']:-0.20,maxMorphTargets['MT_SquintR']:-0.20,maxMorphTargets['MT_BrowUpL']:0.92,maxMorphTargets['MT_BrowUpR']:0.92,maxMorphTargets['MT_MBrowUp']:0.66},
                        'ExpSmileOpen':{maxMorphTargets['MT_Jaw_Open']:0.16,maxMorphTargets['MT_WideL']:0.740,maxMorphTargets['MT_WideR']:0.740,maxMorphTargets['MT_SneerL']:0.280,maxMorphTargets['MT_SneerR']:0.280,maxMorphTargets['MT_BrowUpL']:0.360,maxMorphTargets['MT_BrowUpR']:0.360},
                        'ExpHappy':{maxMorphTargets['MT_Jaw_Open']:0.22,maxMorphTargets['MT_WideL']:0.75,maxMorphTargets['MT_WideR']:0.75,maxMorphTargets['MT_SquintL']:0.35,maxMorphTargets['MT_SquintR']:0.35,maxMorphTargets['MT_BrowDnL']:0.08,maxMorphTargets['MT_BrowDnR']:0.08,maxMorphTargets['MT_UprLipUpL']:0.15,maxMorphTargets['MT_UprLipUpR']:0.15,maxMorphTargets['MT_LwrLipUpL']:0.15,maxMorphTargets['MT_LwrLipUpR']:0.15,maxMorphTargets['MT_BrowUpL']:0.360,maxMorphTargets['MT_BrowUpR']:0.360},
                        'ModBlinkLeft':{maxMorphTargets['MT_Blink_L']:1.0},
                        'ModBlinkRight':{maxMorphTargets['MT_Blink_R']:1.0},
                        'ModBrowDownLeft':{maxMorphTargets['MT_BrowDnL']:1.0},
                        'ModBrowDownRight':{maxMorphTargets['MT_BrowDnR']:1.0},
                        'ModBrowInRight':{maxMorphTargets['MT_BrowSqueeze']:1.0},
                        'ModBrowInLeft':{maxMorphTargets['MT_BrowSqueeze']:1.0},
                        'ModBrowUpLeft':{maxMorphTargets['MT_BrowUpL']:1.0},
                        'ModBrowUpRight':{maxMorphTargets['MT_BrowUpR']:1.0},
                        #'ModEarsOut':[],
                        'ModEyeSquintLeft':{maxMorphTargets['MT_SquintL']:1.0},
                        'ModEyeSquintRight':{maxMorphTargets['MT_SquintR']:1.0},
                        'Phonaah':{maxMorphTargets['MT_Jaw_Open']:1.0},
                        'PhonB,M,P':{maxMorphTargets['MT_WideL']:0.5,maxMorphTargets['MT_WideR']:0.5},
                        'Phonbigaah':{maxMorphTargets['MT_Jaw_Open']:1.0},
                        'Phonch,J,sh':{maxMorphTargets['MT_WideL']:0.5,maxMorphTargets['MT_WideR']:0.5},
                        'PhonD,S,T':{maxMorphTargets['MT_WideL']:0.5,maxMorphTargets['MT_WideR']:0.5,maxMorphTargets['MT_Jaw_Open']:0.2 },
                        'Phonee':{maxMorphTargets['MT_WideL']:0.5,maxMorphTargets['MT_WideR']:0.5},
                        'Phoneh':{maxMorphTargets['MT_WideL']:0.5,maxMorphTargets['MT_WideR']:0.5,maxMorphTargets['MT_Jaw_Open']:0.2 },
                        'PhonF,V':{maxMorphTargets['MT_WideL']:0.2,maxMorphTargets['MT_WideR']:0.2,maxMorphTargets['MT_Jaw_Open']:0.3,maxMorphTargets['MT_UprLipDnL']:0.3,maxMorphTargets['MT_UprLipDnR']:0.3},
                        'Phoni':{maxMorphTargets['MT_WideL']:0.2,maxMorphTargets['MT_WideR']:0.2,maxMorphTargets['MT_Jaw_Open']:0.3 },
                        'PhonK':{maxMorphTargets['MT_Jaw_Open']:0.4},
                        'PhonN':{maxMorphTargets['MT_WideL']:0.5,maxMorphTargets['MT_WideR']:0.5},
                        'Phonoh':{maxMorphTargets['MT_Jaw_Open']:0.4,maxMorphTargets['MT_NarrowL']:0.55,maxMorphTargets['MT_NarrowR']:0.55},
                        'Phonooh,Q':{maxMorphTargets['MT_Jaw_Open']:0.4,maxMorphTargets['MT_NarrowL']:0.55,maxMorphTargets['MT_NarrowR']:0.55},
                        'PhonR':{maxMorphTargets['MT_WideL']:0.5,maxMorphTargets['MT_WideR']:0.5,maxMorphTargets['MT_Jaw_Open']:0.2 },
                        'Phonth':{maxMorphTargets['MT_WideL']:0.5,maxMorphTargets['MT_WideR']:0.5,maxMorphTargets['MT_Jaw_Open']:0.2 },
                        'PhonW':{maxMorphTargets['MT_Jaw_Open']:0.3,maxMorphTargets['MT_NarrowL']:0.7,maxMorphTargets['MT_NarrowR']:0.7},
                        'AU26_MT_Jaw_Open':{maxMorphTargets['MT_Jaw_Open']:1.0},
                        'AU30_MT_Jaw_L':{maxMorphTargets['MT_Jaw_L']:1.0},
                        'AU30_MT_Jaw_R':{maxMorphTargets['MT_Jaw_R']:1.0},
                        'AU31_MT_Jaw_Fwd':{maxMorphTargets['MT_Jaw_Fwd']:1.0},
                        'AU27_MT_WideL':{maxMorphTargets['MT_WideL']:1.0},
                        'AU27_MT_WideR':{maxMorphTargets['MT_WideR']:1.0},
                        'AU18_MT_NarrowL':{maxMorphTargets['MT_NarrowL']:1.0},
                        'AU18_MT_NarrowR':{maxMorphTargets['MT_NarrowL']:1.0},
                        'AU42_MT_FrownL':{maxMorphTargets['MT_FrownL']:1.0},
                        'AU42_MT_FrownR':{maxMorphTargets['MT_FrownR']:1.0},
                        'AU9_MT_SneerL':{maxMorphTargets['MT_SneerL']:1.0},
                        'AU9_MT_SneerR':{maxMorphTargets['MT_SneerR']:1.0},
                        'AU46_MT_SquintL':{maxMorphTargets['MT_SquintL']:1.0},
                        'AU46_MT_SquintR':{maxMorphTargets['MT_SquintR']:1.0},
                        'AU2_MT_BrowUpL':{maxMorphTargets['MT_BrowUpL']:1.0},
                        'AU2_MT_BrowUpR':{maxMorphTargets['MT_BrowUpR']:1.0},
                        'AU4_MT_BrowDnL':{maxMorphTargets['MT_BrowDnL']:1.0},
                        'AU4_MT_BrowDnR':{maxMorphTargets['MT_BrowDnR']:1.0},
                        'AU4_MT_MBrowUp':{maxMorphTargets['MT_MBrowUp']:1.0},
                        'AU1_MT_BrowDnR':{maxMorphTargets['MT_BrowDnR']:1.0},
                        'AU1_MT_BrowDnL':{maxMorphTargets['MT_BrowDnL']:1.0},
                        'AU1_MT_MBrowDn':{maxMorphTargets['MT_MBrowDn']:1.0},
                        'AU44_MT_BrowSqueeze':{maxMorphTargets['MT_BrowSqueeze']:1.0},
                        'AU12_MT_MouthL':{maxMorphTargets['MT_MouthL']:1.0},
                        'AU12_MT_MouthR':{maxMorphTargets['MT_MouthR']:1.0},
                        'AU5_MT_UprLipUpL':{maxMorphTargets['MT_UprLipUpL']:1.0},
                        'AU5_MT_UprLipUpR':{maxMorphTargets['MT_UprLipUpR']:1.0},
                        'MT_UprLipDnL':{maxMorphTargets['MT_UprLipDnL']:1.0},
                        'MT_UprLipDnR':{maxMorphTargets['MT_UprLipDnR']:1.0},
                        'MT_LwrLipUpL':{maxMorphTargets['MT_LwrLipUpL']:1.0},
                        'MT_LwrLipUpR':{maxMorphTargets['MT_LwrLipUpR']:1.0},
                        'MT_LwrLipDnL':{maxMorphTargets['MT_LwrLipDnL']:1.0},
                        'MT_LwrLipDnR':{maxMorphTargets['MT_LwrLipDnR']:1.0},
                        'AU33_MT_BlowCheeksL':{maxMorphTargets['MT_BlowCheeksL']:1.0},
                        'AU33_MT_BlowCheeksR':{maxMorphTargets['MT_BlowCheeksR']:1.0},
                        'AU36_MT_TongueOut':{maxMorphTargets['MT_TongueOut']:1.0},
                        'AU36_MT_TongueUp':{maxMorphTargets['MT_TongueUp']:1.0},
                        'AU36_MT_TongueTipUp':{maxMorphTargets['MT_TongueTipUp']:1.0},
                        'AU36_MT_TongueL':{maxMorphTargets['MT_TongueL']:1.0},
                        'AU36_MT_TongueR':{maxMorphTargets['MT_TongueR']:1.0},
                        'AU45_MT_Blink_L':{maxMorphTargets['MT_Blink_L']:1.0},
                        'AU45_MT_Blink_R':{maxMorphTargets['MT_Blink_R']:1.0}
                        }
        
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
	#Pascal: output of both skeletons amber and alfonse as chracter.bvh
        #SMRPy.exportSkeletonToBvh(self.name + '.bvh',self.agentSMRSkel);
        self.newSkeleton = SMRPy.SMRSkeleton(True,True,'pose')
        self.createFKSkel(self.animationAgent, self.newSkeleton, "root", '')       
        self.realizer.addCharacter( self.name, self.agentSMRSkel );
            
        for key in self.targets.keys():
            self.realizer.addMorphTarget( self.name, key ) #TODO: declare morph targets into CharacterConfigurationFile
        
        #self.realizer.addShaderParameter( self.name, 'blushing' ) #TODO: declare shader inputs into CharacterConfiguration file


        self.realizer.addBVHMotionToCharacter(self.name, 'hands_claw', open('./animations/hands_claw.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_index', open('./animations/hands_index.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_open-relaxed', open('./animations/hands_open-relaxed.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_open-spread', open('./animations/hands_open-spread.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_open-straight', open('./animations/hands_open-straight.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_purse', open('./animations/hands_purse.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ring', open('./animations/hands_ring.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'poseNeutral', open('./animations/poseNeutral.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'shrug', open('./animations/shrug.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_DGS_A', open('./animations/hands_DGS_A.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_DGS_B', open('./animations/hands_DGS_B.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_DGS_C', open('./animations/hands_DGS_C.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_DGS_D', open('./animations/hands_DGS_D.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_DGS_E', open('./animations/hands_DGS_E.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_DGS_F', open('./animations/hands_DGS_F.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_DGS_G', open('./animations/hands_DGS_G.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_DGS_H', open('./animations/hands_DGS_H.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_DGS_I', open('./animations/hands_DGS_I.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_DGS_J', open('./animations/hands_DGS_J.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_DGS_K', open('./animations/hands_DGS_K.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_DGS_L', open('./animations/hands_DGS_L.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_DGS_M', open('./animations/hands_DGS_M.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_DGS_N', open('./animations/hands_DGS_N.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_DGS_O', open('./animations/hands_DGS_O.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_DGS_P', open('./animations/hands_DGS_P.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_DGS_Q', open('./animations/hands_DGS_Q.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_DGS_R', open('./animations/hands_DGS_R.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_DGS_T', open('./animations/hands_DGS_T.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_DGS_U', open('./animations/hands_DGS_U.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_DGS_W', open('./animations/hands_DGS_W.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_DGS_X', open('./animations/hands_DGS_X.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_DGS_Y', open('./animations/hands_DGS_Y.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_DGS_Z', open('./animations/hands_DGS_Z.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_DGS_SCH', open('./animations/hands_DGS_SCH.bvh', "r").read())

        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_A', open('./animations/hands_ASL_A.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_B', open('./animations/hands_ASL_B.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_C', open('./animations/hands_ASL_C.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_D', open('./animations/hands_ASL_D.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_E', open('./animations/hands_ASL_E.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_F', open('./animations/hands_ASL_F.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_G', open('./animations/hands_ASL_G.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_H', open('./animations/hands_ASL_H.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_I', open('./animations/hands_ASL_I.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_J', open('./animations/hands_ASL_J.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_K', open('./animations/hands_ASL_K.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_L', open('./animations/hands_ASL_L.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_M', open('./animations/hands_ASL_M.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_N', open('./animations/hands_ASL_N.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_O', open('./animations/hands_ASL_O.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_P', open('./animations/hands_ASL_P.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_Q', open('./animations/hands_ASL_Q.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_R', open('./animations/hands_ASL_R.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_T', open('./animations/hands_ASL_T.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_U', open('./animations/hands_ASL_U.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_V', open('./animations/hands_ASL_V.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_W', open('./animations/hands_ASL_W.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_X', open('./animations/hands_ASL_X.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_Y', open('./animations/hands_ASL_Y.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_Z', open('./animations/hands_ASL_Z.bvh', "r").read())

        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_1CL', open('./animations/hands_ASL_1CL.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_2CL', open('./animations/hands_ASL_2CL.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_3CL', open('./animations/hands_ASL_3CL.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_4CL', open('./animations/hands_ASL_4CL.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_5aCL', open('./animations/hands_ASL_5aCL.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_5bCL', open('./animations/hands_ASL_5bCL.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_ACL', open('./animations/hands_ASL_ACL.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_BCL', open('./animations/hands_ASL_BCL.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_CCL', open('./animations/hands_ASL_CCL.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_FCL', open('./animations/hands_ASL_FCL.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_ICL', open('./animations/hands_ASL_ICL.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_ILYCL', open('./animations/hands_ASL_ILYCL.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_SCL', open('./animations/hands_ASL_SCL.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_VaCL', open('./animations/hands_ASL_VaCL.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_VbCL', open('./animations/hands_ASL_VbCL.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_YCL', open('./animations/hands_ASL_YCL.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_CbCL', open('./animations/hands_ASL_CbCL.bvh', "r").read())
        self.realizer.addBVHMotionToCharacter(self.name, 'hands_ASL_TCL', open('./animations/hands_ASL_TCL.bvh', "r").read())
        
        firingstevenhigh = open('./animations/alfonse-FiringStevenHigh.bvh', "r").read()

        self.realizer.addBVHMotionToCharacter(self.name, 'FiringStevenHigh', firingstevenhigh )

        print("done")

        self.agent.reparentTo( render )
        
    def update( self ):
        self.realizer.skeletonRequested()
        skeletonHasBeenUpdated = False
        while(not skeletonHasBeenUpdated):
            #wait until skeleton can be displayed
            if(self.realizer.skeletonIsReadyToBeDisplayed(self.name)):
                self.updatePandaSkeleton(self.agentControlJoints, self.agentSMRSkel)
                skeletonHasBeenUpdated = True
                
        for key in self.realizer.getModifiedMorphTargets(self.name): #self.targets.keys(): # Only update the targets that have actually changed here
            #print key, "\n"
            weight = self.realizer.getMorphTargetWeight( self.name, key )
            morphTargets = self.targets[key]
            for morphTargetKey in morphTargets.keys() :
                #print(weight*morphTargets[morphTargetKey])
                morphTargetKey.setX(weight*morphTargets[morphTargetKey])
                
        #blushingValue = self.realizer.getShaderParameterValue( self.name, 'blushing' )
        #self.shaders.headShader.blushing.set( blushingValue )
        
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
    
    def addAnimationWE(self,_actorName,_animationName):
        self.animationAgent.reparentTo(render)
        self.animationAgent.setScale(10)
        agentSMRMotion = SMRPy.SMRMotion()
        agentSMRMotion.setTimeStep(0.04)
        self.realizer.addAnimation(_actorName, _animationName)
        for i in range (self.animationAgent.getNumFrames(_animationName)):
            self.animationAgent.pose(_animationName,i)
            base.graphicsEngine.renderFrame()
            self.updateSMRSkeleton(self.agentNodePaths, self.newSkeleton)
            self.realizer.addPoseToAnimation(_actorName, _animationName, self.newSkeleton)
            agentSMRMotion.insertSkeleton(self.newSkeleton)
        self.animationAgent.detachNode()
        print "animation",_animationName,"added"
        SMRPy.exportMotionToBvh(_animationName+".bvh",agentSMRMotion)

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
