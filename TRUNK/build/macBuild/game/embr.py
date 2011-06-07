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
                             "poseNeutral":self.dataPath + "models/amber/animations/amber-poseNeutral.bam",                             
                             "hands_DGS_A":self.dataPath + "models/amber/animations/amber-hands_DGS_A.bam",
                             "hands_DGS_B":self.dataPath + "models/amber/animations/amber-hands_DGS_B.bam",
                             "hands_DGS_C":self.dataPath + "models/amber/animations/amber-hands_DGS_C.bam",
                             "hands_DGS_D":self.dataPath + "models/amber/animations/amber-hands_DGS_D.bam",
                             "hands_DGS_E":self.dataPath + "models/amber/animations/amber-hands_DGS_E.bam",
                             "hands_DGS_F":self.dataPath + "models/amber/animations/amber-hands_DGS_F.bam",
                             "hands_DGS_G":self.dataPath + "models/amber/animations/amber-hands_DGS_G.bam",
                             "hands_DGS_H":self.dataPath + "models/amber/animations/amber-hands_DGS_H.bam",
                             "hands_DGS_I":self.dataPath + "models/amber/animations/amber-hands_DGS_I.bam",
                             "hands_DGS_J":self.dataPath + "models/amber/animations/amber-hands_DGS_J.bam",
                             "hands_DGS_K":self.dataPath + "models/amber/animations/amber-hands_DGS_K.bam",
                             "hands_DGS_L":self.dataPath + "models/amber/animations/amber-hands_DGS_L.bam",
                             "hands_DGS_M":self.dataPath + "models/amber/animations/amber-hands_DGS_M.bam",
                             "hands_DGS_N":self.dataPath + "models/amber/animations/amber-hands_DGS_N.bam",
                             "hands_DGS_O":self.dataPath + "models/amber/animations/amber-hands_DGS_O.bam",
                             "hands_DGS_P":self.dataPath + "models/amber/animations/amber-hands_DGS_P.bam",
                             "hands_DGS_Q":self.dataPath + "models/amber/animations/amber-hands_DGS_Q.bam",
                             "hands_DGS_R":self.dataPath + "models/amber/animations/amber-hands_DGS_R.bam",
                             "hands_DGS_T":self.dataPath + "models/amber/animations/amber-hands_DGS_T.bam",
                             "hands_DGS_U":self.dataPath + "models/amber/animations/amber-hands_DGS_U.bam",
                             "hands_DGS_V":self.dataPath + "models/amber/animations/amber-hands_DGS_V.bam",
                             "hands_DGS_W":self.dataPath + "models/amber/animations/amber-hands_DGS_W.bam",
                             "hands_DGS_X":self.dataPath + "models/amber/animations/amber-hands_DGS_X.bam",
                             "hands_DGS_Y":self.dataPath + "models/amber/animations/amber-hands_DGS_Y.bam",
                             "hands_DGS_Z":self.dataPath + "models/amber/animations/amber-hands_DGS_Z.bam",
                             "hands_DGS_SCH":self.dataPath + "models/amber/animations/amber-hands_DGS_SCH.bam",
                             "hands_ASL_A":self.dataPath + "models/amber/animations/amber-hands_DGS_A.bam",
                             "hands_ASL_B":self.dataPath + "models/amber/animations/amber-hands_DGS_B.bam",
                             "hands_ASL_C":self.dataPath + "models/amber/animations/amber-hands_DGS_C.bam",
                             "hands_ASL_D":self.dataPath + "models/amber/animations/amber-hands_DGS_D.bam",
                             "hands_ASL_E":self.dataPath + "models/amber/animations/amber-hands_DGS_E.bam",
                             "hands_ASL_F":self.dataPath + "models/amber/animations/amber-hands_DGS_F.bam",
                             "hands_ASL_G":self.dataPath + "models/amber/animations/amber-hands_ASL_G.bam",
                             "hands_ASL_H":self.dataPath + "models/amber/animations/amber-hands_DGS_H.bam",
                             "hands_ASL_I":self.dataPath + "models/amber/animations/amber-hands_DGS_I.bam",
                             "hands_ASL_J":self.dataPath + "models/amber/animations/amber-hands_DGS_J.bam",
                             "hands_ASL_K":self.dataPath + "models/amber/animations/amber-hands_DGS_K.bam",
                             "hands_ASL_L":self.dataPath + "models/amber/animations/amber-hands_DGS_L.bam",
                             "hands_ASL_M":self.dataPath + "models/amber/animations/amber-hands_ASL_M.bam",
                             "hands_ASL_N":self.dataPath + "models/amber/animations/amber-hands_ASL_N.bam",
                             "hands_ASL_O":self.dataPath + "models/amber/animations/amber-hands_DGS_O.bam",
                             "hands_ASL_P":self.dataPath + "models/amber/animations/amber-hands_DGS_P.bam",
                             "hands_ASL_Q":self.dataPath + "models/amber/animations/amber-hands_DGS_Q.bam",
                             "hands_ASL_R":self.dataPath + "models/amber/animations/amber-hands_DGS_R.bam",
                             "hands_ASL_T":self.dataPath + "models/amber/animations/amber-hands_ASL_T.bam",
                             "hands_ASL_U":self.dataPath + "models/amber/animations/amber-hands_DGS_U.bam",
                             "hands_ASL_V":self.dataPath + "models/amber/animations/amber-hands_DGS_V.bam",
                             "hands_ASL_W":self.dataPath + "models/amber/animations/amber-hands_DGS_W.bam",
                             "hands_ASL_X":self.dataPath + "models/amber/animations/amber-hands_DGS_X.bam",
                             "hands_ASL_Y":self.dataPath + "models/amber/animations/amber-hands_DGS_Y.bam",
                             "hands_ASL_Z":self.dataPath + "models/amber/animations/amber-hands_DGS_Z.bam",
                             "hands_ASL_1CL":self.dataPath + "models/amber/animations/amber-hands_ASL_1CL.bam",
                             "hands_ASL_2CL":self.dataPath + "models/amber/animations/amber-hands_ASL_2CL.bam",
                             "hands_ASL_3CL":self.dataPath + "models/amber/animations/amber-hands_ASL_3CL.bam",
                             "hands_ASL_4CL":self.dataPath + "models/amber/animations/amber-hands_ASL_4CL.bam",
                             "hands_ASL_5aCL":self.dataPath + "models/amber/animations/amber-hands_ASL_5aCL.bam",
                             "hands_ASL_5bCL":self.dataPath + "models/amber/animations/amber-hands_ASL_5bCL.bam",
                             "hands_ASL_ACL":self.dataPath + "models/amber/animations/amber-hands_ASL_ACL.bam",
                             "hands_ASL_BCL":self.dataPath + "models/amber/animations/amber-hands_ASL_BCL.bam",
                             "hands_ASL_CCL":self.dataPath + "models/amber/animations/amber-hands_ASL_CCL.bam",
                             "hands_ASL_FCL":self.dataPath + "models/amber/animations/amber-hands_ASL_FCL.bam",
                             "hands_ASL_GCL":self.dataPath + "models/amber/animations/amber-hands_ASL_GCL.bam",
                             "hands_ASL_ICL":self.dataPath + "models/amber/animations/amber-hands_ASL_ICL.bam",
                             "hands_ASL_ILYCL":self.dataPath + "models/amber/animations/amber-hands_ASL_ILYCL.bam",
                             "hands_ASL_SCL":self.dataPath + "models/amber/animations/amber-hands_ASL_SCL.bam",
                             "hands_ASL_VaCL":self.dataPath + "models/amber/animations/amber-hands_ASL_VaCL.bam",
                             "hands_ASL_VbCL":self.dataPath + "models/amber/animations/amber-hands_ASL_VbCL.bam",
                             "hands_ASL_YCL":self.dataPath + "models/amber/animations/amber-hands_ASL_YCL.bam"
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

    
