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

import direct.directbase.DirectStart
from direct.gui.DirectGui import *


class World(DirectObject):
    def __init__(self):

        self.dataPath = "../../../data/"
        
        #This code puts the standard title and instuction text on screen
        self.title = OnscreenText(text="",
                                  style=1, fg=(1,1,1,1),
                                  pos=(0.8,-0.95), scale = .07)            
        #setup key input
        self.accept('escape', sys.exit)
        #self.accept('space', self.toggleCamera )

        #self.agent = Actor(self.dataPath + 'HeikeL/faceGen.egg', {"morph":self.dataPath + "models/alfonse/animations/guy-morphTargets.bam"})
        
        m = loader.loadModel(self.dataPath + 'HeikeL/faceGen.egg')
        m.reparentTo(render)
        
        self.modelHair1 = m.find("**/hair1")
        #self.modelHair1.hide()
        
        self.modelHair2 = m.find("**/hair2")
        self.modelHair2.hide()
        
        print self.modelHair1
        
        self.modelsHair = []
        self.modelsHair.append( self.modelHair1 )
        self.modelsHair.append( self.modelHair2 )
        
        #print self.agent

        #self.agent.reparentTo(render)
        """self.agent.setPos(7,0,0)
        self.agent.setScale(10)
        self.agent.setHpr(-90,0,0)"""
      
        

        #taskMgr.doMethodLater(0.02, self.refreshSkels, "refreshSkels")

        base.disableMouse()       #Disable mouse-based camera-control
        #base.camera.setPos(0,-100,0)    #Position the camera
        #base.camera.setHpr(0,0,0)
        
        """self.cameraSetup = 1
        self.setCamera()"""
        
        #base.camera.setPos(-24,-8,6.5)    #Position the camera
        #base.camera.setHpr(0,0,0)
        #base.camera.setHpr(-71,0,0)
        #base.camera.lookAt(3,0,5)
        
        base.camLens.setFilmSize(24, 36)  # 35 mm film
        base.camLens.setFocalLength(70)   # 70 mm focalLength
        
        

        base.oobe()
        #base.useDrive()
        #base.useTrackball()
        
        base.setFrameRateMeter(True)
        
        base.setBackgroundColor(0.3,0.3,0.3)
        
        base.disableAllAudio()
        
        #self.initShadowMapping()
        #self.shadowMap = shadowmap.ShadowMap( 1, self.dataPath )
        #self.shadowMap2 = shadowmap.ShadowMap( 2, self.dataPath )
        
        self.addGUI()
    
    # Callback function to set  text
    def selectHair( self, index ):
        #self.textObject.setText( "Button Clicked " + str(index) )
        #self.lightIndex = index
        for i in range(2):
            self.modelsHair[i].hide()
        self.modelsHair[index].show()
        #print "blub"
        
    def addGUI( self ):
        # Add some text
        self.textObject = OnscreenText(text = "Hair Stylist", pos = (0.95,-0.95), scale = 0.07,fg=(1,0.5,0.5,1),align=TextNode.ACenter,mayChange=1)
        
        #imageObject = OnscreenImage(image = self.dataPath + 'textures/cubemaps/autumn_0.png', pos = (0, 0, 0)) 
        
 
        # Add button
        b = DirectButton(text = (""), scale=.15, pos = (-1.15,0.0,0.8), command=self.selectHair, extraArgs=[0], relief = None,
                        image = self.dataPath + 'GUI/hair1.png')
        b.setTransparency(TransparencyAttrib.MAlpha)
        #print b.frameStyle
        
        b = DirectButton(text = (""), scale=.15, pos = (-1.15,0.0,0.5), command=self.selectHair, extraArgs=[1], relief = None,
                        image = self.dataPath + 'GUI/hair2.png')
        b.setTransparency(TransparencyAttrib.MAlpha)
        
        #b = DirectButton(text = ("2"), scale=.05, pos = (-0.95,0,0.95), command=self.selectHair, extraArgs=[1])
        """b = DirectButton(text = ("3"), scale=.05, pos = (-0.9,0,0.95), command=self.selectLight, extraArgs=[2])
        b = DirectButton(text = ("4"), scale=.05, pos = (-0.85,0,0.95), command=self.selectLight, extraArgs=[3])
        
        b = DirectButton(text = ("Show"), scale=.05, pos = (-1,0,0.55), command=self.showLamps, extraArgs=[])
        b = DirectButton(text = ("Hide"), scale=.05, pos = (-0.85,0,0.55), command=self.hideLamps, extraArgs=[])
        
        self.sliderTheta = DirectSlider(range=(0,6.28), value=0, pageSize=2, scale=.3, pos = (-0.7,0,0.9), command=self.updateLights)
        self.sliderPhi = DirectSlider(range=(-3.1416/2,3.1416/2), value=0, pageSize=2, scale=.3, pos = (-0.7,0,0.85), command=self.updateLights)
        self.sliderIntensity = DirectSlider(range=(0,2), value=0, pageSize=2, scale=.3, pos = (-0.7,0,0.8), command=self.updateLights)
        self.sliderHue = DirectSlider(range=(0,360), value=0, pageSize=2, scale=.3, pos = (-0.7,0,0.75), command=self.updateLights)
        self.sliderSaturation = DirectSlider(range=(0,1), value=0, pageSize=2, scale=.3, pos = (-0.7,0,0.7), command=self.updateLights)
        self.sliderAmbient = DirectSlider(range=(0,1), value=0, pageSize=2, scale=.3, pos = (-0.7,0,0.65), command=self.updateLights)

        self.thetas = [ 0, 0, 0, 0 ]
        self.phis = [ 0, 0, 0, 0 ]
        self.intensities = [ 1, 0, 0, 0 ]
        self.hues = [ 0, 0, 0, 0 ]
        self.saturations = [ 0, 0, 0, 0 ]
        
        self.selectLight( 0 )
        self.selectLight( 1 )
        self.selectLight( 2 )
        self.selectLight( 3 )
        self.selectLight( 0 )"""
                
  

w = World()
#PStatClient.connect()
run()              #Run the realizer
