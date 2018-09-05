cmake_minimum_required(VERSION 2.8.10)

project(IntensitySegmenter)

if( IntensitySegmenter_BUILD_SLICER_EXTENSION )
  #-----------------------------------------------------------------------------
  # Extension meta-information
  set(EXTENSION_HOMEPAGE "http://www.nitrc.org/projects/dentaltools")
  set(EXTENSION_CATEGORY "Segmentation")
  set(EXTENSION_CONTRIBUTORS "Francois Budin (UNC), Beatriz Paniagua (UNC), Pengdong Xiao (UNC)")
  set(EXTENSION_DESCRIPTION "This is an example of a simple extension")
  set(EXTENSION_ICONURL "http://wiki.slicer.org/slicerWiki/images/f/f6/IntensitySegmenterIcon.png")
  set(EXTENSION_SCREENSHOTURLS "http://wiki.slicer.org/slicerWiki/images/7/7c/IntensitySegmenter-OutputScreenshot.png")
  #-----------------------------------------------------------------------------
  # Extension dependencies
  find_package( Slicer REQUIRED )
  include( ${Slicer_USE_FILE} )
else()
  set( STATIC "EXECUTABLE_ONLY" )
  find_package( SlicerExecutionModel REQUIRED )
  include( ${SlicerExecutionModel_USE_FILE} )
  find_package( ITK REQUIRED )
  include( ${ITK_USE_FILE} )
endif()

set( MyModule_NAME IntensitySegmenter )

set(MyModule_SOURCE
    ${MyModule_NAME}.cxx )

SEMMacroBuildCLI(
  NAME ${MyModule_NAME}
  ${STATIC}
  TARGET_LIBRARIES ${ITK_LIBRARIES}
  )

option(BUILD_TESTING "Build the testing tree" ON)
if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()

if( IntensitySegmenter_BUILD_SLICER_EXTENSION )
  include(${Slicer_EXTENSION_CPACK})
endif()
