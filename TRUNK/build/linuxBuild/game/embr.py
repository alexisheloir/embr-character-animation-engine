# Author: Alexis Heloir
# Models and Textures by: Alexis Heloir
# Last Updated: 17/10/08
#
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
import thread
import sys
import math
import copy
import getopt
import shaders
import shadowmap

import agent

class World(DirectObject):
    def __init__( self, _realizer, _useAdvancedShaders ):

        self.setDataPath("../../../data/")

        self.realizer = _realizer
        self.useAdvancedShaders = _useAdvancedShaders

        #This code puts the standard title and instuction text on screen
        self.title = OnscreenText(text="",
                                  style=1, fg=(1,1,1,1),
                                  pos=(0.8,-0.95), scale = .07)            
        #setup key input
        self.accept('escape', sys.exit)

#         base.disableMouse()       #Disable mouse-based camera-control
#         camera.setPos(0,-100,0)    #Position the camera
#         camera.setHpr(0,0,0)
        base.camLens.setFilmSize(24, 36)  # 35 mm film
        base.camLens.setFocalLength(70)   # 70 mm focalLength
#         base.oobe()                       # out od body experience (camera debug)
#         base.setFrameRateMeter(True)

        self.ground = loader.loadModel( self.dataPath + 'models/ground/ground.bam')
        self.ground.reparentTo(render)
        self.ground.setScale(10)
        self.ground.setPos(0,0,-0.6)    #Position the ground


        #load lighting configuration
        from configobj import ConfigObj
        config = ConfigObj("lighting.cfg")
        print config
        
        #create new agent
        self.amber = agent.Agent( "Amber" )
        self.amber.setDataPath( self.dataPath )
        self.amber.setActor( self.dataPath + 'models/amber/amber.bam',
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
                             },
                             {"morph":self.dataPath + "models/amber/animations/amber-morphTargets.bam"} )
        self.amber.setRealizer( self.realizer )
        self.amber.setCameraMask( 2 )
        if self.useAdvancedShaders == 1:
            self.amber.setShadowMapParameters( 1, 25.0 )
        self.amber.setLightingConfiguration( config, 'Lighting' )
        self.amber.init()
        self.amber.setTransform( 0,0,0, 0,0,0, 10 )
        
        shaderPath = "shadersSimple/"
        if self.useAdvancedShaders == 1:
            shaderPath = "shaders/"
        
        self.amber.setShaders( shaders.AmberModel( self.amber.agent, self.amber.lighting, self.dataPath, self.dataPath + shaderPath ) )
        
        
        self.time=0

        
        self.realizer.init()
        thread.start_new_thread ( self.realizer.run,())

        taskMgr.doMethodLater(0.02, self.refreshSkels, "refreshSkels")

        base.disableMouse()       #Disable mouse-based camera-control
        base.camera.setPos(0,-100,0)    #Position the camera
        base.camera.setHpr(0,0,0)
        base.camLens.setFilmSize(24, 36)  # 35 mm film
        base.camLens.setFocalLength(70)   # 70 mm focalLength
        base.useTrackball()
        base.trackball.node().setPos(0,100,0)
        
        base.setFrameRateMeter(True)
        

#******************************************************************************************#
    
        
    def setDataPath(self, _dataPath):
        self.dataPath = _dataPath;

    def setRealizer(self, _realizer):
        self.realizer = _realizer

    


    def increaseIncrement(self):
        self.increment += 0.01

    def decreaseIncrement(self):
        self.increment -= 0.01
    


    def refreshSkels(self,task):
           
        self.amber.update()

        return Task.again

    
