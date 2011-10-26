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

import direct.directbase.DirectStart
from direct.gui.DirectGui import *

# http://www.python.org/doc/2.6/library/colorsys.html
# http://vermeulen.ca/python-graphics.html
# Source: http://code.activestate.com/recipes/576554/
def hsvToRGB(h, s, v):
    """Convert HSV color space to RGB color space
    
    @param h: Hue
    @param s: Saturation
    @param v: Value
    return (r, g, b)  
    """
    import math
    hi = math.floor(h / 60.0) % 6
    f =  (h / 60.0) - math.floor(h / 60.0)
    p = v * (1.0 - s)
    q = v * (1.0 - (f*s))
    t = v * (1.0 - ((1.0 - f) * s))
    return {
        0: Vec4(v, t, p, 0),
        1: Vec4(q, v, p, 0),
        2: Vec4(p, v, t, 0),
        3: Vec4(p, q, v, 0),
        4: Vec4(t, p, v, 0),
        5: Vec4(v, p, q, 0),
    }[hi]

def rgbToHSV(r, g, b):
    """Convert RGB color space to HSV color space
    
    @param r: Red
    @param g: Green
    @param b: Blue
    return (h, s, v)  
    """
    maxc = max(r, g, b)
    minc = min(r, g, b)
    colorMap = {
        id(r): 'r',
        id(g): 'g',
        id(b): 'b'
    }
    if colorMap[id(maxc)] == colorMap[id(minc)]:
        h = 0
    elif colorMap[id(maxc)] == 'r':
        h = 60.0 * ((g - b) / (maxc - minc)) % 360.0
    elif colorMap[id(maxc)] == 'g':
        h = 60.0 * ((b - r) / (maxc - minc)) + 120.0
    elif colorMap[id(maxc)] == 'b':
        h = 60.0 * ((r - g) / (maxc - minc)) + 240.0
    v = maxc
    if maxc == 0.0:
        s = 0.0
    else:
        s = 1.0 - (minc / maxc)
    return (h, s, v)


class World(DirectObject):
    def __init__(self):

        #This code puts the standard title and instuction text on screen
        self.title = OnscreenText(text="",
                                  style=1, fg=(1,1,1,1),
                                  pos=(0.8,-0.95), scale = .07)            
        #setup key input
        self.accept('escape', sys.exit)

        self.agent = Actor('models/amber/amber.bam', {"morph":"models/amber/animations/amber-morphTargets.bam"})
        
        self.agent2 = Actor('models/amber/amber.bam', {"morph":"models/amber/animations/amber-morphTargets.bam"})

        self.agent.reparentTo(render)
        self.agent.setPos(7,0,0)
        self.agent.setScale(10)
        
        self.agent2.reparentTo(render)
        self.agent2.setPos(-7,0,0)
        self.agent2.setScale(10)
        
        self.agent2.hide(BitMask32.bit(1))
        
        #self.agent2.setHpr(100,90,60)
        
        #self.agent.setAntialias(AntialiasAttrib.MMultisample, AntialiasAttrib.MBetter)

        ambientColor = Vec4(1.0,1.0,1.0,1.0)

        #lighting 1
        self.lighting = shaders.Lighting( ambientColor )
        self.lighting.directionalLights[2].color = Vec4(1,0,0,0)
        
        #lighting 2
        self.lighting2 = shaders.Lighting( ambientColor )
        self.lighting2.directionalLights[2].color = Vec4(1,1,1,0)

        #set up shaders for amber model
        self.amberModel = shaders.AmberModel( self.agent, self.lighting )
        self.amberModel2 = shaders.AmberModel( self.agent2, self.lighting2 )
        
        self.time=0
        self.amberModel.update()
        self.amberModel2.update()
        
        #load lamp
        self.addLamps()
        
        #materials[1].setDiffuse( Vec4(1,0,0,0) )
        #self.lamp.setMaterial( materials[0] )
        #self.lamp.setMaterial( materials[1] )
        
        #print self.lamp.findAllMaterials()
        

        taskMgr.doMethodLater(0.02, self.refreshSkels, "refreshSkels")

        base.disableMouse()       #Disable mouse-based camera-control
        base.camera.setPos(0,-100,0)    #Position the camera
        base.camera.setHpr(0,0,0)
        base.camLens.setFilmSize(24, 36)  # 35 mm film
        base.camLens.setFocalLength(70)   # 70 mm focalLength
        
        #base.camera.node().setCameraMask(BitMask32.bit(0))

        #base.oobe()
        #base.useDrive()
        base.useTrackball()
        
        base.setFrameRateMeter(True)
        
        base.setBackgroundColor(0,0,0)
        
        base.disableAllAudio()
        
        #self.initShadowMapping()
        self.shadowMap = shadowmap.ShadowMap()
        
        self.addGUI()
        
        
        
    """def initShadowMapping( self ):
        # creating the offscreen buffer.
    
        winprops = WindowProperties.size(4096,4096)
        props = FrameBufferProperties()
        props.setRgbColor(1)
        props.setAlphaBits(1)
        props.setDepthBits(1)
        LBuffer = base.graphicsEngine.makeOutput(
                 base.pipe, "offscreen buffer", -2,
                 props, winprops,
                 GraphicsPipe.BFRefuseWindow,
                 base.win.getGsg(), base.win)
                 
        if (LBuffer == None):
           print "Video driver cannot create an offscreen buffer."
           return         
                 
        Ldepthmap = Texture()
        LBuffer.addRenderTexture(Ldepthmap, GraphicsOutput.RTMBindOrCopy, GraphicsOutput.RTPDepthStencil)
        if (base.win.getGsg().getSupportsShadowFilter()):
            Ldepthmap.setMinfilter(Texture.FTShadow)
            Ldepthmap.setMagfilter(Texture.FTShadow)
            
        # Adding a color texture is totally unnecessary, but it helps with debugging.
        Lcolormap = Texture()
        LBuffer.addRenderTexture(Lcolormap, GraphicsOutput.RTMBindOrCopy, GraphicsOutput.RTPColor)
        
        self.LCam=base.makeCamera(LBuffer)
        self.LCam.node().setScene(render)
        self.LCam.node().getLens().setFov(40)
        self.LCam.node().getLens().setNearFar(10,100)
        self.LCam.reparentTo(render)
        
        self.LCam.setPos(7,-20,0)
        self.LCam.lookAt(7,0,0)
        
        self.LCam.node().setCameraMask(BitMask32.bit(1))
        
        bias = 0.01
        scale = 1.0
        render.setShaderInput('scale',1,1,1,1)
        render.setShaderInput('Ldepthmap',Ldepthmap)
        render.setShaderInput('light',self.LCam)
        render.setShaderInput('scale',scale,scale,scale,1)
        render.setShaderInput('push',bias,bias,bias,1)
        
        # Put a shader on the Light camera.
        lci = NodePath(PandaNode("Light Camera Initializer"))
        lci.setShader(Shader.load('shaders/caster.sha'))
        self.LCam.node().setInitialState(lci.getState())
        
        #base.bufferViewer.toggleEnable()"""
        
    def addLamps( self ):
        self.lamps = []
        
        for i in range(4):
            lamp = loader.loadModel( 'models/lamp'+str(i+1)+'/lamp'+str(i+1) )
            lamp.reparentTo(render)
            materials = lamp.findAllMaterials()
            #materials[0] = Material("lamp"+str(i))
            materials[0].setAmbient( Vec4(0,0,0,0) )
            materials[0].setDiffuse( Vec4(0,0,0,0) )
            materials[0].setEmission( Vec4(1,1,0,0) )
            materials[0].setSpecular( Vec4(0,0,0,0) )
            
            #print lamp.find('body')
            
            """material = Material("lamp"+str(i))
            lamp.setMaterial( material )"""
            
            lamp.show(BitMask32.bit(0))
            lamp.hide(BitMask32.bit(1))
            
            self.lamps.append( lamp )
            
    def updateLamps( self ):
        self.shadowMap.setLighting( self.lighting, Vec3(7,0,0), 25 )
        
        for i in range(4):
            lamp = self.lamps[i]
            light = self.lighting.directionalLights[i]
            
            theta = self.thetas[i]
            phi = self.phis[i]
            
            materials = lamp.findAllMaterials()
            
            #blub=0.5-theta
            
            color = Vec4( light.color.getX()*light.intensity,light.color.getY()*light.intensity,light.color.getZ()*light.intensity,0)
            
            materials[0].setEmission( color )
            #lamp.setMaterial( materials[0] )
            
            
            """material = Material("lamp"+str(i))
            material.setAmbient( Vec4(0,0,0,0) )
            material.setDiffuse( Vec4(0,0,0,0) )
            material.setEmission( Vec4(1,1,0,0) )
            material.setSpecular( Vec4(0,0,0,0) )
            lamp.setMaterial( material )"""
            
            
            #print lamp.findAllMaterials()
            
            radius=9
            lamp.setPos( -light.direction.getX()*radius+7,-light.direction.getY()*radius,-light.direction.getZ()*radius)
            lamp.setScale( 0.5 )
            lamp.setHpr( -theta*180/3.1416, phi*180/3.1416-90, 0 )
            
            #radius=25
            
            #if i==0:
                #self.shadowMap.LCam.setPos( -light.direction.getX()*radius+7,-light.direction.getY()*radius,-light.direction.getZ()*radius )
                #self.shadowMap.LCam.lookAt(7,0,0)
                
                #print 'blub'
            
            
            #lamp.lookAt(7,0,0)
            #lamp.setHpr(0,90,0)
            
    def hideLamps( self ):  
        for i in range(4):
            lamp = self.lamps[i]
            lamp.hide()
            
    def showLamps( self ):  
        for i in range(4):
            lamp = self.lamps[i]
            lamp.show()
        
    def addGUI( self ):
        # Add some text
        self.textObject = OnscreenText(text = "Lighting comparison", pos = (0.95,-0.95), scale = 0.07,fg=(1,0.5,0.5,1),align=TextNode.ACenter,mayChange=1)
        
        OnscreenText(text = "Light: ", pos = (-1.1,0.95), scale = 0.05,fg=(1,0.5,0.5,1),align=TextNode.ARight,mayChange=1)
        
        OnscreenText(text = "Theta [0..360]: ", pos = (-1.02,0.89), scale = 0.05,fg=(1,0.5,0.5,1),align=TextNode.ARight,mayChange=1)
        OnscreenText(text = "Phi [-90..90]: ", pos = (-1.02,0.84), scale = 0.05,fg=(1,0.5,0.5,1),align=TextNode.ARight,mayChange=1)
        OnscreenText(text = "Intensity [0..2]: ", pos = (-1.02,0.79), scale = 0.05,fg=(1,0.5,0.5,1),align=TextNode.ARight,mayChange=1)
        OnscreenText(text = "Hue [0..360]: ", pos = (-1.02,0.74), scale = 0.05,fg=(1,0.5,0.5,1),align=TextNode.ARight,mayChange=1)
        OnscreenText(text = "Saturation [0..1]: ", pos = (-1.02,0.69), scale = 0.05,fg=(1,0.5,0.5,1),align=TextNode.ARight,mayChange=1)
        OnscreenText(text = "Ambient [0..1]: ", pos = (-1.02,0.64), scale = 0.05,fg=(1,0.5,0.5,1),align=TextNode.ARight,mayChange=1)
        
        OnscreenText(text = "Eye Reflection", pos = (-1.0,0.45), scale = 0.05,fg=(1,0.5,0.5,1),align=TextNode.ARight,mayChange=1)
        OnscreenText(text = "Contrast [0..1]: ", pos = (-1.02,0.4), scale = 0.05,fg=(1,0.5,0.5,1),align=TextNode.ARight,mayChange=1)
        OnscreenText(text = "Amount [0..1]: ", pos = (-1.02,0.35), scale = 0.05,fg=(1,0.5,0.5,1),align=TextNode.ARight,mayChange=1)
        OnscreenText(text = "Fresnel [0..10]: ", pos = (-1.02,0.3), scale = 0.05,fg=(1,0.5,0.5,1),align=TextNode.ARight,mayChange=1)
        
        # Add button
        b = DirectButton(text = ("1"), scale=.05, pos = (-1,0,0.95), command=self.selectLight, extraArgs=[0])
        b = DirectButton(text = ("2"), scale=.05, pos = (-0.95,0,0.95), command=self.selectLight, extraArgs=[1])
        b = DirectButton(text = ("3"), scale=.05, pos = (-0.9,0,0.95), command=self.selectLight, extraArgs=[2])
        b = DirectButton(text = ("4"), scale=.05, pos = (-0.85,0,0.95), command=self.selectLight, extraArgs=[3])
        
        b = DirectButton(text = ("Show"), scale=.05, pos = (-1,0,0.55), command=self.showLamps, extraArgs=[])
        b = DirectButton(text = ("Hide"), scale=.05, pos = (-0.85,0,0.55), command=self.hideLamps, extraArgs=[])
        
        self.sliderTheta = DirectSlider(range=(0,6.28), value=0, pageSize=2, scale=.3, pos = (-0.7,0,0.9), command=self.updateLights)
        self.sliderPhi = DirectSlider(range=(-3.1416/2,3.1416/2), value=0, pageSize=2, scale=.3, pos = (-0.7,0,0.85), command=self.updateLights)
        self.sliderIntensity = DirectSlider(range=(0,2), value=0, pageSize=2, scale=.3, pos = (-0.7,0,0.8), command=self.updateLights)
        self.sliderHue = DirectSlider(range=(0,360), value=0, pageSize=2, scale=.3, pos = (-0.7,0,0.75), command=self.updateLights)
        self.sliderSaturation = DirectSlider(range=(0,1), value=0, pageSize=2, scale=.3, pos = (-0.7,0,0.7), command=self.updateLights)
        self.sliderAmbient = DirectSlider(range=(0,1), value=0, pageSize=2, scale=.3, pos = (-0.7,0,0.65), command=self.updateLights)
        
        self.sliderContrast = DirectSlider(range=(0,1), value=5, pageSize=2, scale=.3, pos = (-0.7,0,0.41), command=self.updateEyes)
        self.sliderAmount = DirectSlider(range=(0,1), value=0.35, pageSize=2, scale=.3, pos = (-0.7,0,0.36), command=self.updateEyes)
        self.sliderFresnel = DirectSlider(range=(0,10), value=5, pageSize=2, scale=.3, pos = (-0.7,0,0.31), command=self.updateEyes)
        
        self.thetas = [ 0, 0, 0, 0 ]
        self.phis = [ 0, 0, 0, 0 ]
        self.intensities = [ 1, 0, 0, 0 ]
        self.hues = [ 0, 0, 0, 0 ]
        self.saturations = [ 0, 0, 0, 0 ]
        
        self.selectLight( 0 )
        self.selectLight( 1 )
        self.selectLight( 2 )
        self.selectLight( 3 )
        self.selectLight( 0 )
                
    def updateLights( self ):
        theta = self.sliderTheta['value']
        phi = self.sliderPhi['value']
        intensity = self.sliderIntensity['value']
        hue = self.sliderHue['value']
        saturation = self.sliderSaturation['value']
    
        self.thetas[ self.lightIndex ] = theta
        self.phis[ self.lightIndex ] = phi
        self.intensities[ self.lightIndex ] = intensity
        self.hues[ self.lightIndex ] = hue
        self.saturations[ self.lightIndex ] = saturation
        
        light = self.lighting.directionalLights[ self.lightIndex ]
        light.direction = Vec3( math.sin(theta) * math.cos(phi), math.cos(theta) * math.cos(phi), math.sin(phi) )
        light.intensity = intensity
        light.color = hsvToRGB( hue, saturation, 1 )
        
        self.updateLamps()
        
    def updateEyes( self ):
        contrast = self.sliderContrast['value']
        amount = self.sliderAmount['value']
        fresnel = self.sliderFresnel['value']
        
        contrast = pow(10,contrast)
        
        self.amberModel.eyeShader.contrastLEye.set( contrast )
        self.amberModel.eyeShader.contrastREye.set( contrast )
        self.amberModel.eyeShader.reflectionAmountLEye.set( amount )
        self.amberModel.eyeShader.reflectionAmountREye.set( amount )
        self.amberModel.eyeShader.reflectionExponentLEye.set( fresnel )
        self.amberModel.eyeShader.reflectionExponentREye.set( fresnel )
    
    def updateGUI( self ):
        theta = self.thetas[ self.lightIndex ]
        phi = self.phis[ self.lightIndex ]
        intensity = self.intensities[ self.lightIndex ]
        hue = self.hues[ self.lightIndex ]
        saturation = self.saturations[ self.lightIndex ]
        
        self.sliderTheta['value'] = theta
        self.sliderPhi['value'] = phi
        self.sliderIntensity['value'] = intensity
        self.sliderHue['value'] = hue
        self.sliderSaturation['value'] = saturation
        
    # Callback function to set  text
    def selectLight( self, index ):
        #self.textObject.setText( "Button Clicked " + str(index) )
        self.lightIndex = index
        self.updateGUI()
        self.updateLights()
 


#******************************************************************************************#

    def refreshSkels(self,task):

        #debug
        self.time+=0.05
        #self.time = self.slider['value']
        #self.lighting.directionalLights[0].direction = Vec3( math.sin(self.time), math.cos(self.time), 0 )
        #self.lighting.directionalLights[2].direction = Vec3( math.sin(self.time+math.pi), math.cos(self.time+math.pi), 0 )
        
        #intensity = (math.sin(self.time*0.3)+0.5)*3
        intensity = self.sliderAmbient['value']
        #intensity = (math.sin(self.time*0.3)+1)/2
        
        self.lighting.ambientColor = Vec4(intensity,intensity,intensity,0)
        self.lighting.update()
        self.amberModel.update()
        
        #self.LCam.setPos( Vec3( math.sin(self.time)*30, math.cos(self.time)*30, 0 ) )
        
        #print self.LCam
        #print base.camera
        
        return Task.again

w = World()
#PStatClient.connect()
run()              #Run the realizer
