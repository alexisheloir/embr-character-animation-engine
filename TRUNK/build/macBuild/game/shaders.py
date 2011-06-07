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


class AmberModel:
    def __init__( self, actor, lighting, dataPath, shaderPath ):
        self.modelHead = actor.find("**/theHead")
        
        materials=actor.findAllMaterials()
        
        self.modelHair = actor.find("**/theHair")
        self.modelJacket = actor.find("**/theJacket")
        
        self.modelLEye = actor.find("**/theLEye")
        self.modelREye = actor.find("**/theREye")
        
        self.modelBody = actor.find("**/theBody")
        self.modelTrousers = actor.find("**/theTrousers")
        
        self.modelLShoe = actor.find("**/theLeftShoe")
        self.modelRShoe = actor.find("**/theRightShoe")
        
        self.lighting = lighting
        
        #create shaders
        self.hairShader = HairShader( self.modelHair, camera, lighting, dataPath, shaderPath )
        self.headShader = HeadShader( self.modelHead, camera, lighting, dataPath, shaderPath )
        self.eyeShader = EyeShader( self.modelLEye, self.modelREye, camera, lighting, dataPath, shaderPath )
        self.bodyShader = BodyShader( self.modelBody, camera, lighting, dataPath, shaderPath )
        self.jacketShader = GenericShader( self.modelJacket, camera, lighting, "UVTex", "models/amber/textures/jacket/jacketDiffuse.jpg", "models/amber/textures/jacket/jacketShadow.jpg", dataPath, shaderPath )
        self.trousersShader = GenericShader( self.modelTrousers, camera, lighting, "UVTex", "models/amber/textures/trousers/trousersDiffuse.jpg", "models/amber/textures/trousers/trousersShadow.jpg", dataPath, shaderPath )
        
        self.lshoeShader = GenericShader( self.modelLShoe, camera, lighting, "UVTex", "models/amber/textures/shoes/shoeDiffuse.jpg", "models/amber/textures/shoes/shoeShadow.jpg", dataPath, shaderPath )
        self.rshoeShader = GenericShader( self.modelRShoe, camera, lighting, "UVTex", "models/amber/textures/shoes/shoeDiffuse.jpg", "models/amber/textures/shoes/shoeShadow.jpg", dataPath, shaderPath )
        
        self.modelJacket.setTwoSided(True)
        
    def update(self):
        self.hairShader.update()
        self.headShader.update()
        self.eyeShader.update()
        self.bodyShader.update()
        self.jacketShader.update()
        self.trousersShader.update()
        self.lshoeShader.update()
        self.rshoeShader.update()
        
    def updateWithShadows( self, shadowMap ):
        """self.hairShader.update()
        self.headShader.update()
        self.eyeShader.update()
        self.bodyShader.update()
        self.jacketShader.update()
        self.trousersShader.update()
        self.lshoeShader.update()
        self.rshoeShader.update()"""
        self.update()
        
        shadowMap.setTexture( self.modelHair )
        shadowMap.setTexture( self.modelJacket )
        shadowMap.setTexture( self.modelHead )
        shadowMap.setTexture( self.modelLEye )
        shadowMap.setTexture( self.modelREye )
        shadowMap.setTexture( self.modelBody )
        shadowMap.setTexture( self.modelTrousers )
        shadowMap.setTexture( self.modelLShoe )
        shadowMap.setTexture( self.modelRShoe )

class ShaderDirectionalLight:
    def __init__( self, color, direction, intensity, index ):
        self.color = color
        self.direction = direction
        self.intensity = intensity
        self.index = index
        self.name = "directionalLight" + str( index )
        
    def create( self, lightAttrib ):
        #create directional light
        directionalLight = DirectionalLight( self.name )
        directionalLight.setDirection( self.direction )
        directionalLight.setColor( self.color * self.intensity )
        directionalLight.setSpecularColor( Vec4(0.5,0.5,0.5,0.5) )
        lightAttrib = lightAttrib.addLight( directionalLight )
        
        #attach lights to scene graph
        render.attachNewNode( directionalLight )
        self.directionalLight = directionalLight
        
        return lightAttrib
        
    def update( self ):
        self.directionalLight.setDirection( self.direction )
        self.directionalLight.setColor( self.color * self.intensity )
 
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
# ambientColor - Vec4
# directionalColor - Vec4
# direction - Vec3
class Lighting:
    def __init__( self, ambientColor ):
        self.ambientColor = ambientColor
        self.create()
        
    def create( self ):
        lightAttrib = LightAttrib.makeAllOff()
        
        #create ambient light
        ambientLight = AmbientLight( "ambientLight" )
        ambientLight.setColor( self.ambientColor )
        lightAttrib = lightAttrib.addLight( ambientLight )
        
        render.attachNewNode( ambientLight.upcastToPandaNode() ) 
        self.ambientLight = ambientLight
        
        #default light settings
        """colors = [ Vec4(1,0,0,0), Vec4(0,1,0,0), Vec4(0,0,1,0), Vec4(1,1,0,0) ]
        directions = [ Vec3(1,0,0), Vec3(0,1,0), Vec3(-1,0,0), Vec3(0,-1,0) ]
        intensities = [ 3.0, 0.1, 3.0, 0.1 ]"""
        
        colors = [ Vec4(1,1,1,0), Vec4(0,1,0,0), Vec4(0,0,1,0), Vec4(1,1,0,0) ]
        # basic 3+1 point lighting
        directions = [ Vec3(0,1,-0.2), Vec3(0,1,0), Vec3(-1,0.3,0), Vec3(0,-1,0) ]
        intensities = [ 1.0, 0.0, 0.5, 0.0 ]
        
        #add directional lights
        self.directionalLights = []
        
        for i in range(4):
            self.directionalLights.append( ShaderDirectionalLight( colors[i], directions[i], intensities[i], i ) )
            lightAttrib = self.directionalLights[i].create( lightAttrib )
        
        #set light attributes
        render.node().setAttrib( lightAttrib )

        
    """def set(self,ambientColor,directionalColor,direction):
        self.ambientColor=ambientColor
        self.directionalColor=directionalColor
        self.direction=direction
        #self.update()"""
        
    def update( self ):
        self.ambientLight.setColor( self.ambientColor )
        
        for i in range(4):
            self.directionalLights[i].update()
        
    def setShaderInput(self,model):
        print "blub"

    def loadFromConfig( self, config, section ):
        ambient = config[ section ].as_float( 'Ambient' )
        self.ambientColor = Vec4( ambient, ambient, ambient, 0 )
        
        for i in range(4):
            lightConfig = config[ section ][ 'Light'+str(i+1) ]
            
            theta = lightConfig.as_float( 'Theta' ) * math.pi / 180
            phi = lightConfig.as_float( 'Phi' ) * math.pi / 180
            intensity = lightConfig.as_float( 'Intensity' )
            hue = lightConfig.as_float( 'Hue' )
            saturation = lightConfig.as_float( 'Saturation' )
            
            light = self.directionalLights[ i ]
            light.intensity = intensity
            light.direction = Vec3( math.sin(theta) * math.cos(phi), math.cos(theta) * math.cos(phi), math.sin(phi) )
            light.color = hsvToRGB( hue, saturation, 1 )
class Shader:
    def __init__(self):
        print "blub"
        
    def loadAndSetTexture(self,nodePath,fileName,textureStageName,texCoordName,sort):
        #load texture from file
        texture=loader.loadTexture(fileName)
        texture.setWrapU(Texture.WMClamp)
        texture.setWrapV(Texture.WMClamp)
        
        #set texture stage
        textureStage=TextureStage(textureStageName)
        textureStage.setTexcoordName(texCoordName)
        textureStage.setSort(sort)
        
        #set texture and texture stage
        nodePath.setTexture(textureStage,texture)
        
    def loadAndSetTexture2( self, nodePath1, nodePath2, fileName, textureStageName, texCoordName, sort ):
        #load texture from file
        texture=loader.loadTexture(fileName)
        texture.setWrapU(Texture.WMClamp)
        texture.setWrapV(Texture.WMClamp)
        
        #set texture stage
        textureStage=TextureStage(textureStageName)
        textureStage.setTexcoordName(texCoordName)
        textureStage.setSort(sort)
        
        #set texture and texture stage
        nodePath1.setTexture(textureStage,texture)
        nodePath2.setTexture(textureStage,texture)
        
    def loadAndSetCubeTexture2( self, nodePath1, nodePath2, fileName, textureStageName, texCoordName, sort ):
        #load texture from file
        texture = loader.loadCubeMap(fileName)
        
        #set texture stage
        textureStage=TextureStage(textureStageName)
        textureStage.setTexcoordName(texCoordName)
        textureStage.setSort(sort)
        
        #set texture and texture stage
        nodePath1.setTexture(textureStage,texture)
        nodePath2.setTexture(textureStage,texture)    
        
    def setLighting( self, model, lighting ):
        ambientColor = lighting.ambientColor
        """directionalLightColor = lighting.directionalLights[0].color
        directionalLightDirection = lighting.directionalLights[0].direction"""
        
        model.setShaderInput("ambientLight",Vec4(ambientColor.getX(),ambientColor.getY(),ambientColor.getZ(),0))
        
        for i in range(4):
            light = lighting.directionalLights[i]
            color = Vec4( light.color.getX() * light.intensity, light.color.getY() * light.intensity, light.color.getZ() * light.intensity, 0 )
            direction = Vec4( light.direction.getX(), light.direction.getY(), light.direction.getZ(), 0 )
            
            model.setShaderInput( "directionalLightColor" + str(i+1), color )
            model.setShaderInput( "directionalLightDirection" + str(i+1), direction )
    
    def loadShaderRelative( self, relativeShaderPath, fileName ):
        print "Loading shader: " + fileName
        
        shaderDirectory = os.path.abspath( relativeShaderPath ) + os.sep
        shaderFileName = shaderDirectory + fileName
        newShaderFileName = shaderDirectory + "~" + fileName
        
        #replace Windows backslashes with double backslashes
        textShaderDir = shaderDirectory
        if os.sep == '\\':
            textShaderDir = textShaderDir.replace( "\\", "\\\\" )
        
        #replace shader path placeholder
        o = open( newShaderFileName, "w" )
        for line in open( shaderFileName ):
            line = line.replace( "<SHADERPATH>", textShaderDir )
            o.write(line)
        o.close()
        
        #load tempered shader        
        shader = loader.loadShader( relativeShaderPath + "~" + fileName )
        
        return shader
    
class HairShader(Shader):
    def __init__(self,modelHair,camera, lighting, dataPath, shaderPath ):
        #textures
        texCoordName="UVTex"
        
       	#set diffuse and specular map
        self.loadAndSetTexture( modelHair, dataPath + "models/amber/textures/hair/hair.png",  "diffuse",  texCoordName, 0 )
        self.loadAndSetTexture( modelHair, dataPath + "models/amber/textures/hair/hairSpecular.jpg", "specular", texCoordName, 1 )
		
        shader = self.loadShaderRelative( shaderPath, "hair.sha" )
        
        modelHair.setShader(shader,-1)
        modelHair.setShaderInput("camera",camera)
        modelHair.setTwoSided(True)
        
        self.modelHair = modelHair
        
        self.lighting = lighting
        self.update()
        
    def update(self):
        self.setLighting( self.modelHair, self.lighting )
        
class HeadShader(Shader):
    def __init__( self, modelHead, camera, lighting, dataPath, shaderPath ):
        #textures
        texCoordName="UVTex"
                
        #set diffuse, normal, ambient occlusion, blood and subsurface scattering strength map
        self.loadAndSetTexture( modelHead, dataPath + "models/amber/textures/head/headDiffuse.jpg",  "diffuse", texCoordName, 0 )
        self.loadAndSetTexture( modelHead, dataPath + "models/amber/textures/head/headShadow.jpg",   "shadow",  texCoordName, 2 )
        self.loadAndSetTexture( modelHead, dataPath + "models/amber/textures/head/headBlushing.png", "blood",   texCoordName, 3 )
        
        #load shader
        shader = self.loadShaderRelative( shaderPath, "head.sha" )
        modelHead.setShader(shader,-1)
        
       	#make camera position available to vertex shader for fresnel view direction computation
        modelHead.setShaderInput("camera",camera)
        
        self.modelHead = modelHead
        
        self.blushing = ShaderValue( 0.0, modelHead, "values", 0 )
        
        self.lighting = lighting
        self.update()
        
    def update(self):
        self.setLighting( self.modelHead, self.lighting )

class EyeShader(Shader):
    def __init__( self, modelLEye, modelREye, camera, lighting, dataPath, shaderPath ):
        #textures
        texCoordName="UVTex"
        
       	#load maps
        self.loadAndSetTexture2( modelLEye, modelREye, dataPath + "models/amber/textures/eyes/eyeDiffuse.jpg",       "diffuse", texCoordName, 0 )
        self.loadAndSetCubeTexture2( modelLEye, modelREye, dataPath + 'models/amber/textures/eyes/brightday2_#.png', "cube",    texCoordName, 1 )
 
        #load shader
        shader = self.loadShaderRelative( shaderPath, "eye.sha" )
        modelLEye.setShader(shader,-1)
        modelREye.setShader(shader,-1)
        modelLEye.setShaderInput("camera",camera)
        modelREye.setShaderInput("camera",camera)
        
        #[contrast,reflexionamount,reflectionExponent,dummy]
        modelLEye.setShaderInput("values",Vec4(5,0.35,5,0))
        modelREye.setShaderInput("values",Vec4(5,0.35,5,0))
        
        self.contrastLEye = ShaderValue(5.0,modelLEye,"values",0)
        self.reflectionAmountLEye = ShaderValue(0.35,modelLEye,"values",1)
        self.reflectionExponentLEye = ShaderValue(5.0,modelLEye,"values",2)

        self.contrastREye = ShaderValue(5.0,modelREye,"values",0)
        self.reflectionAmountREye = ShaderValue(0.35,modelREye,"values",1)
        self.reflectionExponentREye = ShaderValue(5.0,modelREye,"values",2)

        self.modelLEye = modelLEye
        self.modelREye = modelREye
        
        self.lighting = lighting
        self.update()
    
    def update(self):
        self.setLighting( self.modelLEye, self.lighting )
        self.setLighting( self.modelREye, self.lighting )
        
class BodyShader(Shader):
    def __init__( self, modelBody, camera, lighting, dataPath, shaderPath ):
        #textures
        texCoordName="body"
                
        #set maps
        self.loadAndSetTexture( modelBody, dataPath + "models/amber/textures/body/bodyDiffuse.jpg", "diffuse",  texCoordName,0 )
        self.loadAndSetTexture( modelBody, dataPath + "models/amber/textures/body/bodyShadow.jpg",  "shadow",   texCoordName,1 )
        
       	#load shader
        shader = self.loadShaderRelative( shaderPath, "body.sha" )
        modelBody.setShader(shader,-1)
        
       	#make camera position available to vertex shader for fresnel view direction computation
        modelBody.setShaderInput("camera",camera)
                
        self.modelBody = modelBody
                
        self.lighting = lighting
        self.update()
        
    def update(self):
        self.setLighting( self.modelBody, self.lighting )
        
class GenericShader(Shader):
    def __init__( self, model, camera, lighting, texCoordName, fileNameDiffuse, fileNameShadow, dataPath, shaderPath ): 
        #set maps
        self.loadAndSetTexture( model, dataPath + fileNameDiffuse, "diffuse", texCoordName, 0 )
        self.loadAndSetTexture( model, dataPath + fileNameShadow,  "shadow",  texCoordName, 1 )
        
       	#load shader
        shader = self.loadShaderRelative( shaderPath, "generic.sha" )
        model.setShader(shader,-1)
        
       	#make camera position available to vertex shader for fresnel view direction computation
        model.setShaderInput("camera",camera)
        
        self.model = model

        self.lighting = lighting
        self.update()
        
    def update(self):
        self.setLighting( self.model, self.lighting )
        
class ShaderValue:
    def __init__(self,value,model,key,position):
        self.value=value
        self.model=model
        self.key=key
        self.position=position
        self.set(value)
        
    def set(self,value):
        self.value=value
        vector=self.model.getShaderInput(self.key).getVector()
        newVector=Vec4(vector.getX(),vector.getY(),vector.getZ(),vector.getW())
        if self.position==0:
            newVector.setX(self.value)
        elif self.position==1:
            newVector.setY(self.value)
        elif self.position==2:
            newVector.setZ(self.value)
        else:
            newVector.setW(self.value)
        self.model.setShaderInput(self.key,newVector)
