from pandac.PandaModules import *

class ShadowMap:
    def __init__( self, cameraMask, dataPath ):
        # creating the offscreen buffer.
        winprops = WindowProperties.size(512,512)
        props = FrameBufferProperties()
        props.setRgbColor(0)
        props.setAlphaBits(0)
        props.setDepthBits(0)
        LBuffer = base.graphicsEngine.makeOutput(
                 base.pipe, "offscreen buffer", -2,
                 props, winprops,
                 GraphicsPipe.BFRefuseWindow,
                 base.win.getGsg(), base.win)
                 
        if (LBuffer == None):
           print "Video driver cannot create an offscreen buffer."
           return         
                 
        self.Ldepthmap = Texture()
        LBuffer.addRenderTexture(self.Ldepthmap, GraphicsOutput.RTMCopyTexture , GraphicsOutput.RTPDepthStencil)
        if (base.win.getGsg().getSupportsShadowFilter()):
            self.Ldepthmap.setMinfilter(Texture.FTShadow)
            self.Ldepthmap.setMagfilter(Texture.FTShadow)
            
        # Adding a color texture is totally unnecessary, but it helps with debugging.
        Lcolormap = Texture()
        LBuffer.addRenderTexture(Lcolormap, GraphicsOutput.RTMCopyTexture , GraphicsOutput.RTPColor)
        
        self.LCam=base.makeCamera(LBuffer)
        self.LCam.node().setScene(render)
        self.LCam.node().getLens().setFov(40)
        self.LCam.node().getLens().setNearFar(10,100)
        self.LCam.reparentTo(render)
        
        self.LCam.setPos(7,-20,0)
        self.LCam.lookAt(7,0,0)
        
        self.LCam.node().setCameraMask(BitMask32.bit(cameraMask))
        
        bias = 0.01
        scale = 1.0
        render.setShaderInput('scale',1,1,1,1)
        render.setShaderInput('Ldepthmap',self.Ldepthmap)
        render.setShaderInput('light',self.LCam)
        render.setShaderInput('scale',scale,scale,scale,1)
        render.setShaderInput('push',bias,bias,bias,1)
        
        # Put a shader on the Light camera.
        lci = NodePath(PandaNode("Light Camera Initializer"))
        lci.setShader( Shader.load( dataPath + 'shaders/caster.sha') )
        self.LCam.node().setInitialState(lci.getState())
        
        #base.bufferViewer.toggleEnable()
        #base.bufferViewer.enable(1)
        
    def setPos( self, direction, center, radius ):
        self.LCam.setPos( -direction.getX()*radius+center.getX(),-direction.getY()*radius+center.getY(),-direction.getZ()*radius+center.getZ() )
        self.LCam.lookAt( center.getX(), center.getY(), center.getZ() )
    
    def setLighting( self, lighting, center, radius ):
        light = lighting.directionalLights[0]
        self.setPos( light.direction, center, radius )
        
    def setTexture( self, model ):
        model.setShaderInput( 'Ldepthmap', self.Ldepthmap )
        model.setShaderInput('light',self.LCam)
