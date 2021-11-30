cmake_minimum_required(VERSION 3.10)

add_package(stb)
add_package(freetype)
add_package(freeimage)

add_subdirectory(Shaders)

start_sources()
sources_platform(ALL)
add_sources(
	"NoUberFile"
	PROJECTS
	RendererDX11
	RendererGL
	RendererVK
	SOURCE_GROUP "Source Files"
		"AuxRenderer.cpp"
		"AuxRendererNull.cpp"
		"BaseRenderer.cpp"
		"FreeTypeFont.cpp"
		"RenderThread.cpp"
		"RenderThread.h"
		"Shaders/FxParser.cpp"
	SOURCE_GROUP "Common"
		"Common/CommonRender.h"
		"Common/CommonRender.cpp"
		"Common/CryNameR.h"
	SOURCE_GROUP "GraphicsPipeline"
		"GraphicsPipeline/StandardGraphicsPipeline.cpp"
		"GraphicsPipeline/StandardGraphicsPipeline.h"
	SOURCE_GROUP "GraphicsPipeline\\\\Common"
		"GraphicsPipeline/Common/GraphicsPipeline.cpp"
		"GraphicsPipeline/Common/GraphicsPipeline.h"
		"GraphicsPipeline/Common/GraphicsPipelineStage.cpp"
		"GraphicsPipeline/Common/GraphicsPipelineStage.h"
	SOURCE_GROUP "DeviceManager"
		"DeviceManager/DeviceObjects.h"
		"DeviceManager/Deviceobjects.cpp"
		"DeviceManager/DeviceResources.cpp"
		"DeviceManager/DeviceResources.hpp"
)

add_sources(
	"NoUberFile"
	PROJECTS
	RendererDX11
	SOURCE_GROUP "D3D11"
		"D3D11/BufferManager.cpp"
		"D3D11/CMakeLists.txt"
		"D3D11/DDSTextureLoader11.cpp"
		"D3D11/DDSTextureLoader11.h"
		"D3D11/Renderer.cpp"
		"D3D11/Renderer.h"
		"D3D11/Shader.cpp"
		"D3D11/Shader.hpp"
)

add_sources(
	"NoUberFile"
	PROJECTS
	RendererDX12
	SOURCE_GROUP "D3D12"
		"D3D12/BufferManager.cpp"
		"D3D12/CMakeLists.txt"
		"D3D12/DDSTextureLoader11.cpp"
		"D3D12/DDSTextureLoader11.h"
		"D3D12/Renderer.cpp"
		"D3D12/Renderer.h"
		"D3D12/Shader.cpp"
		"D3D12/Shader.hpp"
)

add_sources(
	"NoUberFile"
	PROJECTS
	RendererVK
	SOURCE_GROUP "Vulkan"
		"Vulkan/Renderer.cpp"
		"Vulkan/Shader.cpp"
		"Vulkan/Renderer.h"
)
sources_platform(WINDOWS)
add_sources("NoUberFile" PROJECTS RendererVK SOURCE_GROUP
						"Vulkan\\\\Resource Files" "Vulkan/RendererVK.rc")

end_sources()

macro(add_common_libs)
	target_link_libraries(${THIS_PROJECT} PRIVATE Math)
	target_link_libraries(${THIS_PROJECT} PRIVATE freetype)
	target_link_libraries(${THIS_PROJECT} PRIVATE freeimage::FreeImage freeimage::FreeImagePlus)
	target_link_libraries(${THIS_PROJECT} PRIVATE Utils)
	target_link_libraries(${THIS_PROJECT} PRIVATE FxParser)
	add_sdl(${THIS_PROJECT})
endmacro()

macro(add_common_includes)
	target_include_directories(${THIS_PROJECT} PRIVATE ${Stb_INCLUDE_DIR})
endmacro()

if (FALSE)
	message(STATUS "Using D3D12 Renderer Backend")
	enginemodule(RendererDX12 FORCE_SHARED_WIN PCH
							 "${CMAKE_CURRENT_SOURCE_DIR}/pch.hpp" SOLUTION_FOLDER "Engine")
	add_common_libs()
	add_common_includes()

	target_compile_definitions(RendererDX12 PUBLIC "DX_RENDERER")
	target_compile_definitions(RendererDX12 PUBLIC "BB_RENDERER_DIRECT3D=120")
	target_link_libraries(${THIS_PROJECT} PRIVATE dxguid d3d12 d3d11 d3d10 d3d9 d3dcompiler)
	add_library(Renderer ALIAS RendererDX12)
endif()
if(OPTION_DX_RENDERER)
	message(STATUS "Using D3D11 Renderer Backend")
	enginemodule(RendererDX11 FORCE_SHARED_WIN PCH
							 "${CMAKE_CURRENT_SOURCE_DIR}/pch.hpp" SOLUTION_FOLDER "Engine")
	add_common_libs()
	add_common_includes()

	target_compile_definitions(RendererDX11 PUBLIC "DX_RENDERER")
	target_compile_definitions(RendererDX11 PUBLIC "BB_RENDERER_DIRECT3D=110")
	target_link_libraries(RendererDX11 PRIVATE dxguid d3d11 d3d10 d3d9 d3dcompiler)
	add_library(Renderer ALIAS RendererDX11)
else()
	cryfilecontainer(RendererDX11 SOLUTION_FOLDER "Engine")
endif()
if(OPTION_VK_RENDERER)
	enginemodule(RendererVK FORCE_SHARED_WIN PCH "pch.hpp" SOLUTION_FOLDER
							 "Engine")
	add_common_libs()
	add_common_includes()

	find_package(Vulkan REQUIRED)
	target_link_libraries(${THIS_PROJECT} PRIVATE Vulkan::Vulkan)
	target_compile_definitions(${THIS_PROJECT} PUBLIC VK_RENDERER)
	target_include_directories(${THIS_PROJECT} PUBLIC "Vulkan")
else()
	cryfilecontainer(RendererVK SOLUTION_FOLDER "Engine")
endif()

