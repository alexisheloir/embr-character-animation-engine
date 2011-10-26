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

#debug
import time
import os
from pandac.PandaModules import Filename
import re

from pandac.PandaModules import TransparencyAttrib

from pandac.PandaModules import *

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
        0: Vec4(v, t, p, 1),
        1: Vec4(q, v, p, 1),
        2: Vec4(p, v, t, 1),
        3: Vec4(p, q, v, 1),
        4: Vec4(t, p, v, 1),
        5: Vec4(v, p, q, 1),
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
    
class LightingGUI:
    def __init__( self, dataPath, lighting1, config, agent1 ):
      self.dataPath = dataPath
      self.lighting1 = lighting1
      #self.lighting2 = lighting2
      self.agent1 = agent1
      #self.agent2 = agent2
      self.config = config
      
      # get lighting presets
      print 'Presets:'
      self.presets = []
      for entry in self.config:
        if entry != 'Character1' and entry != 'Character2':
          self.presets.append(entry)
      print self.presets
      
      self.lighting = lighting1
      self.agent = agent1
      self.character = 1
      self.characterConfig = 'Character1'
      
      self.visible = 1
      self.lampsVisible = 1
      
      
      self.addLamps()
      self.addGUI()
      
      self.setPresetsComboBox()
      #self.setLightingConfiguration()
      
      
    def setLightingConfiguration( self ):
        
        section = self.config[self.characterConfig]['Lighting']
        
        ambient = self.config[ section ].as_float( 'Ambient' )
        self.sliderAmbient['value'] = ambient
        
        for i in range(4):
            lightConfig = self.config[ section ][ 'Light'+str(i+1) ]
            
            theta = lightConfig.as_float( 'Theta' ) * math.pi / 180
            phi = lightConfig.as_float( 'Phi' ) * math.pi / 180
            intensity = lightConfig.as_float( 'Intensity' )
            hue = lightConfig.as_float( 'Hue' )
            saturation = lightConfig.as_float( 'Saturation' )
            
            self.thetas[i] = theta
            self.phis[i] = phi
            self.intensities[i] = intensity
            self.hues[i] = hue
            self.saturations[i] = saturation
            
        self.selectLight( 0 )
        self.selectLight( 1 )
        self.selectLight( 2 )
        self.selectLight( 3 )
        self.selectLight( 0 )
        self.update()
      
    def addLamps( self ):
        self.lamps = []
        
        for i in range(4):
            lamp = loader.loadModel( self.dataPath + 'models/lamp'+str(i+1)+'/lamp'+str(i+1) )
            lamp.reparentTo(render)
            materials = lamp.findAllMaterials()

            materials[0].setAmbient( Vec4(0,0,0,0) )
            materials[0].setDiffuse( Vec4(0,0,0,0) )
            materials[0].setEmission( Vec4(1,1,0,0) )
            materials[0].setSpecular( Vec4(0,0,0,0) )
            
            lamp.show(BitMask32.bit(0))
            lamp.hide(BitMask32.bit(1))
            
            self.lamps.append( lamp )
            
    def updateLamps( self ):
        pos = self.agent.getPosition()
        for i in range(4):
            lamp = self.lamps[i]
            light = self.lighting.directionalLights[i]
            
            theta = self.thetas[i]
            phi = self.phis[i]
            
            materials = lamp.findAllMaterials()
            
            color = Vec4( light.color.getX()*light.intensity,light.color.getY()*light.intensity,light.color.getZ()*light.intensity,0)
            
            materials[0].setEmission( color )
            
            radius=9
            lamp.setPos( -light.direction.getX()*radius+pos.getX(),-light.direction.getY()*radius+pos.getY(),-light.direction.getZ()*radius+pos.getZ())
            lamp.setScale( 0.5 )
            lamp.setHpr( -theta*180/3.1416, phi*180/3.1416-90, 0 )
            
            """characterIndex = self.character - 1
            lamp.show(BitMask32.bit( characterIndex ))
            lamp.hide(BitMask32.bit( 1-characterIndex ))"""
            lamp.hide(BitMask32.bit( self.character ))
            
            
    def hideLamps( self ):  
        self.lampsVisible = 0
        for i in range(4):
            lamp = self.lamps[i]
            lamp.hide()
            
    def showLamps( self ):  
        self.lampsVisible = 1
        for i in range(4):
            lamp = self.lamps[i]
            lamp.show()
    
    def setPresetsComboBox( self ):
      preset = self.config[self.characterConfig]['Lighting']
      index = 0
      for entry in self.presets:
        if entry == preset:
          break
        index = index+1
        
      self.presetsComboBox.set( index )
    
    def presetSelected( self, args ):
      preset = self.presets[ self.presetsComboBox.selectedIndex ]
      print "Selected preset: " + preset
      self.config[self.characterConfig]['Lighting'] = preset
      
      self.setLightingConfiguration()
    
    def addGUI( self ):
        # Add some text
        #self.textObject = OnscreenText(text = "Lighting comparison", pos = (0.95,-0.95), scale = 0.07,fg=(1,0.5,0.5,1),align=TextNode.ACenter,mayChange=1)
        
        #character
        self.label1 = OnscreenText(text = "Character: ", pos = (-1.02,0.95), scale = 0.05,fg=(1,0.5,0.5,1),align=TextNode.ARight,mayChange=1)
        self.characterLabel = OnscreenText(text = "1", pos = (-0.78,0.95), scale = 0.05,fg=(1,0.5,0.5,1),align=TextNode.ARight,mayChange=1)
        self.characterButton = DirectButton(text = ("Switch"), scale=.05, pos = (-0.9,0,0.95), command=self.toggleCharacter)
        
        #line
        self.label2 = OnscreenText(text = "-------------------------------------------------", pos = (-0.40,0.9), scale = 0.05,fg=(1,0.5,0.5,1),align=TextNode.ARight,mayChange=1)
        
        self.label3 = OnscreenText(text = "Light: ", pos = (-1.02,0.75), scale = 0.05,fg=(1,0.5,0.5,1),align=TextNode.ARight,mayChange=1 )
        self.lightColorLabel = OnscreenText(text = "|      |", pos = (-0.69,0.75), scale = 0.05,fg=(0,0,0,1),align=TextNode.ARight,mayChange=1, bg=(0,0,0,1) )
        
        self.label4 = OnscreenText(text = "Theta [0..360]: ", pos = (-1.02,0.69), scale = 0.05,fg=(1,0.5,0.5,1),align=TextNode.ARight,mayChange=1)
        self.label5 = OnscreenText(text = "Phi [-90..90]: ", pos = (-1.02,0.64), scale = 0.05,fg=(1,0.5,0.5,1),align=TextNode.ARight,mayChange=1)
        self.label6 = OnscreenText(text = "Intensity [0..2]: ", pos = (-1.02,0.59), scale = 0.05,fg=(1,0.5,0.5,1),align=TextNode.ARight,mayChange=1)
        self.label7 = OnscreenText(text = "Hue [0..360]: ", pos = (-1.02,0.54), scale = 0.05,fg=(1,0.5,0.5,1),align=TextNode.ARight,mayChange=1)
        self.label8 = OnscreenText(text = "Saturation [0..1]: ", pos = (-1.02,0.49), scale = 0.05,fg=(1,0.5,0.5,1),align=TextNode.ARight,mayChange=1)
        self.label9 = OnscreenText(text = "Ambient [0..1]: ", pos = (-1.02,0.44), scale = 0.05,fg=(1,0.5,0.5,1),align=TextNode.ARight,mayChange=1)
        self.label10 = OnscreenText(text = "Lamps: ", pos = (-1.02,0.35), scale = 0.05,fg=(1,0.5,0.5,1),align=TextNode.ARight,mayChange=1)
        
        self.label11 = OnscreenText(text = "-------------------------------------------------", pos = (-0.40,0.3), scale = 0.05,fg=(1,0.5,0.5,1),align=TextNode.ARight,mayChange=1)
        self.label12 = OnscreenText(text = "Config: ", pos = (-1.02,0.25), scale = 0.05,fg=(1,0.5,0.5,1),align=TextNode.ARight,mayChange=1)
        
        # Add button
        
        
        #presets
        self.label13 = OnscreenText(text = "Preset: ", pos = (-1.02,0.85), scale = 0.05,fg=(1,0.5,0.5,1),align=TextNode.ARight,mayChange=1)
        
        self.presetsComboBox = DirectOptionMenu(text="options", scale=.05,items=self.presets,initialitem=0, pos = (-1.0,0,0.85),
          highlightColor=(0.65,0.65,0.65,1),textMayChange=1,command=self.presetSelected, extraArgs=[])
        
        self.button1 = DirectButton(text = ("Update Preset"), scale=.05, pos = (-0.6,0,0.85), command=self.updatePreset, extraArgs=[])
        
        #lights
        self.button2 = DirectButton(text = (" 1 "), scale=.05, pos = (-1,0,0.75), command=self.selectLight, extraArgs=[0])
        self.button3 = DirectButton(text = (" 2 "), scale=.05, pos = (-0.95,0,0.75), command=self.selectLight, extraArgs=[1])
        self.button4 = DirectButton(text = (" 3 "), scale=.05, pos = (-0.9,0,0.75), command=self.selectLight, extraArgs=[2])
        self.button5 = DirectButton(text = (" 4 "), scale=.05, pos = (-0.85,0,0.75), command=self.selectLight, extraArgs=[3])
        
        self.button6 = DirectButton(text = ("Show"), scale=.05, pos = (-0.94,0,0.35), command=self.showLamps, extraArgs=[])
        self.button7 = DirectButton(text = ("Hide"), scale=.05, pos = (-0.79,0,0.35), command=self.hideLamps, extraArgs=[])
        
        self.button8 = DirectButton(text = ("Save"), scale=.05, pos = (-0.94,0,0.25), command=self.saveConfig, extraArgs=[])
        
        
        
        self.sliderTheta = DirectSlider(range=(0,6.28), value=0, pageSize=2, scale=.3, pos = (-0.7,0,0.7), command=self.updateLights)
        self.sliderPhi = DirectSlider(range=(-3.1416/2,3.1416/2), value=0, pageSize=2, scale=.3, pos = (-0.7,0,0.65), command=self.updateLights)
        self.sliderIntensity = DirectSlider(range=(0,2), value=0, pageSize=2, scale=.3, pos = (-0.7,0,0.6), command=self.updateLights)
        self.sliderHue = DirectSlider(range=(0,360), value=0, pageSize=2, scale=.3, pos = (-0.7,0,0.55), command=self.updateLights)
        self.sliderSaturation = DirectSlider(range=(0,1), value=0, pageSize=2, scale=.3, pos = (-0.7,0,0.5), command=self.updateLights)
        self.sliderAmbient = DirectSlider(range=(0,1), value=0, pageSize=2, scale=.3, pos = (-0.7,0,0.45), command=self.updateLights)
        
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
    
    def saveConfig( self ):
      self.config.write()
    
    def updatePreset( self ):
      section = self.config[self.characterConfig]['Lighting']
      
      ambient = self.sliderAmbient['value']
      
      self.config[ section ]['Ambient'] = str( ambient )
      
      for i in range(4):
          lightConfig = self.config[ section ][ 'Light'+str(i+1) ]
          
          theta = self.thetas[i] * 180 / math.pi
          phi = self.phis[i] * 180 / math.pi
          intensity = self.intensities[i]
          hue = self.hues[i]
          saturation = self.saturations[i]
          
          lightConfig['Theta'] = theta
          lightConfig['Phi'] = phi
          lightConfig['Intensity'] = intensity
          lightConfig['Hue'] = hue
          lightConfig['Saturation'] = saturation
          
          self.config[ section ][ 'Light'+str(i+1) ] = lightConfig
          
      self.toggleCharacter()
      print "Updating preset"
      self.toggleCharacter()
    
    def toggleCharacter( self ):
        
        if self.character == 1:
          self.character = 2
          #self.lighting = self.lighting2
          self.characterConfig = 'Character2'
          #self.agent = self.agent2
        else:
          self.character = 1
          self.lighting = self.lighting1
          self.characterConfig = 'Character1'
          self.agent = self.agent1
          
        #self.characterButton.setText('blub')
        self.characterLabel.setText( str(self.character) )
        
        #self.setLightingConfiguration()
        self.setPresetsComboBox()
    
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
        
        self.lightColorLabel['fg'] = Vec4(0,0,0,0)
        self.lightColorLabel['bg'] = light.color * intensity
        
        self.updateLamps()
    
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
        
    def update( self ):
      intensity = self.sliderAmbient['value']

      self.lighting.ambientColor = Vec4(intensity,intensity,intensity,0)
      self.lighting.update()
      
    def toggleVisible( self ):
      if self.visible == 1:
          self.visible = 0
          
          self.characterLabel.hide()
          self.characterButton.hide()
          self.lightColorLabel.hide()
          self.presetsComboBox.hide()
          
          self.label1.hide()
          self.label2.hide()
          self.label3.hide()
          self.label4.hide()
          self.label5.hide()
          self.label6.hide()
          self.label7.hide()
          self.label8.hide()
          self.label9.hide()
          self.label10.hide()
          self.label11.hide()
          self.label12.hide()
          self.label13.hide()
          
          self.button1.hide()
          self.button2.hide()
          self.button3.hide()
          self.button4.hide()
          self.button5.hide()
          self.button6.hide()
          self.button7.hide()
          self.button8.hide()
          
          self.sliderTheta.hide()
          self.sliderPhi.hide()
          self.sliderIntensity.hide()
          self.sliderHue.hide()
          self.sliderSaturation.hide()
          self.sliderAmbient.hide()
          
          self.lampsWereVisible = self.lampsVisible
          self.hideLamps()
      else:
          self.visible = 1
          
          self.characterLabel.show()
          self.characterButton.show()
          self.lightColorLabel.show()
          self.presetsComboBox.show()
          
          self.label1.show()
          self.label2.show()
          self.label3.show()
          self.label4.show()
          self.label5.show()
          self.label6.show()
          self.label7.show()
          self.label8.show()
          self.label9.show()
          self.label10.show()
          self.label11.show()
          self.label12.show()
          self.label13.show()
          
          self.button1.show()
          self.button2.show()
          self.button3.show()
          self.button4.show()
          self.button5.show()
          self.button6.show()
          self.button7.show()
          self.button8.show()
          
          self.sliderTheta.show()
          self.sliderPhi.show()
          self.sliderIntensity.show()
          self.sliderHue.show()
          self.sliderSaturation.show()
          self.sliderAmbient.show()
          
          if self.lampsWereVisible == 1:
            self.showLamps()
