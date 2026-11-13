cmake_minimum_required(VERSION 3.16.3...3.31)

project(IntensitySegmenter)

if( IntensitySegmenter_BUILD_SLICER_EXTENSION )
  #-----------------------------------------------------------------------------
  # Extension meta-information
  set(EXTENSION_HOMEPAGE "https://www.nitrc.org/projects/dentaltools")
  set(EXTENSION_CATEGORY "Segmentation")
  set(EXTENSION_CONTRIBUTORS "Francois Budin (UNC), Beatriz Paniagua (UNC), Pengdong Xiao (UNC)")
  set(EXTENSION_DESCRIPTION "Segments an image according to the intensity value of its voxels (for example Hounsfield units), using default or user-defined intensity ranges.")
  # The icon and the screenshot that were hosted on wiki.slicer.org no longer exist. To restore them, add the
  # images to this repository and reference them through raw.githubusercontent.com:
  # set(EXTENSION_ICONURL "https://raw.githubusercontent.com/DCBIA-OrthoLab/IntensitySegmenter/release/IntensitySegmenter.png")
  # set(EXTENSION_SCREENSHOTURLS "https://raw.githubusercontent.com/DCBIA-OrthoLab/IntensitySegmenter/release/IntensitySegmenter-screenshot.png")
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
