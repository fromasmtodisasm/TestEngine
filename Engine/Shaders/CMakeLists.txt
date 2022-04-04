
start_sources()
# -- hwscripts.waf_files --
sources_platform(ALL)
add_sources("NoUberFile"
	SOURCE_GROUP "FX"
		"FX/PostProcess.fx"
		"FX/auxgeom.fx"
		"FX/bb.fx"
		"FX/cube.fx"
		"FX/framebuffer.fx"
		"FX/gui.fx"
		"FX/illum.fx"
		"FX/primitive.fx"
		"FX/screen.fx"
		"FX/shadow.fx"
		"FX/sprite.fx"
		"FX/z.fx"
	SOURCE_GROUP "FX\\\\Include"
		"FX/common.cfi"
		"FX/FXStreamDefs.cfi"
		"FX/FXConstantDefs.cfi"
		"FX/ModificatorVT.cfi"
		"FX/shadeLib.cfi"
		"FX/vertexLib.cfi"
)

end_sources()

CryfileContainer( Shaders SOLUTION_FOLDER "Shaders")

