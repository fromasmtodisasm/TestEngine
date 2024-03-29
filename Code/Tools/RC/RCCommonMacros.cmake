function(add_prefix list_name prefix)
	set( ${list_name}_TMP )
	foreach(s ${${list_name}})
		list(APPEND ${list_name}_TMP "${prefix}${s}")
	endforeach()
	set( ${list_name} ${${list_name}_TMP} PARENT_SCOPE)
endfunction()

function(add_suffix list_name suffix)
	set( ${list_name}_TMP )
	foreach(s ${${list_name}})
		list(APPEND ${list_name}_TMP "${s}${suffix}")
	endforeach()
	set( ${list_name} ${${list_name}_TMP} PARENT_SCOPE)
endfunction()

function(make_library list_name path)
	set( ${list_name}_TMP )
	foreach(s ${${list_name}})
		list(APPEND ${list_name}_TMP "${path}${s}.lib")
	endforeach()
	set( ${list_name} ${${list_name}_TMP} PARENT_SCOPE)
endfunction()

macro(set_libpath_flag)
	set(LIBPATH_FLAG " /LIBPATH:")
endmacro()

macro(USE_MSVC_PRECOMPILED_HEADER TargetProject PrecompiledHeader PrecompiledSource)
	if (OPTION_PCH AND MSVC)
		set_target_properties(${TargetProject} PROPERTIES COMPILE_FLAGS "/Yu\"${PrecompiledHeader}\"")
		set_source_files_properties(${PrecompiledSource} PROPERTIES COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\"")
		# Disable Precompiled Header on all C files
		foreach(C_sourcefile ${SOURCES})
			if ("${C_sourcefile}" MATCHES ".*\\.\\c$")
				set_property(SOURCE "${C_sourcefile}" APPEND_STRING PROPERTY COMPILE_FLAGS " /Y- ")
			endif ()
		endforeach(C_sourcefile)
	endif()
endmacro()

MACRO(EXCLUDE_FILE_FROM_MSVC_PRECOMPILED_HEADER)
	if (MSVC)
		set_property( SOURCE ${ARGN} APPEND PROPERTY COMPILE_FLAGS "/Y-")
	endif()
ENDMACRO(EXCLUDE_FILE_FROM_MSVC_PRECOMPILED_HEADER)

# Organize projects into solution folders
macro(set_solution_folder folder target)
	if(TARGET ${target})
		set_property(TARGET ${target} PROPERTY FOLDER "${folder}")
	endif()
endmacro()

MACRO(SET_PLATFORM_TARGET_PROPERTIES TargetProject)
	target_compile_definitions( ${THIS_PROJECT} PRIVATE "-DCODE_BASE_FOLDER=\"${CRYENGINE_SOURCE_DIR}/Code/\"")
	target_link_libraries( ${THIS_PROJECT} PRIVATE ${COMMON_LIBS} )

	get_target_property(archout ${TargetProject} ARCHIVE_OUTPUT_DIRECTORY)
	get_target_property(libout ${TargetProject} LIBRARY_OUTPUT_DIRECTORY)
	get_target_property(runout ${TargetProject} RUNTIME_OUTPUT_DIRECTORY)

	foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
		string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
		if (archout)
			set_target_properties(${TargetProject} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${archout}")
		endif()
		if (libout)
			set_target_properties(${TargetProject} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${libout}")
		endif()
		if (runout)
			set_target_properties(${TargetProject} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${runout}")
		endif()
	endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

ENDMACRO(SET_PLATFORM_TARGET_PROPERTIES)

# Process source files
macro(start_sources)
	set(SOURCES)
	set(SOURCE_GROUPS)
	set(UBERFILES)
endmacro()

#Specifies a set of platforms that should build source files provided after this point. Specify ALL to build for all platforms.
#Syntax: sources_platform([OR] X Y Z [AND A B C])
#Subsequent source files will be built if "if(T)" is true for at least one token T in OR and all tokens T in AND.
#Example: sources_platform(WINDOWS ANDROID AND HAS_FOO) = build if target is Windows or Android, and HAS_FOO is true.
macro(sources_platform)
	set(PLATFORM_CONDITION)
	set(multiValueArgs OR AND)
	cmake_parse_arguments(COND "" "" "${multiValueArgs}" ${ARGN})
	list(APPEND COND_OR ${COND_UNPARSED_ARGUMENTS})
	set(PLATFORM_CONDITION FALSE)
	foreach(c ${COND_OR})
		if(${c} STREQUAL "ALL")
			set(c TRUE)
		endif()
		if(${c})
			set(PLATFORM_CONDITION TRUE)
		endif()
	endforeach()
	foreach(c ${COND_AND})
		if(${c} STREQUAL "ALL")
			set(c TRUE)
		endif()
		if(NOT ${c})
			set(PLATFORM_CONDITION FALSE)
		endif()
	endforeach()	
endmacro()

macro(add_files)
	foreach(p ${UB_PROJECTS})
		list(APPEND ${p}_SOURCES ${ARGN})
	endforeach()
	list(APPEND SOURCES ${ARGN})
endmacro()

macro(add_to_uberfile uberfile)
	if(OPTION_UNITY_BUILD AND NOT "${uberfile}" STREQUAL "NoUberFile")
		list(APPEND ${uberfile} ${ARGN})
	endif()		
	add_files(${ARGN})
endmacro()

#Usage: add_sources(uberFileName [PROJECTS proj1 proj2 ...] (SOURCE_GROUP "GroupName" file1 file2...)+
#Use "NoUberFile" to signify source files which should not be compiled with others.
#PROJECTS is optional, but should be used when building multiple projects with distinct sets of source files from one directory. If a project is never referenced in PROJECTS, it will use all sources given in this directory.
#Added files will be built if building for a platform matching the latest sources_platform call. If the files should not be built, they are allowed to be missing on the filesystem.
macro(add_sources name)
	set(multiValueArgs PROJECTS SOURCE_GROUP)
	cmake_parse_arguments(UB "" "" "${multiValueArgs}" ${ARGN})

	if(NOT "${name}" STREQUAL "NoUberFile")
		list(APPEND UBERFILES ${name})
		set(${name}_PROJECTS ${UB_PROJECTS})
	endif()

	#Parse source groups manually to avoid conflating multiple groups
	set(CURRENT_SOURCE_GROUP)
	set(EXPECTING_FILE FALSE)
	set(EXPECTING_GROUP_NAME FALSE)
	foreach(ARG ${ARGN})
		if(${ARG} STREQUAL "PROJECTS")
			set(EXPECTING_FILE FALSE)
		elseif(${ARG} STREQUAL "SOURCE_GROUP")
			set(EXPECTING_GROUP_NAME TRUE)
		elseif(EXPECTING_GROUP_NAME)
			set(CURRENT_SOURCE_GROUP ${ARG})
			string(REPLACE " " "_" CURRENT_SOURCE_GROUP_VAR ${CURRENT_SOURCE_GROUP})
			list(FIND SOURCE_GROUPS ${CURRENT_SOURCE_GROUP_VAR} GROUP_INDEX)			
			if(GROUP_INDEX EQUAL -1)
				list(APPEND SOURCE_GROUPS ${CURRENT_SOURCE_GROUP_VAR})
				set(SOURCE_GROUP_${CURRENT_SOURCE_GROUP_VAR})
			endif()
			set(EXPECTING_GROUP_NAME FALSE)
			set(EXPECTING_FILE TRUE)
		elseif(EXPECTING_FILE)
			if(NOT CURRENT_SOURCE_GROUP)
				message(FATAL_ERROR "No source group name defined")
			endif()
			list(APPEND SOURCE_GROUP_${CURRENT_SOURCE_GROUP_VAR} ${ARG})
			if(NOT ${CURRENT_SOURCE_GROUP} STREQUAL "Root")
				source_group(${CURRENT_SOURCE_GROUP} FILES ${ARG})
			endif()
			# .mm files are Objective-C; disable those from build on non-Apple
			if(NOT (${PLATFORM_CONDITION}) OR (NOT APPLE AND "${ARG}" MATCHES ".*\\.\\mm$"))
				if (EXISTS "${ARG}")
					set_source_files_properties(${ARG} PROPERTIES HEADER_FILE_ONLY TRUE)
					add_files(${ARG})
				endif()
			else()
				add_to_uberfile(${name} ${ARG})
			endif()
		endif()
	endforeach()
endmacro()

macro(get_source_group output group)
	string(REPLACE " " "_" group_var ${group})	
	set(${output} ${SOURCE_GROUP_${group_var}})
endmacro()

macro(end_sources)
	if(OPTION_UNITY_BUILD AND UBERFILES)
		list(REMOVE_DUPLICATES UBERFILES)
		foreach(u ${UBERFILES})
			set(UB_PROJECTS ${${u}_PROJECTS})
			if(UB_PROJECTS)
				list(REMOVE_DUPLICATES UB_PROJECTS)
			endif()
			list(LENGTH UB_PROJECTS projcount)
			if(projcount GREATER 1)
				set(UB_PROJECTS_TEMP ${UB_PROJECTS})
				foreach(UB_PROJECTS ${UB_PROJECTS_TEMP})
					if(${u})
						enable_unity_build(${UB_PROJECTS}_${u} ${u})
						add_files(${UB_PROJECTS}_${u})
					endif()
				endforeach()
			else()
				if(${u})
					enable_unity_build(${u} ${u})
					add_files(${u})
				endif()
			endif()
		endforeach()
	endif()
endmacro()

macro(read_settings)
	#set(options DISABLE_MFC)
	set(oneValueArgs SOLUTION_FOLDER PCH)
	set(multiValueArgs FILE_LIST INCLUDES LIBS DEFINES)
	cmake_parse_arguments(MODULE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
endmacro()

macro(prepare_project)
	set(THIS_PROJECT ${target} PARENT_SCOPE)
	set(THIS_PROJECT ${target})
	include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
	project(${target})
	read_settings(${ARGN})
	if(NOT ${THIS_PROJECT}_SOURCES)
		set(${THIS_PROJECT}_SOURCES ${SOURCES})
	endif()	
endmacro()

macro(apply_compile_settings)
	if (MODULE_PCH)
		string(REPLACE ".cpp" ".h" PCH_H ${MODULE_PCH})
		get_filename_component(PCH_H ${PCH_H} NAME)
		USE_MSVC_PRECOMPILED_HEADER( ${THIS_PROJECT} ${PCH_H} ${MODULE_PCH} )
		set_property(TARGET ${THIS_PROJECT} APPEND PROPERTY AUTOMOC_MOC_OPTIONS -b ${PCH_H})
	endif()
	
	SET_PLATFORM_TARGET_PROPERTIES( ${THIS_PROJECT} )	
	if(MODULE_SOLUTION_FOLDER)
		set_solution_folder("${MODULE_SOLUTION_FOLDER}" ${THIS_PROJECT})
	endif()	

	get_target_property(target_type ${THIS_PROJECT} TYPE)
	if (target_type MATCHES "EXECUTABLE")
		target_compile_options(${THIS_PROJECT} PRIVATE -DCRY_IS_APPLICATION)
	endif()
endmacro()

macro(set_rc_flags)
	target_compile_definitions( ${THIS_PROJECT} PRIVATE
		-DRESOURCE_COMPILER
		-DFORCE_STANDARD_ASSERT
		-DNOT_USE_CRY_MEMORY_MANAGER
	)
    if (WIN32)
		target_compile_definitions(${THIS_PROJECT} PRIVATE -DWIN32)
	endif()
	target_include_directories( ${THIS_PROJECT} PRIVATE
		${CRYENGINE_SOURCE_DIR}/Code/CryEngine/CryCommon 
		${SDK_DIR}/boost
		${CRYENGINE_SOURCE_DIR}/Code/Sandbox/Plugins/EditorCommon 
	)
endmacro()

function(CryResourceCompiler target)
	prepare_project(${ARGN})
	add_executable(${THIS_PROJECT} WIN32 ${${THIS_PROJECT}_SOURCES})
	set_rc_flags()
	if (WIN32)
		set_property(TARGET ${THIS_PROJECT} APPEND_STRING PROPERTY LINK_FLAGS " /SUBSYSTEM:CONSOLE")
	endif()
	set_property(TARGET ${THIS_PROJECT} PROPERTY LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY}")
	set_property(TARGET ${THIS_PROJECT} PROPERTY RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY}")	
	#set_property(TARGET ${THIS_PROJECT} PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY})
	set_property(TARGET ${THIS_PROJECT} PROPERTY OUTPUT_NAME "rc")
	apply_compile_settings()	
endfunction()

function(CommonLib target)
	prepare_project(${ARGN})
	add_library(${THIS_PROJECT} STATIC ${${THIS_PROJECT}_SOURCES})
	set_property(TARGET ${THIS_PROJECT} PROPERTY LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY}")
	set_property(TARGET ${THIS_PROJECT} PROPERTY RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY}")
	#set_property(TARGET ${THIS_PROJECT} PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY})
	apply_compile_settings()	
    
    set_property(TARGET ${THIS_PROJECT} APPEND PROPERTY AUTOMOC_MOC_OPTIONS -b ${PCH_H})
endfunction()

function(CryResourceCompilerModule target)
	prepare_project(${ARGN})
	add_library(${THIS_PROJECT} ${${THIS_PROJECT}_SOURCES})
	set_rc_flags()
	set_property(TARGET ${THIS_PROJECT} PROPERTY LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY}")
	set_property(TARGET ${THIS_PROJECT} PROPERTY RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY}")
	#set_property(TARGET ${THIS_PROJECT} PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY})
	apply_compile_settings()
endfunction()

function(CryEngineStaticModule target)
	prepare_project(${ARGN})
	add_library(${THIS_PROJECT} STATIC ${${THIS_PROJECT}_SOURCES})
	apply_compile_settings()
endfunction()

function(CryConsoleApplication target)
	prepare_project(${ARGN})
	add_executable(${THIS_PROJECT} ${${THIS_PROJECT}_SOURCES})
	if (WIN32)
		set_property(TARGET ${THIS_PROJECT} APPEND_STRING PROPERTY LINK_FLAGS " /SUBSYSTEM:CONSOLE")
	endif()
	apply_compile_settings()
endfunction()

function(CryFileContainer target)
	set(THIS_PROJECT ${target} PARENT_SCOPE)
	set(THIS_PROJECT ${target})
	project(${target})

	read_settings(${ARGN})
	if(NOT ${THIS_PROJECT}_SOURCES)
		set(${THIS_PROJECT}_SOURCES ${SOURCES})
	endif()

	add_custom_target( ${THIS_PROJECT} SOURCES ${${THIS_PROJECT}_SOURCES})
	if(MODULE_SOLUTION_FOLDER)
		set_solution_folder("${MODULE_SOLUTION_FOLDER}" ${THIS_PROJECT})
	endif()
endfunction()

# Common library requirements
macro(use_fbx_sdk)
	target_include_directories(${THIS_PROJECT} PRIVATE "${SDK_DIR}/FbxSdk/include")
	target_compile_definitions(${THIS_PROJECT} PRIVATE -DFBXSDK_NEW_API=1 -DTOOLS_ENABLE_FBX_SDK)
	if (MSVC_VERSION GREATER 1900) # Visual Studio > 2015
		set(FBX_SUBFOLDER vs2015)
	elseif (MSVC_VERSION EQUAL 1900) # Visual Studio 2015
		set(FBX_SUBFOLDER vs2015)
	elseif (MSVC_VERSION EQUAL 1800) # Visual Studio 2013
		set(FBX_SUBFOLDER vs2013)
	elseif (MSVC_VERSION EQUAL 1700) # Visual Studio 2012
		set(FBX_SUBFOLDER vs2012)
	endif()

	set(FBX_PATH "${SDK_DIR}/FbxSdk/lib/${FBX_SUBFOLDER}/x64")

	set_libpath_flag()
	set_property(TARGET ${THIS_PROJECT} APPEND_STRING PROPERTY LINK_FLAGS_DEBUG "${LIBPATH_FLAG}\"${FBX_PATH}/debug\"")
	set_property(TARGET ${THIS_PROJECT} APPEND_STRING PROPERTY LINK_FLAGS_RELEASE "${LIBPATH_FLAG}\"${FBX_PATH}/release\"")
	target_link_libraries(${THIS_PROJECT} PRIVATE libfbxsdk-mt)		
endmacro()
