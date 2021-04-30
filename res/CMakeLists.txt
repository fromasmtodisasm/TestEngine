#project(resources LANGUAGES)

#add_subdirectory(shaders)

#FILE(GLOB_RECURSE SOURCES "./*")
##FILE(GLOB_RECURSE SOURCES_toolchain "toolchain/*.cmake")
##FILE(GLOB_RECURSE SOURCES_modules "modules/*.cmake")

#source_group("Data" FILES ${SOURCES})
##source_group("Toolchain" FILES ${SOURCES_toolchain})
##source_group("Modules" FILES ${SOURCES_modules})

#set( DATA_SOURCES
#	${SOURCES}
#)
#add_custom_target( Data SOURCES ${DATA_SOURCES})
#set_property(TARGET Data PROPERTY FOLDER "${VS_FOLDER_PREFIX}/Data")

file(GLOB_RECURSE res_files ${CMAKE_SOURCE_DIR}/res/*)

source_group(
  TREE ${CMAKE_SOURCE_DIR}/res
  #PREFIX Resources 
  FILES
  ${res_files}
  )

add_custom_target( Data SOURCES ${res_files})
set_property(TARGET Data PROPERTY FOLDER "Data")
