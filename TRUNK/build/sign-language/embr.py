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

        self.realizer = _realizer
    
        self.setDataPath("../../data/")
        
        #setup key input
        self.accept('escape', sys.exit)
        self.accept('c', self.toggleCharacter )
        self.currentCharacter = 1

        #self.ground = loader.loadModel( self.dataPath + 'models/ground/ground.bam')
        #self.ground.reparentTo(render)
        #self.ground.setScale(10)
        #self.ground.setPos(0,0,-0.6)    #Position the ground

        #load lighting configuration
        from configobj import ConfigObj
        config = ConfigObj("lighting.cfg")
        character1 = config[ 'Character1' ]
        character2 = config[ 'Character2' ]
        preset1 = character1['Lighting']
        preset2 = character2['Lighting']
        #print preset1
        #print preset2
        #print config

        
        #create new agent
        self.amber = agent.Agent( "Amber" )
        self.amber.setDataPath( self.dataPath )
        self.amber.setActor( self.dataPath + 'models/amber/amber.bam',{},{"morph":self.dataPath + "models/amber/animations/amber-morphTargets.bam"} )
        self.amber.setRealizer( self.realizer )

        #self.amber.setCameraMask( 1 )
        #self.amber.setShadowMapParameters( 1, 25.0 )
        self.amber.setLightingConfiguration( config, preset1 )
        self.amber.init()
        self.amber.setTransform( 0,0,0,0,0,0, 10 )
        self.amber.setShaders( shaders.AmberModel( self.amber.agent, self.amber.lighting, self.dataPath ) )
        self.amber.agent.hide()
        
        #create new agent
        self.alfonse = agent.Agent( "Alfonse" )
        self.alfonse.setDataPath( self.dataPath )
        self.alfonse.setActor( self.dataPath + 'models/alfonse/alfonse.bam',{},{"morph":self.dataPath + "models/amber/animations/amber-morphTargets.bam"} )
        self.alfonse.setRealizer( self.realizer )
        #self.alfonse.setCameraMask( 2 )
        #self.alfonse.setShadowMapParameters( 2, 25.0 )
        self.alfonse.setLightingConfiguration( config, preset2 )
        self.alfonse.init()
        self.alfonse.setTransform( 0,0,0,0,0,0, 10 )
        self.alfonse.setShaders( shaders.AlfonseModel( self.alfonse.agent, self.alfonse.lighting, self.dataPath ) )
        self.alfonse.agent.show()

        characterConfigurationFileContent = open("./characterConfiguration.xml", "r").read()
        #create realizer    

        print("read characterConf. file")

        #create realizer    
        self.realizer.init(characterConfigurationFileContent)
       
        thread.start_new_thread ( self.realizer.run,())

        self.time=0
        #self.amber.lighting.copy( self.alfonse.lighting )
        
        base.camLens.setFilmSize(24, 36)  # 35 mm film
        base.camLens.setFocalLength(70)   # 70 mm focalLength

        base.useTrackball()
        base.trackball.node().setPos(0,100,0)
    
        base.setFrameRateMeter(True)        
        base.setBackgroundColor(0.1,0.1,0.4)
        base.disableAllAudio()

        render.setAntialias(AntialiasAttrib.MMultisample)

        taskMgr.doMethodLater(0.02, self.refreshSkels, "refreshSkels")
    
    
    def setDataPath(self, _dataPath):
        self.dataPath = _dataPath;
    
    def setCamera( self ):
        if self.cameraSetup == 1:
            base.camera.setPos(0,-50,10)    #Position the camera
            base.camera.lookAt(0,0,4)
            #base.camera.setHpr(-71,0,0)
        elif self.cameraSetup == 2:
            base.camera.setPos(3,-12,8.5)    #Position the camera
            base.camera.lookAt(0,0,7)
            #base.camera.setHpr(71,0,0)
        elif self.cameraSetup == 3:
            base.camera.setPos(12,-10,7.5)    #Position the camera
            base.camera.lookAt(0,0,6.5)
            #base.camera.setHpr(71,0,0)
        elif self.cameraSetup == 4:
            base.camera.setPos(24,-8,7.5)    #Position the camera
            base.camera.lookAt(0,0,6.5)
            #base.camera.setHpr(71,0,0)

        
    def toggleCharacter( self ):
      if self.currentCharacter == 1:
        self.currentCharacter = 2
        
        self.amber.agent.show()
        self.alfonse.agent.hide()
      else:
        self.currentCharacter = 1
        
        self.alfonse.agent.show()
        self.amber.agent.hide()
 
#******************************************************************************************#

    def refreshSkels(self,task):
        
        self.time+=0.02
        
        if (self.currentCharacter == 2):
         self.amber.update()
        else:
         self.alfonse.update()
        
        return Task.again

