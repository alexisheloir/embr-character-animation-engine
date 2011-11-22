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
import shaders
import shadowmap
import agent

import direct.directbase.DirectStart
from direct.gui.DirectGui import *

import SMRPy
import thread

class World(DirectObject):
    def __init__( self, _realizer ):

        #create the Realizer
        self.realizer = _realizer
    
        self.setDataPath("../../../data/")
        
        #This code puts the standard title and instuction text on screen
        self.title = OnscreenText(text="",
                                  style=1, fg=(1,1,1,1),
                                  pos=(0.8,-0.95), scale = .07)            
        #setup key input
        self.accept('escape', sys.exit)
        self.accept('space', self.toggleCamera )

        #load lighting configuration
        from configobj import ConfigObj
        config = ConfigObj("lighting.cfg")
        config2 = ConfigObj("lighting2.cfg")

        print config
        
        #create new agent
        self.amber = agent.Agent( "Amber" )
        self.amber.setDataPath( self.dataPath )
        self.amber.setActor( self.dataPath + 'models/amber/amberTransHair.bam',
                             {
                              "breathing":self.dataPath + "models/amber/animations/amber-breathing.bam",
                              "hands_claw":self.dataPath + "models/amber/animations/amber-hands_claw.bam",
                              "hands_fist":self.dataPath + "models/amber/animations/amber-hands_fist.bam",
                              "hands_index":self.dataPath + "models/amber/animations/amber-hands_index.bam",
                              "hands_open-relaxed":self.dataPath + "models/amber/animations/amber-hands_open-relaxed.bam",
                              "hands_open-spread":self.dataPath + "models/amber/animations/amber-hands_open-spread.bam",
                              "hands_open-straight":self.dataPath + "models/amber/animations/amber-hands_open-straight.bam",
                              "hands_purse":self.dataPath + "models/amber/animations/amber-hands_purse.bam",
                              "hands_ring":self.dataPath + "models/amber/animations/amber-hands_ring.bam",
                              "endian":self.dataPath + "models/amber/animations/amber-endian.bam",
                              "poseNeutral":self.dataPath + "models/amber/animations/amber-poseNeutral.bam"
                             },
                             {"morph":self.dataPath + "models/amber/animations/amber-morphTargets.bam"} )
        self.amber.setRealizer( self.realizer )
        self.amber.setCameraMask( 2 )
        self.amber.setShadowMapParameters( 1, 25 )
        self.amber.setLightingConfiguration( config, 'Lighting1' )
        self.amber.init()
        self.amber.setTransform( 7,0,0, -90,0,0, 10 )
        self.amber.setShaders( shaders.AmberModel( self.amber.agent, self.amber.lighting, self.dataPath ) )
        
        #create new agent
        self.alfonse = agent.Agent( "Alfonse" )
        self.alfonse.setDataPath( self.dataPath )
        self.alfonse.setActor( self.dataPath + 'models/alfonse/alfonse.bam',
                             {
                              "breathing":self.dataPath + "models/amber/animations/amber-breathing.bam",
                              "hands_claw":self.dataPath + "models/amber/animations/amber-hands_claw.bam",
                              "hands_fist":self.dataPath + "models/amber/animations/amber-hands_fist.bam",
                              "hands_index":self.dataPath + "models/amber/animations/amber-hands_index.bam",
                              "hands_open-relaxed":self.dataPath + "models/amber/animations/amber-hands_open-relaxed.bam",
                              "hands_open-spread":self.dataPath + "models/amber/animations/amber-hands_open-spread.bam",
                              "hands_open-straight":self.dataPath + "models/amber/animations/amber-hands_open-straight.bam",
                              "hands_purse":self.dataPath + "models/amber/animations/amber-hands_purse.bam",
                              "hands_ring":self.dataPath + "models/amber/animations/amber-hands_ring.bam",
                              "endian":self.dataPath + "models/amber/animations/amber-endian.bam",
                              "poseNeutral":self.dataPath + "models/amber/animations/amber-poseNeutral.bam"
                             },
                             {"morph":self.dataPath + "models/alfonse/animations/guy.egg"} )
        self.alfonse.setRealizer( self.realizer )
        self.alfonse.setCameraMask( 1 )
        self.alfonse.setShadowMapParameters( 2, 25 )
        self.alfonse.setLightingConfiguration( config, 'Lighting2' )
        self.alfonse.init()
        self.alfonse.setTransform( -7,0,0, 90,0,0, 10 )
        self.alfonse.setShaders( shaders.AlfonseModel( self.alfonse.agent, self.alfonse.lighting, self.dataPath ) )
        
        
        #create realizer    
        self.realizer.init()
        thread.start_new_thread ( self.realizer.run,())
        
        self.time=0

        taskMgr.doMethodLater(0.02, self.refreshSkels, "refreshSkels")

        base.disableMouse()       #Disable mouse-based camera-control
        #base.camera.setPos(0,-100,0)    #Position the camera
        #base.camera.setHpr(0,0,0)
        
        self.cameraSetup = 1
        self.setCamera()
        
        base.camLens.setFilmSize(24, 36)  # 35 mm film
        base.camLens.setFocalLength(70)   # 70 mm focalLength

        base.oobe()
        #base.useDrive()
        #base.useTrackball()
        
        base.setFrameRateMeter(True)
        
        base.setBackgroundColor(0.4,0.4,0.4)
        
        base.disableAllAudio()
     
    def setDataPath(self, _dataPath):
        self.dataPath = _dataPath;
    
    def setCamera( self ):
        if self.cameraSetup == 1:
            base.camera.setPos(-24,-8,6.5)    #Position the camera
            base.camera.setHpr(-71,0,0)
        elif self.cameraSetup == 2:
            base.camera.setPos(24,-8,6.5)    #Position the camera
            base.camera.setHpr(71,0,0)
    
    def toggleCamera( self ):
        self.cameraSetup += 1
        if self.cameraSetup > 2:
            self.cameraSetup = 1
        self.setCamera()
 
#******************************************************************************************#

    def refreshSkels(self,task):
        
        self.time+=0.02
        
        self.amber.lighting.directionalLights[0].direction = Vec3( math.sin(self.time*0.5), -math.cos(self.time*0.5), 0 )
        #intensity = 0.7
        #self.amber.lighting.ambientColor = Vec4(intensity,intensity,intensity,0)
        self.amber.update()
        
        self.alfonse.lighting.directionalLights[0].direction = Vec3( math.sin(self.time*0.5), -math.cos(self.time*0.5), 0 )
        #intensity = 0.7
        #self.alfonse.lighting.ambientColor = Vec4(intensity,intensity,intensity,0)
        self.alfonse.update()
        
        return Task.again

