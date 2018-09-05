include(ExternalData)

set(CLP IntensitySegmenter )

add_executable(${CLP}Test ${CLP}Test.cxx)
target_link_libraries(${CLP}Test ${CLP}Lib )
set_target_properties(${CLP}Test PROPERTIES LABELS ${CLP})


# Add standard remote object stores to user's
# configuration.
list(APPEND ExternalData_URL_TEMPLATES
  "http://slicer.kitware.com/midas3/api/rest?method=midas.bitstream.download&checksum=%(hash)"
  )

Add_Test(  NAME ${CLP}TestHelp
           COMMAND $<TARGET_FILE:${CLP}> --help
        )

Add_Test(  NAME ${CLP}TestSaveDefaultRange
           COMMAND $<TARGET_FILE:${CLP}> -s ${CMAKE_CURRENT_BINARY_DIR}/defaultRange.txt
        )

set( output${CLP}TestRunDefaultOutputVolume ${CMAKE_CURRENT_BINARY_DIR}/SegmentedPreDentalSurgery.nii.gz )
ExternalData_Add_Test( ${CLP}Data
  NAME ${CLP}TestRunDefaultParameters
  COMMAND $<TARGET_FILE:${CLP}Test> --compare DATA{Data/Segmentation_presurg_IntensitySegmenterDefaultRange.nii.gz}
  ${output${CLP}TestRunDefaultOutputVolume}
  --compareIntensityTolerance 0
  --compareRadiusTolerance 0
  --compareNumberOfPixelsTolerance 0
  ModuleEntryPoint
  -i DATA{Data/Greyscale_presurg.gipl.gz}
  -o ${output${CLP}TestRunDefaultOutputVolume}
  )

set( ${CLP}TestFloatIntensityRangeOutputVolume ${CMAKE_CURRENT_BINARY_DIR}/TestFloatIntensityRangeOutputVolume.nrrd )
ExternalData_Add_Test( ${CLP}Data
  NAME ${CLP}TestFloatIntensityRange
  COMMAND $<TARGET_FILE:${CLP}Test> --compare DATA{Data/TestFloatIntensityRangeOutputVolume.nrrd}
  ${${CLP}TestFloatIntensityRangeOutputVolume}
  --compareIntensityTolerance 0
  --compareRadiusTolerance 0
  --compareNumberOfPixelsTolerance 0
  ModuleEntryPoint
  -i DATA{Data/TestFloatIntensityRangeInputVolume.nrrd}
  -o ${${CLP}TestFloatIntensityRangeOutputVolume}
  -r ${CMAKE_CURRENT_SOURCE_DIR}/Data/TestFloatIntensityRange.txt
  )


set( ${CLP}DefaultLabelValueOutputVolume ${CMAKE_CURRENT_BINARY_DIR}/TestDefaultLabelValueOutputVolume.nrrd )
ExternalData_Add_Test( ${CLP}Data
  NAME ${CLP}TestDefaultLabelValue
  COMMAND $<TARGET_FILE:${CLP}Test> --compare DATA{Data/TestDefaultLabelValueOutputVolume.nrrd}
  ${${CLP}DefaultLabelValueOutputVolume}
  --compareIntensityTolerance 0
  --compareRadiusTolerance 0
  --compareNumberOfPixelsTolerance 0
  ModuleEntryPoint
  -i DATA{Data/Greyscale_presurg.gipl.gz}
  -o ${${CLP}DefaultLabelValueOutputVolume}
  -r ${CMAKE_CURRENT_SOURCE_DIR}/Data/RangeWithHoles-defaultLabelTest.txt
  -d 1
  )

############ Tests failing on purpose #######################
set( ${CLP}TestSaveDefaultRangeSpecifyInputAndOutput ${CMAKE_CURRENT_BINARY_DIR}/TestSaveDefaultRangeSpecifyInputAndOutput.nrrd )
ExternalData_Add_Test( ${CLP}Data
  NAME ${CLP}TestSaveDefaultRangeSpecifyInputAndOutput
  COMMAND $<TARGET_FILE:${CLP}> -s ${CMAKE_CURRENT_BINARY_DIR}/defaultRange.txt -i DATA{Data/Greyscale_presurg.gipl.gz} -o ${${CLP}TestSaveDefaultRangeSpecifyInputAndOutput}
  )
set_tests_properties(${CLP}TestSaveDefaultRangeSpecifyInputAndOutput PROPERTIES WILL_FAIL true)

set( ${CLP}TestFailingNoInput ${CMAKE_CURRENT_BINARY_DIR}/TestFailingNoInput.nrrd )
Add_Test( NAME ${CLP}TestNoInput
          COMMAND $<TARGET_FILE:${CLP}> -o ${${CLP}TestFailingNoInput}
        )
set_tests_properties(${CLP}TestNoInput PROPERTIES WILL_FAIL true)

ExternalData_Add_Test( ${CLP}Data
  NAME ${CLP}TestNoOutput
  COMMAND $<TARGET_FILE:${CLP}> -i DATA{Data/Greyscale_presurg.gipl.gz}
  )
set_tests_properties(${CLP}TestNoOutput PROPERTIES WILL_FAIL true)
#############################################################


# Add a build target to populate the real data.
ExternalData_Add_Target( ${CLP}Data )
