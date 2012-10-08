# Author: Alexis Heloir
# Models and Textures by: Alexis Heloir
# Last Updated: 17/10/08
#

#from pandac.PandaModules import loadPrcFileData
#loadPrcFileData("", "want-directtools #t")
#loadPrcFileData("", "want-tk #t")


import direct.directbase.DirectStart
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
from pandac.PandaModules import Material

from pandac.PandaModules import *

import sys
import math
import copy
import getopt
import ben

import thread

import direct.directbase.DirectStart
from direct.gui.DirectGui import *

import SMRPy

class World(DirectObject):
    def __init__( self, _realizer ):

        self.realizer = _realizer
    
        self.setDataPath("./data/")
        
        #setup key input
        self.accept('escape', sys.exit)
        self.currentCharacter = 1

        self.accept('space', self.sendEMBRScriptKeyboard)
        self.currentCharacter = 1

        #create new agent
        self.ben = ben.Agent( "Ben" )
        self.ben.setDataPath( self.dataPath )
        self.ben.setActor( self.dataPath + 'models/max/max.bam',{},{})
        self.ben.setRealizer( self.realizer )

        self.ben.init()
        self.ben.setTransform( 0,0,0,0,0,0, 10 )
        self.ben.agent.show()

        characterConfigurationFileContent = open("./characterConfiguration.xml", "r").read()
        #create realizer    

        print("read characterConf. file")

        self.realizer.init(characterConfigurationFileContent)

        print("initialized character")

        thread.start_new_thread ( self.realizer.run,())

        print("thread lauched")        

        self.time=0

        if base.appRunner:
            base.appRunner.main.base  = base
            base.appRunner.main.sendEMBRScript  = self.sendEMBRScript

        #base.appRunner.main.base.toggleWireframe()
        #Next door stereo display

        #dr = base.camNode.getDisplayRegion(0)
        #window = dr.getWindow()

        #dr1 = window.makeDisplayRegion(0, 0.5, 0, 1)
        #dr1.setSort(dr.getSort())

        #dr2 = window.makeDisplayRegion(0.5, 1, 0, 1)
        #dr2.setSort(dr.getSort())

        
        base.camLens.setFilmSize(24, 36)  # 35 mm film
        base.camLens.setFocalLength(70)   # 70 mm focalLength

        base.useTrackball()
        base.trackball.node().setPos(0,100,0)
    
        base.setFrameRateMeter(True)        
        base.setBackgroundColor(0.1,0.1,0.4)
        base.disableAllAudio()

        #PStatClient.connect()

        render.setAntialias(AntialiasAttrib.MMultisample) #Enable Antialiasingy
        render.setShaderAuto(True) #Enable perpixel lighting

        taskMgr.doMethodLater(0.02, self.refreshSkels, "refreshSkels")

    def sendEMBRScriptKeyboard(self):
        EMBRScriptSequence="TIME_RESET \n\
BEGIN K_POSE_SEQUENCE  # --- LEXEME:Wie \n\
 CHARACTER:Ben \n\
 START:300 \n\
 FADE_IN:200 \n\
 FADE_OUT:300 \n\
 BEGIN K_POSE  # --- Pose 0 --- SYNC:start \n\
  TIME_POINT:750 \n\
  HOLD:0 \n\
  BEGIN LOOK_AT_CONSTRAINT \n\
    FEEDBACK_ID:feedback_start \n\
    BODY_GROUP:eyes \n\
    TARGET:-0.04;-2;0.2 \n\
  END \n\
  BEGIN ORIENTATION_CONSTRAINT \n\
    FEEDBACK_ID:feedback_start \n\
    BODY_GROUP:headNeck \n\
    DIRECTION:1;-2;1.36 \n\
    JOINT:head \n\
    NORMAL:Zaxis \n\
  END \n\
  BEGIN ORIENTATION_CONSTRAINT \n\
    FEEDBACK_ID:feedback_start \n\
    BODY_GROUP:headNeck \n\
    DIRECTION:-0.04;-0.64;2 \n\
    JOINT:head \n\
    NORMAL:Yaxis \n\
  END \n\
  BEGIN MORPH_TARGET \n\
    FEEDBACK_ID:feedback_start \n\
    MORPH_KEY:ExpSmileClosed \n\
    MORPH_VALUE:0.0 \n\
  END \n\
  BEGIN MORPH_TARGET \n\
    FEEDBACK_ID:feedback_start \n\
    MORPH_KEY:ModBlinkLeft \n\
    MORPH_VALUE:0.21 \n\
  END \n\
  BEGIN MORPH_TARGET \n\
    FEEDBACK_ID:feedback_start \n\
    MORPH_KEY:ModBlinkRight \n\
    MORPH_VALUE:0.21 \n\
  END \n\
  BEGIN MORPH_TARGET \n\
    FEEDBACK_ID:feedback_start \n\
    MORPH_KEY:ModEyeSquintLeft \n\
    MORPH_VALUE:0.39 \n\
  END \n\
  BEGIN MORPH_TARGET \n\
    FEEDBACK_ID:feedback_start \n\
    MORPH_KEY:ModEyeSquintRight \n\
    MORPH_VALUE:0.39 \n\
  END \n\
  BEGIN MORPH_TARGET \n\
    FEEDBACK_ID:feedback_start \n\
    MORPH_KEY:ModBrowUpLeft \n\
    MORPH_VALUE:0.51 \n\
  END \n\
  BEGIN MORPH_TARGET \n\
    FEEDBACK_ID:feedback_start \n\
    MORPH_KEY:ModBrowUpRight \n\
    MORPH_VALUE:0.525 \n\
  END \n\
  BEGIN MORPH_TARGET \n\
    FEEDBACK_ID:feedback_start \n\
    MORPH_KEY:ModBrowDownLeft \n\
    MORPH_VALUE:1.02 \n\
  END \n\
  BEGIN MORPH_TARGET \n\
    FEEDBACK_ID:feedback_start \n\
    MORPH_KEY:ModBrowDownRight \n\
    MORPH_VALUE:1.02 \n\
  END \n\
  BEGIN MORPH_TARGET \n\
    FEEDBACK_ID:feedback_start \n\
    MORPH_KEY:ExpSmileOpen \n\
    MORPH_VALUE:0.0 \n\
  END \n\
  BEGIN MORPH_TARGET \n\
    FEEDBACK_ID:feedback_start \n\
    MORPH_KEY:Phonoh \n\
    MORPH_VALUE:0.0 \n\
  END \n\
  BEGIN MORPH_TARGET \n\
    FEEDBACK_ID:feedback_start \n\
    MORPH_KEY:Phonbigaah \n\
    MORPH_VALUE:0.27 \n\
  END \n\
  BEGIN POSE_TARGET \n\
    FEEDBACK_ID:feedback_start \n\
    BODY_GROUP:lhand \n\
    POSE_KEY:hands_DGS_Y \n\
  END \n\
  BEGIN POSITION_CONSTRAINT \n\
    FEEDBACK_ID:feedback_start \n\
    BODY_GROUP:larm \n\
    TARGET:0.1;-0.16;0.4 \n\
    JOINT:lhand \n\
    OFFSET:0;0;0 \n\
  END \n\
  BEGIN ORIENTATION_CONSTRAINT \n\
    FEEDBACK_ID:feedback_start \n\
    BODY_GROUP:larm \n\
    DIRECTION:-0.06;-0.06;0.86 \n\
    JOINT:lhand \n\
    NORMAL:Yaxis \n\
  END \n\
  BEGIN ORIENTATION_CONSTRAINT \n\
    FEEDBACK_ID:feedback_start \n\
    BODY_GROUP:larm \n\
    DIRECTION:0;-1;0 \n\
    JOINT:lhand \n\
    NORMAL:Zaxis \n\
  END \n\
  BEGIN POSITION_CONSTRAINT \n\
    FEEDBACK_ID:feedback_start \n\
    BODY_GROUP:spine \n\
    TARGET:-0.08;0.4;1.84 \n\
    JOINT:spine4 \n\
    OFFSET:0;0;0 \n\
  END \n\
  BEGIN SWIVEL_CONSTRAINT \n\
    FEEDBACK_ID:feedback_start \n\
    BODY_GROUP:larm \n\
    SWIVEL_ANGLE:10.800000000000011 \n\
  END \n\
  BEGIN POSE_TARGET \n\
    FEEDBACK_ID:feedback_start \n\
    BODY_GROUP:rhand \n\
    POSE_KEY:hands_DGS_Y \n\
  END \n\
  BEGIN POSITION_CONSTRAINT \n\
    FEEDBACK_ID:feedback_start \n\
    BODY_GROUP:rarm \n\
    TARGET:-0.08;-0.16;0.4 \n\
    JOINT:rhand \n\
    OFFSET:0;0;0 \n\
  END \n\
  BEGIN ORIENTATION_CONSTRAINT \n\
    FEEDBACK_ID:feedback_start \n\
    BODY_GROUP:rarm \n\
    DIRECTION:0.1;-0.12;0.56 \n\
    JOINT:rhand \n\
    NORMAL:Yaxis \n\
  END \n\
  BEGIN ORIENTATION_CONSTRAINT \n\
    FEEDBACK_ID:feedback_start \n\
    BODY_GROUP:rarm \n\
    DIRECTION:-0.04;-1;-0.04 \n\
    JOINT:rhand \n\
    NORMAL:Zaxis \n\
  END \n\
  BEGIN SWIVEL_CONSTRAINT \n\
    FEEDBACK_ID:feedback_start \n\
    BODY_GROUP:rarm \n\
    SWIVEL_ANGLE:14.400000000000006 \n\
  END \n\
 END \n\
 END \n\
"
        self.sendEMBRScript(EMBRScriptSequence);

    def sendEMBRScript(self, EMBRScript):
        self.realizer.sendEMBRScript(EMBRScript)
    
    def setDataPath(self, _dataPath):
        self.dataPath = _dataPath

    def refreshSkels(self,task):        
        self.time+=0.02
        self.ben.update()
        return Task.again

