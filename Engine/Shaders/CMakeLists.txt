
start_sources()
# -- hwscripts.waf_files --
sources_platform(ALL)
add_sources("NoUberFile"
	SOURCE_GROUP "FX"
		"CFX/PostProcess.fx"
		"CFX/auxgeom.fx"
		"CFX/bb.fx"
		"CFX/cube.fx"
		"CFX/framebuffer.fx"
		"CFX/gui.fx"
		"CFX/illum.fx"
		"CFX/primitive.fx"
		"CFX/screen.fx"
		"CFX/shadow.fx"
		"CFX/sprite.fx"
		"CFX/z.fx"
	SOURCE_GROUP "FX\\\\Include"
		"CFX/common.fx"
		"CFX/FXStreamDefs.fx"
		"CFX/FXConstantDefs.fx"
		"CFX/ModificatorVT.fx"
		"CFX/shadeLib.fx"
		"CFX/vertexLib.fx"
)

end_sources()

CryfileContainer( Shaders SOLUTION_FOLDER "Shaders")

