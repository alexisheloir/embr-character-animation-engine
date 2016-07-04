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
#import lightingGUI

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
        #self.title = OnscreenText(text="",
        #                          style=1, fg=(1,1,1,1),
        #                          pos=(0.8,-0.95), scale = .07)            
        #setup key input
        self.accept('escape', sys.exit)
        self.accept('space', self.toggleCamera )
        self.accept('c', self.toggleCharacter )
        self.currentCharacter = 1

        self.ground = loader.loadModel( self.dataPath + 'models/ground/ground.bam')
        self.ground.reparentTo(render)
        self.ground.setScale(10)
        self.ground.setPos(0,0,-0.6)    #Position the ground

        #load lighting configuration
        from configobj import ConfigObj
        config = ConfigObj("lighting.cfg")
        character1 = config[ 'Character1' ]
        character2 = config[ 'Character2' ]
        preset1 = character1['Lighting']
        preset2 = character2['Lighting']
        print preset1
        print preset2
        print config

        
        #create new agent
        self.amber = agent.Agent( "Amber" )
        self.amber.setDataPath( self.dataPath )
        self.amber.setActor( self.dataPath + 'models/amber/amber.bam',# pascal: instead of amberWithTransHair.bam import the amber.bam with the correct skeleton
                              {
                              # "breathing":self.dataPath + "models/amber/animations/amber-breathing.bam",
                              # "hands_claw":self.dataPath + "models/amber/animations/amber-hands_claw.bam",
                              # "hands_fist":self.dataPath + "models/amber/animations/amber-hands_fist.bam",
                              # "hands_index":self.dataPath + "models/amber/animations/amber-hands_index.bam",
                              # "hands_open-relaxed":self.dataPath + "models/amber/animations/amber-hands_open-relaxed.bam",
                              # "hands_open-spread":self.dataPath + "models/amber/animations/amber-hands_open-spread.bam",
                              # "hands_open-straight":self.dataPath + "models/amber/animations/amber-hands_open-straight.bam",
                              # "hands_purse":self.dataPath + "models/amber/animations/amber-hands_purse.bam",
                              # "hands_ring":self.dataPath + "models/amber/animations/amber-hands_ring.bam",
                              # "endian":self.dataPath + "models/amber/animations/amber-endian.bam",
                              # "shrug":self.dataPath + "models/amber/animations/amber-shrug.bam",
                              # "poseNeutral":self.dataPath + "models/amber/animations/amber-poseNeutral.bam",                             
                              # "hands_DGS_A":self.dataPath + "models/amber/animations/amber-hands_DGS_A.bam",
                              # "hands_DGS_B":self.dataPath + "models/amber/animations/amber-hands_DGS_B.bam",
                              # "hands_DGS_C":self.dataPath + "models/amber/animations/amber-hands_DGS_C.bam",
                              # "hands_DGS_D":self.dataPath + "models/amber/animations/amber-hands_DGS_D.bam",
                              # "hands_DGS_E":self.dataPath + "models/amber/animations/amber-hands_DGS_E.bam",
                              # "hands_DGS_F":self.dataPath + "models/amber/animations/amber-hands_DGS_F.bam",
                              # "hands_DGS_G":self.dataPath + "models/amber/animations/amber-hands_DGS_G.bam",
                              # "hands_DGS_H":self.dataPath + "models/amber/animations/amber-hands_DGS_H.bam",
                              # "hands_DGS_I":self.dataPath + "models/amber/animations/amber-hands_DGS_I.bam",
                              # "hands_DGS_J":self.dataPath + "models/amber/animations/amber-hands_DGS_J.bam",
                              # "hands_DGS_K":self.dataPath + "models/amber/animations/amber-hands_DGS_K.bam",
                              # "hands_DGS_L":self.dataPath + "models/amber/animations/amber-hands_DGS_L.bam",
                              # "hands_DGS_M":self.dataPath + "models/amber/animations/amber-hands_DGS_M.bam",
                              # "hands_DGS_N":self.dataPath + "models/amber/animations/amber-hands_DGS_N.bam",
                              # "hands_DGS_O":self.dataPath + "models/amber/animations/amber-hands_DGS_O.bam",
                              # "hands_DGS_P":self.dataPath + "models/amber/animations/amber-hands_DGS_P.bam",
                              # "hands_DGS_Q":self.dataPath + "models/amber/animations/amber-hands_DGS_Q.bam",
                              # "hands_DGS_R":self.dataPath + "models/amber/animations/amber-hands_DGS_R.bam",
                              # "hands_DGS_T":self.dataPath + "models/amber/animations/amber-hands_DGS_T.bam",
                              # "hands_DGS_U":self.dataPath + "models/amber/animations/amber-hands_DGS_U.bam",
                              # "hands_DGS_V":self.dataPath + "models/amber/animations/amber-hands_DGS_V.bam",
                              # "hands_DGS_W":self.dataPath + "models/amber/animations/amber-hands_DGS_W.bam",
                              # "hands_DGS_X":self.dataPath + "models/amber/animations/amber-hands_DGS_X.bam",
                              # "hands_DGS_Y":self.dataPath + "models/amber/animations/amber-hands_DGS_Y.bam",
                              # "hands_DGS_Z":self.dataPath + "models/amber/animations/amber-hands_DGS_Z.bam",
                              # "hands_DGS_SCH":self.dataPath + "models/amber/animations/amber-hands_DGS_SCH.bam",
                              # "hands_ASL_A":self.dataPath + "models/amber/animations/amber-hands_DGS_A.bam",
                              # "hands_ASL_B":self.dataPath + "models/amber/animations/amber-hands_DGS_B.bam",
                              # "hands_ASL_C":self.dataPath + "models/amber/animations/amber-hands_DGS_C.bam",
                              # "hands_ASL_D":self.dataPath + "models/amber/animations/amber-hands_DGS_D.bam",
                              # "hands_ASL_E":self.dataPath + "models/amber/animations/amber-hands_DGS_E.bam",
                              # "hands_ASL_F":self.dataPath + "models/amber/animations/amber-hands_DGS_F.bam",
                              # "hands_ASL_G":self.dataPath + "models/amber/animations/amber-hands_ASL_G.bam",
                              # "hands_ASL_H":self.dataPath + "models/amber/animations/amber-hands_DGS_H.bam",
                              # "hands_ASL_I":self.dataPath + "models/amber/animations/amber-hands_DGS_I.bam",
                              # "hands_ASL_J":self.dataPath + "models/amber/animations/amber-hands_DGS_J.bam",
                              # "hands_ASL_K":self.dataPath + "models/amber/animations/amber-hands_DGS_K.bam",
                              # "hands_ASL_L":self.dataPath + "models/amber/animations/amber-hands_DGS_L.bam",
                              # "hands_ASL_M":self.dataPath + "models/amber/animations/amber-hands_ASL_M.bam",
                              # "hands_ASL_N":self.dataPath + "models/amber/animations/amber-hands_ASL_N.bam",
                              # "hands_ASL_O":self.dataPath + "models/amber/animations/amber-hands_DGS_O.bam",
                              # "hands_ASL_P":self.dataPath + "models/amber/animations/amber-hands_DGS_P.bam",
                              # "hands_ASL_Q":self.dataPath + "models/amber/animations/amber-hands_DGS_Q.bam",
                              # "hands_ASL_R":self.dataPath + "models/amber/animations/amber-hands_DGS_R.bam",
                              # "hands_ASL_T":self.dataPath + "models/amber/animations/amber-hands_ASL_T.bam",
                              # "hands_ASL_U":self.dataPath + "models/amber/animations/amber-hands_DGS_U.bam",
                              # "hands_ASL_V":self.dataPath + "models/amber/animations/amber-hands_DGS_V.bam",
                              # "hands_ASL_W":self.dataPath + "models/amber/animations/amber-hands_DGS_W.bam",
                              # "hands_ASL_X":self.dataPath + "models/amber/animations/amber-hands_DGS_X.bam",
                              # "hands_ASL_Y":self.dataPath + "models/amber/animations/amber-hands_DGS_Y.bam",
                              # "hands_ASL_Z":self.dataPath + "models/amber/animations/amber-hands_DGS_Z.bam",
                              # "hands_ASL_1CL":self.dataPath + "models/amber/animations/amber-hands_ASL_1CL.bam",
                              # "hands_ASL_2CL":self.dataPath + "models/amber/animations/amber-hands_ASL_2CL.bam",
                              # "hands_ASL_3CL":self.dataPath + "models/amber/animations/amber-hands_ASL_3CL.bam",
                              # "hands_ASL_4CL":self.dataPath + "models/amber/animations/amber-hands_ASL_4CL.bam",
                              # "hands_ASL_5aCL":self.dataPath + "models/amber/animations/amber-hands_ASL_5aCL.bam",
                              # "hands_ASL_5bCL":self.dataPath + "models/amber/animations/amber-hands_ASL_5bCL.bam",
                              # "hands_ASL_ACL":self.dataPath + "models/amber/animations/amber-hands_ASL_ACL.bam",
                              # "hands_ASL_BCL":self.dataPath + "models/amber/animations/amber-hands_ASL_BCL.bam",
                              # "hands_ASL_CCL":self.dataPath + "models/amber/animations/amber-hands_ASL_CCL.bam",
                              # "hands_ASL_FCL":self.dataPath + "models/amber/animations/amber-hands_ASL_FCL.bam",
                              # "hands_ASL_GCL":self.dataPath + "models/amber/animations/amber-hands_ASL_GCL.bam",
                              # "hands_ASL_ICL":self.dataPath + "models/amber/animations/amber-hands_ASL_ICL.bam",
                              # "hands_ASL_ILYCL":self.dataPath + "models/amber/animations/amber-hands_ASL_ILYCL.bam",
                              # "hands_ASL_SCL":self.dataPath + "models/amber/animations/amber-hands_ASL_SCL.bam",
                              # "hands_ASL_VaCL":self.dataPath + "models/amber/animations/amber-hands_ASL_VaCL.bam",
                              # "hands_ASL_VbCL":self.dataPath + "models/amber/animations/amber-hands_ASL_VbCL.bam",
                              # "hands_ASL_YCL":self.dataPath + "models/amber/animations/amber-hands_ASL_YCL.bam",
                              # "hands_ASL_CbCL":self.dataPath + "models/amber/animations/amber-hands_ASL_CbCL.bam",
                              # "hands_ASL_TCL":self.dataPath + "models/amber/animations/amber-hands_ASL_TCL.bam"
                              },
                              {"morph":self.dataPath + "models/amber/animations/amber-morphTargets.bam"} )
        self.amber.setRealizer( self.realizer )
        self.amber.setCameraMask( 2 )
        #self.amber.setShadowMapParameters( 1, 25.0 )
        self.amber.setLightingConfiguration( config, preset1 )
        self.amber.init()
        self.amber.setTransform( 7,0,0, -45,0,0, 10 )
        self.amber.setShaders( shaders.AmberModel( self.amber.agent, self.amber.lighting, self.dataPath ) )
        self.amber.agent.hide()
        
        #create new agent
        self.alfonse = agent.Agent( "Alfonse" )
        self.alfonse.setDataPath( self.dataPath )
        self.alfonse.setActor( self.dataPath + 'models/alfonse/alfonse.bam',
                             {
                              # "breathing":self.dataPath + "models/amber/animations/amber-breathing.bam",
                              # "hands_claw":self.dataPath + "models/amber/animations/amber-hands_claw.bam",
                              # "hands_fist":self.dataPath + "models/amber/animations/amber-hands_fist.bam",
                              # "hands_index":self.dataPath + "models/amber/animations/amber-hands_index.bam",
                              # "hands_open-relaxed":self.dataPath + "models/amber/animations/amber-hands_open-relaxed.bam",
                              # "hands_open-spread":self.dataPath + "models/amber/animations/amber-hands_open-spread.bam",
                              # "hands_open-straight":self.dataPath + "models/amber/animations/amber-hands_open-straight.bam",
                              # "hands_purse":self.dataPath + "models/amber/animations/amber-hands_purse.bam",
                              # "hands_ring":self.dataPath + "models/amber/animations/amber-hands_ring.bam",
                              # "endian":self.dataPath + "models/amber/animations/amber-endian.bam",
                              # "shrug":self.dataPath + "models/amber/animations/amber-shrug.bam",

                              # "mocapTest":self.dataPath + "models/alfonse/animations/alfonse.bam",

                              # "poseNeutral":self.dataPath + "models/amber/animations/amber-poseNeutral.bam",                             
                              # "hands_DGS_A":self.dataPath + "models/amber/animations/amber-hands_DGS_A.bam",
                              # "hands_DGS_B":self.dataPath + "models/amber/animations/amber-hands_DGS_B.bam",
                              # "hands_DGS_C":self.dataPath + "models/amber/animations/amber-hands_DGS_C.bam",
                              # "hands_DGS_D":self.dataPath + "models/amber/animations/amber-hands_DGS_D.bam",
                              # "hands_DGS_E":self.dataPath + "models/amber/animations/amber-hands_DGS_E.bam",
                              # "hands_DGS_F":self.dataPath + "models/amber/animations/amber-hands_DGS_F.bam",
                              # "hands_DGS_G":self.dataPath + "models/amber/animations/amber-hands_DGS_G.bam",
                              # "hands_DGS_H":self.dataPath + "models/amber/animations/amber-hands_DGS_H.bam",
                              # "hands_DGS_I":self.dataPath + "models/amber/animations/amber-hands_DGS_I.bam",
                              # "hands_DGS_J":self.dataPath + "models/amber/animations/amber-hands_DGS_J.bam",
                              # "hands_DGS_K":self.dataPath + "models/amber/animations/amber-hands_DGS_K.bam",
                              # "hands_DGS_L":self.dataPath + "models/amber/animations/amber-hands_DGS_L.bam",
                              # "hands_DGS_M":self.dataPath + "models/amber/animations/amber-hands_DGS_M.bam",
                              # "hands_DGS_N":self.dataPath + "models/amber/animations/amber-hands_DGS_N.bam",
                              # "hands_DGS_O":self.dataPath + "models/amber/animations/amber-hands_DGS_O.bam",
                              # "hands_DGS_P":self.dataPath + "models/amber/animations/amber-hands_DGS_P.bam",
                              # "hands_DGS_Q":self.dataPath + "models/amber/animations/amber-hands_DGS_Q.bam",
                              # "hands_DGS_R":self.dataPath + "models/amber/animations/amber-hands_DGS_R.bam",
                              # "hands_DGS_T":self.dataPath + "models/amber/animations/amber-hands_DGS_T.bam",
                              # "hands_DGS_U":self.dataPath + "models/amber/animations/amber-hands_DGS_U.bam",
                              # "hands_DGS_V":self.dataPath + "models/amber/animations/amber-hands_DGS_V.bam",
                              # "hands_DGS_W":self.dataPath + "models/amber/animations/amber-hands_DGS_W.bam",
                              # "hands_DGS_X":self.dataPath + "models/amber/animations/amber-hands_DGS_X.bam",
                              # "hands_DGS_Y":self.dataPath + "models/amber/animations/amber-hands_DGS_Y.bam",
                              # "hands_DGS_Z":self.dataPath + "models/amber/animations/amber-hands_DGS_Z.bam",
                              # "hands_DGS_SCH":self.dataPath + "models/amber/animations/amber-hands_DGS_SCH.bam",
                              # "hands_ASL_A":self.dataPath + "models/amber/animations/amber-hands_DGS_A.bam",
                              # "hands_ASL_B":self.dataPath + "models/amber/animations/amber-hands_DGS_B.bam",
                              # "hands_ASL_C":self.dataPath + "models/amber/animations/amber-hands_DGS_C.bam",
                              # "hands_ASL_D":self.dataPath + "models/amber/animations/amber-hands_DGS_D.bam",
                              # "hands_ASL_E":self.dataPath + "models/amber/animations/amber-hands_DGS_E.bam",
                              # "hands_ASL_F":self.dataPath + "models/amber/animations/amber-hands_DGS_F.bam",
                              # "hands_ASL_G":self.dataPath + "models/amber/animations/amber-hands_ASL_G.bam",
                              # "hands_ASL_H":self.dataPath + "models/amber/animations/amber-hands_DGS_H.bam",
                              # "hands_ASL_I":self.dataPath + "models/amber/animations/amber-hands_DGS_I.bam",
                              # "hands_ASL_J":self.dataPath + "models/amber/animations/amber-hands_DGS_J.bam",
                              # "hands_ASL_K":self.dataPath + "models/amber/animations/amber-hands_DGS_K.bam",
                              # "hands_ASL_L":self.dataPath + "models/amber/animations/amber-hands_DGS_L.bam",
                              # "hands_ASL_M":self.dataPath + "models/amber/animations/amber-hands_ASL_M.bam",
                              # "hands_ASL_N":self.dataPath + "models/amber/animations/amber-hands_ASL_N.bam",
                              # "hands_ASL_O":self.dataPath + "models/amber/animations/amber-hands_DGS_O.bam",
                              # "hands_ASL_P":self.dataPath + "models/amber/animations/amber-hands_DGS_P.bam",
                              # "hands_ASL_Q":self.dataPath + "models/amber/animations/amber-hands_DGS_Q.bam",
                              # "hands_ASL_R":self.dataPath + "models/amber/animations/amber-hands_DGS_R.bam",
                              # "hands_ASL_T":self.dataPath + "models/amber/animations/amber-hands_ASL_T.bam",
                              # "hands_ASL_U":self.dataPath + "models/amber/animations/amber-hands_DGS_U.bam",
                              # "hands_ASL_V":self.dataPath + "models/amber/animations/amber-hands_DGS_V.bam",
                              # "hands_ASL_W":self.dataPath + "models/amber/animations/amber-hands_DGS_W.bam",
                              # "hands_ASL_X":self.dataPath + "models/amber/animations/amber-hands_DGS_X.bam",
                              # "hands_ASL_Y":self.dataPath + "models/amber/animations/amber-hands_DGS_Y.bam",
                              # "hands_ASL_Z":self.dataPath + "models/amber/animations/amber-hands_DGS_Z.bam",
                              # "hands_ASL_1CL":self.dataPath + "models/amber/animations/amber-hands_ASL_1CL.bam",
                              # "hands_ASL_2CL":self.dataPath + "models/amber/animations/amber-hands_ASL_2CL.bam",
                              # "hands_ASL_3CL":self.dataPath + "models/amber/animations/amber-hands_ASL_3CL.bam",
                              # "hands_ASL_4CL":self.dataPath + "models/amber/animations/amber-hands_ASL_4CL.bam",
                              # "hands_ASL_5aCL":self.dataPath + "models/amber/animations/amber-hands_ASL_5aCL.bam",
                              # "hands_ASL_5bCL":self.dataPath + "models/amber/animations/amber-hands_ASL_5bCL.bam",
                              # "hands_ASL_ACL":self.dataPath + "models/amber/animations/amber-hands_ASL_ACL.bam",
                              # "hands_ASL_BCL":self.dataPath + "models/amber/animations/amber-hands_ASL_BCL.bam",
                              # "hands_ASL_CCL":self.dataPath + "models/amber/animations/amber-hands_ASL_CCL.bam",
                              # "hands_ASL_FCL":self.dataPath + "models/amber/animations/amber-hands_ASL_FCL.bam",
                              # "hands_ASL_GCL":self.dataPath + "models/amber/animations/amber-hands_ASL_GCL.bam",
                              # "hands_ASL_ICL":self.dataPath + "models/amber/animations/amber-hands_ASL_ICL.bam",
                              # "hands_ASL_ILYCL":self.dataPath + "models/amber/animations/amber-hands_ASL_ILYCL.bam",
                              # "hands_ASL_SCL":self.dataPath + "models/amber/animations/amber-hands_ASL_SCL.bam",
                              # "hands_ASL_VaCL":self.dataPath + "models/amber/animations/amber-hands_ASL_VaCL.bam",
                              # "hands_ASL_VbCL":self.dataPath + "models/amber/animations/amber-hands_ASL_VbCL.bam",
                              # "hands_ASL_YCL":self.dataPath + "models/amber/animations/amber-hands_ASL_YCL.bam",
                              # "hands_ASL_CbCL":self.dataPath + "models/amber/animations/amber-hands_ASL_CbCL.bam",
                              # "hands_ASL_TCL":self.dataPath + "models/amber/animations/amber-hands_ASL_TCL.bam"
                             },
                             {"morph":self.dataPath + "models/amber/animations/amber-morphTargets.bam"} )
        self.alfonse.setRealizer( self.realizer )
        self.alfonse.setCameraMask( 2 )
        self.alfonse.setShadowMapParameters( 1, 25.0 )
        self.alfonse.setLightingConfiguration( config, preset2 )
        self.alfonse.init()
        self.alfonse.setTransform( -7,0,0, 45,0,0, 10 )
        self.alfonse.setShaders( shaders.AlfonseModel( self.alfonse.agent, self.alfonse.lighting, self.dataPath ) )

        self.amber.agent.show()
        self.alfonse.agent.show()
        
        #lighting
        #self.lightingGUI = lightingGUI.LightingGUI( self.dataPath, self.alfonse.lighting, config, self.alfonse )
        #self.lightingGUI.setLightingConfiguration( config )
        
        #self.accept('g', self.lightingGUI.toggleVisible )
        characterConfigurationFileContent = open("./characterConfiguration.xml", "r").read()
                
        #create realizer    
        self.realizer.init(characterConfigurationFileContent)
        thread.start_new_thread ( self.realizer.run,())

        self.time=0
        self.amber.lighting.copy( self.alfonse.lighting )
        taskMgr.doMethodLater(0.02, self.refreshSkels, "refreshSkels")

        #base.disableMouse()       #Disable mouse-based camera-control
        #base.camera.setPos(0,-100,0)    #Position the camera
        #base.camera.setHpr(0,0,0)
        
        #self.cameraSetup = 1
        #self.setCamera()
        
        base.camLens.setFilmSize(24, 36)  # 35 mm film
        base.camLens.setFocalLength(70)   # 70 mm focalLength

        #base.oobe()
        #base.useDrive()
        base.useTrackball()
        base.trackball.node().setPos(0,100,0)
    
        base.setFrameRateMeter(True)
        
        base.setBackgroundColor(0.7,0.7,0.7)
        
        base.disableAllAudio()
     
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

    
    def toggleCamera( self ):
        self.cameraSetup += 1
        if self.cameraSetup > 4:
            self.cameraSetup = 1
        self.setCamera()
        
    def toggleCharacter( self ):
      if self.currentCharacter == 1:
        self.currentCharacter = 2
        
        self.amber.agent.show()
        self.alfonse.agent.show()
      else:
        self.currentCharacter = 1
        
        self.alfonse.agent.show()
        self.amber.agent.show()
 
#******************************************************************************************#

    def refreshSkels(self,task):
        
        self.time+=0.02
        
        #if (self.currentCharacter == 2):
        self.amber.update()
        #else:
        self.alfonse.update()
        
        return Task.again

