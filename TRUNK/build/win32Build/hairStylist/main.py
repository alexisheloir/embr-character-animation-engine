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
        
        m = loader.loadModel(self.dataPath + 'HeikeL/faceGen.egg')
        m.reparentTo(render)
        
        #add hair models
        self.numModelsHair = 2
        self.modelsHair = []
        print "Number of hair models: " + str( self.numModelsHair )
        
        for i in range( self.numModelsHair ):
            modelHair = m.find("**/hair" + str( i + 1 ) )
            print modelHair
            self.modelsHair.append( modelHair )
        
        self.selectHair( 0 )
        
        base.disableMouse()       #Disable mouse-based camera-control
        
        base.camLens.setFilmSize(24, 36)  # 35 mm film
        base.camLens.setFocalLength(70)   # 70 mm focalLength
        
        base.oobe()
        
        base.setFrameRateMeter(True)
        base.setBackgroundColor(0.3,0.3,0.3)
        base.disableAllAudio()
        
        self.addGUI()
    
    # Callback function to set hair
    def selectHair( self, index ):
        for i in range( self.numModelsHair ):
            self.modelsHair[i].hide()
        self.modelsHair[index].show()
        
    def addGUI( self ):
        # Add some text
        self.textObject = OnscreenText(text = "Hair Stylist", pos = (0.95,-0.95), scale = 0.07,fg=(1,0.5,0.5,1),align=TextNode.ACenter,mayChange=1)
        
        # Add buttons
        for i in range( self.numModelsHair ):
            b = DirectButton(text = (""), scale=.15, pos = (-1.15,0.0,0.8 - 0.3*i), command=self.selectHair, extraArgs=[i], relief = None,
                    image = self.dataPath + 'GUI/hair' + str( i+1 ) + '.png')
            b.setTransparency(TransparencyAttrib.MAlpha)
    
  

w = World()
#PStatClient.connect()
run()              #Run the realizer
